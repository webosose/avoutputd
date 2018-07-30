// Copyright (c) 2016-2018 LG Electronics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0


#include <map>
#include "errors.h"
#include "volumeservice.h"
#include <aval_api.h>

using namespace pbnjson;

VolumeService::VolumeService(LS::Handle &handle)
		: mService(&handle)
{
	mService.registerMethod("/audio/volume", "up", this, &VolumeService::up);
	mService.registerMethod("/audio/volume", "down", this, &VolumeService::down);
	mService.registerMethod("/audio/volume", "set", this, &VolumeService::set);
	mService.registerMethod("/audio/volume", "getStatus", this, &VolumeService::getStatus);
	mService.registerMethod("/audio/volume", "muteSpeaker", this, &VolumeService::muteSpeaker);
	mService.registerMethod("/audio/spdif", "setMode", this, &VolumeService::spdifSetMode);

	//Initialize outputs list
	mOutputs.emplace(std::piecewise_construct,
	                 std::forward_as_tuple("tv_speaker"),
	                 std::forward_as_tuple("tv_speaker", &mSpeaker));
	mOutputs.emplace(std::piecewise_construct,
	                 std::forward_as_tuple("spdif"),
	                 std::forward_as_tuple("spdif", &mSpdif));

	//Apply initial volumes, all outputs unmuted
	for (auto& volFuncIter : mOutputs)
	{
		volFuncIter.second.volumeController->init(false, 100);
		volFuncIter.second.userMute = false;
	}

	mSpdif.setSpdifMode(AVAL_AUDIO_SPDIF_AUTO);
}

AudioOutput* VolumeService::findOutput(const std::string &soundOutputType)
{
	auto iter = mOutputs.find(soundOutputType);
	if (iter == mOutputs.end())
	{
		return nullptr;
	}

	return &(*iter).second;
}

pbnjson::JValue VolumeService::set(LSHelpers::JsonRequest& request)
{
	std::string soundOutputType;
	uint8_t volLevel;

	request.get("tvSoundOutput", soundOutputType);
	request.get("volume", volLevel);
	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	if (volLevel > MAX_VOLUME || volLevel < MIN_VOLUME)
	{
		return API_ERROR_INVALID_PARAMETERS("Volume out of range");
	}

	AudioOutput* speaker = findOutput(soundOutputType);

	if (!speaker)
	{
		return API_ERROR_INVALID_SPKTYPE(soundOutputType.c_str());
	}

	if(!speaker->volumeController->setVolume(volLevel))
	{
		return API_ERROR_HAL_ERROR;
	}

	sendStatusUpdate(*speaker);
	return true;
}

pbnjson::JValue VolumeService::up(LSHelpers::JsonRequest& request)
{

	std::string soundOutputType;

	request.get("tvSoundOutput", soundOutputType);
	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	AudioOutput* speaker = findOutput(soundOutputType);

	if (!speaker)
	{
		return API_ERROR_INVALID_SPKTYPE(soundOutputType.c_str());
	}

	auto curVolume = speaker->volumeController->getVolume();

	if (curVolume == MAX_VOLUME)
	{
		return API_ERROR_VOLUME_LIMIT("Volume already at max");
	}

	if(!speaker->volumeController->setVolume(curVolume + 1))
	{
		return API_ERROR_HAL_ERROR;
	}

	sendStatusUpdate(*speaker);
	return true;
}

pbnjson::JValue VolumeService::down(LSHelpers::JsonRequest& request)
{
	std::string soundOutputType;

	request.get("tvSoundOutput", soundOutputType);
	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	AudioOutput* speaker = findOutput(soundOutputType);

	if (!speaker)
	{
		return API_ERROR_INVALID_SPKTYPE(soundOutputType.c_str());
	}

	auto curVolume = speaker->volumeController->getVolume();

	if (curVolume == MIN_VOLUME)
	{
		return API_ERROR_VOLUME_LIMIT("Volume already at min");
	}

	if(!speaker->volumeController->setVolume(curVolume - 1))
	{
		return API_ERROR_HAL_ERROR;
	}

	sendStatusUpdate(*speaker);
	return true;
}

pbnjson::JValue VolumeService::muteSpeaker(LSHelpers::JsonRequest& request)
{
	std::string soundOutputType;
	bool muteFlag = false;

	request.get("tvSoundOutput", soundOutputType);
	request.get("mute", muteFlag);
	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	AudioOutput* speaker = findOutput(soundOutputType);

	if (!speaker)
	{
		return API_ERROR_INVALID_SPKTYPE(soundOutputType.c_str());
	}

	speaker->userMute = muteFlag;

	if(!speaker->volumeController->setMute(mOutputsMuted || speaker->userMute))
	{
		return API_ERROR_HAL_ERROR;
	}

	sendStatusUpdate(*speaker);
	return true;
}

pbnjson::JValue VolumeService::getStatus(LSHelpers::JsonRequest& request)
{
	bool subscribe = false;
	std::string soundOutputType;
	JValue response;

	request.get("subscribe", subscribe).optional(true);
	request.get("tvSoundOutput", soundOutputType).optional(true);
	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	if (soundOutputType.empty())
	{
		response = this->buildAudioStatus();

		if (subscribe)
		{
			mAllOutputsSubscription.addSubscription(request);
		}
	}
	else
	{
		AudioOutput* speaker = findOutput(soundOutputType);

		if (!speaker)
		{
			return API_ERROR_INVALID_SPKTYPE(soundOutputType.c_str());
		}

		response = buildAudioStatus(*speaker);

		if (subscribe)
		{
			speaker->subscription.addSubscription(request);
		}

	}
	response.put("subscribed", subscribe);
	response.put("returnValue", true);
	return response;
}

static std::unordered_map<std::string, AVAL_AUDIO_SPDIF_MODE_T> SPDIF_MODE_MAP =
		{{"pcm", AVAL_AUDIO_SPDIF_PCM},
		 {"auto", AVAL_AUDIO_SPDIF_AUTO}};

pbnjson::JValue VolumeService::spdifSetMode(LSHelpers::JsonRequest& request)
{
	AVAL_AUDIO_SPDIF_MODE_T mode = AVAL_AUDIO_SPDIF_NONE;
	request.getAndMap("mode", mode, SPDIF_MODE_MAP);
	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	AudioOutput* speaker = findOutput("spdif");

	if (!speaker)
	{
		return API_ERROR_INVALID_SPKTYPE("spdif");
	}

	if (!mSpdif.setSpdifMode(mode))
	{
		return API_ERROR_HAL_ERROR;
	}

	sendStatusUpdate(*speaker);
	return true;
}

JValue VolumeService::buildAudioStatus()
{
	JArray status;

	std::string spdifModeName = "unknown";
	auto spdifMode = mSpdif.getSpdifMode();
	for (auto& iter: SPDIF_MODE_MAP)
	{
		if (iter.second == spdifMode)
		{
			spdifModeName = iter.first;
			break;
		}
	}

	for (auto& volFuncIter : mOutputs)
	{
		AudioOutput& output = volFuncIter.second;
		status.append(JObject{{"tvSoundOutput", output.name},
		                      {"volume",        output.volumeController->getVolume()},
		                      {"muted",         output.volumeController->getMute()}});
	}
	return JObject{{"audioStatus",status},
	               {"spdifMode", spdifModeName}};
}

JValue VolumeService::buildAudioStatus(AudioOutput& output)
{
	JArray status;
	status.append(JObject{{"tvSoundOutput", output.name},
	                       {"volume",        output.volumeController->getVolume()},
	                       {"muted",         output.volumeController->getMute()}});

	return JObject{{"audioStatus",status}};
}

void VolumeService::sendStatusUpdate(AudioOutput& output)
{
	//Send subscription response to subscribers monitoring to all speakers
	if (mAllOutputsSubscription.hasSubscribers())
	{
		JValue status = buildAudioStatus();
		status.put("subscribed",true);
		mAllOutputsSubscription.post(status);
	}

	//Send subscription response to subscribers monitoring a specific speakers
	if (output.subscription.hasSubscribers())
	{
		JValue status = buildAudioStatus(output);
		status.put("subscribed",true);
		output.subscription.post(status);
	}
}

void VolumeService::unmuteOutputs()
{
	mOutputsMuted = false;

	for (auto& device: mOutputs)
	{
		device.second.volumeController->setMute(mOutputsMuted || device.second.userMute);
	}
}

void VolumeService::muteOutputs()
{
	mOutputsMuted = true;

	for (auto& device: mOutputs)
	{
		device.second.volumeController->setMute(mOutputsMuted || device.second.userMute);
	}
}
