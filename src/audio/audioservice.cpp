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

#include "logging.h"
#include "errors.h"
#include "audioservice.h"

using namespace pbnjson;

AudioService::AudioService(LS::Handle &handle, VolumeService& volumeService)
		: mVolumeService(volumeService)
		, mService(&handle)
{
	mService.registerMethod("/audio", "connect", this, &AudioService::connect);
	mService.registerMethod("/audio", "disconnect", this, &AudioService::disconnect);
	mService.registerMethod("/audio", "getStatus", this, &AudioService::getStatus);
	mService.registerMethod("/audio", "mute", this, &AudioService::mute);
	//TODO::Integration with settings app will require a revisit/verify the output
	//muting sequence
	mVolumeService.unmuteOutputs();

}

AudioService::~AudioService()
{

	for (auto& connection: mConnections)
	{
		doDisconnectAudio(connection);
	}
	mConnections.clear();
}

pbnjson::JValue AudioService::connect(LSHelpers::JsonRequest& request)
{
	std::string sinkName;
	std::string sourceName;
	std::string outputMode;
	std::string audioType;
	uint8_t sourcePort;

	request.get("source", sourceName);
	request.get("sourcePort", sourcePort);
	request.get("sink", sinkName);
	request.get("outputMode", outputMode);
	request.get("audioType", audioType);
	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	LOG_DEBUG("Audio connect request for source %s, sourcePort %d, sink %s, outputMode %s, audioType %s",
			   sourceName.c_str(), sourcePort, sinkName.c_str(), outputMode.c_str(), audioType.c_str() );



	if (sourceName != "ADEC")
	{
		return API_ERROR_INVALID_PARAMETERS("Only ADEC source implemented");
	}

	//TODO: we ignore source ports for now.
	if (sourcePort !=0 && sourcePort !=1 )
	{
		return API_ERROR_INVALID_PARAMETERS("SourcePort must be 0 or 1");
	}

	AVAL_AUDIO_RESOURCE_T audioResourceId = AVAL_AUDIO_RESOURCE_T::AVAL_AUDIO_RESOURCE_ADEC0;

	if (sinkName == "MAIN")
	{
		audioResourceId = AVAL_AUDIO_RESOURCE_MIXER0;
	}
	if (sinkName == "SUB")
	{
		audioResourceId = AVAL_AUDIO_RESOURCE_MIXER1;
	}

	AudioConnection* connection = findAudioConnection(sourceName, sourcePort, sinkName);
	if (!connection)
	{
		connection = &(*mConnections.emplace(mConnections.end(), AudioConnection()));
		connection->sink = sinkName;
		connection->source = sourceName;
		connection->sourcePort = sourcePort;
		connection->audioResourceId = audioResourceId;
	}

	connection->outputMode = outputMode; //TODO::Add output switch/route
	connection->audioType = audioType;
	int16_t port = -1;


	bool success = aval->audio->connectInput(connection->audioResourceId, &port);

	if (success)
	{
		sendStatusUpdate();
		LOG_DEBUG("Audio connect success");

		return JObject{{"mixerDevice", "PseudoMixer"},
		               {"mixerPort", (int)port},
		               {"passthroughDevice", "PassThrough"},
		               {"passthroughPort", (int)port},
		               {"returnValue", true}};
	}
	else
	{
		removeAudioConnection(sourceName, sourcePort, sinkName);
		sendStatusUpdate();
		return API_ERROR_HAL_ERROR;
	}
}

pbnjson::JValue AudioService::disconnect(LSHelpers::JsonRequest& request)
{
	std::string sinkName;
	std::string sourceName;
	int sourcePort;

	request.get("sink", sinkName);
	request.get("source", sourceName);
	request.get("sourcePort", sourcePort);
	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	LOG_DEBUG("Audio disconnect request for source %s, sourcePort %d, sink %s",
			   sourceName.c_str(), sourcePort, sinkName.c_str());

	AudioConnection* connection = findAudioConnection(sourceName, sourcePort, sinkName);
	if (!connection)
	{
		return API_ERROR_AUDIO_NOT_CONNECTED;
	}

	bool success = doDisconnectAudio(*connection);

	removeAudioConnection(sourceName, sourcePort, sinkName);

	sendStatusUpdate();

	if (success)
	{
		LOG_DEBUG("Audio disconnect success");
		return true;
	}
	else
	{
		return API_ERROR_HAL_ERROR;
	}
}

pbnjson::JValue AudioService::mute(LSHelpers::JsonRequest& request)
{
	std::string sinkName;
	std::string sourceName;
	bool muted;
	int sourcePort;

	request.get("sink", sinkName);
	request.get("source", sourceName);
	request.get("sourcePort", sourcePort);
	request.get("mute", muted);
	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	LOG_DEBUG("Audio mute called for source %s, sourcePort %d, sink %s, mute %d",
			   sourceName.c_str(), sourcePort, sinkName.c_str(), muted);

	AudioConnection* connection = findAudioConnection(sourceName, sourcePort, sinkName);

	if (!connection)
	{
		// Not seting status update - no status change
		return API_ERROR_INVALID_PARAMETERS("Not connected");
	}

	if (!doMuteAudio(*connection, muted))
	{
		// Not seting status update - no status change
		return API_ERROR_HAL_ERROR;
	}

	sendStatusUpdate();
	return true;
}

pbnjson::JValue AudioService::getStatus(LSHelpers::JsonRequest& request)
{
	bool subscribe;

	request.get("subscribe", subscribe).optional(true).defaultValue(false);
	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	JValue response = this->buildStatus();
	response.put("subscribed", subscribe);
	response.put("returnValue", true);

	if (subscribe)
	{
		this->mStatusSubscription.post(response);
		this->mStatusSubscription.addSubscription(request);
	}
	return response;
}

void AudioService::sendStatusUpdate()
{
	if (!this->mStatusSubscription.hasSubscribers())
	{
		return;
	}

	JValue response = this->buildStatus();
	response.put("subscribed", true);
	this->mStatusSubscription.post(response);
}

pbnjson::JValue AudioService::buildStatus()
{
	JArray array;
	for (AudioConnection& connection: mConnections)
	{
		array.append(buildAudioStatus(connection));
	}
	return JObject{{"audio", array}};
}

pbnjson::JValue AudioService::buildAudioStatus(const AudioConnection& c)
{
	return JObject{{"sink", c.sink},
	               {"source", c.source },
	               {"sourcePort", c.sourcePort},
	               {"audioType", c.audioType},
	               {"outputMode", c.outputMode},
	               {"muted", c.muted}};
}

AudioConnection* AudioService::findAudioConnection(const std::string& source,
                                                       int sourcePort,
                                                       const std::string& sink)
{
	for (auto iter = mConnections.begin(); iter != mConnections.end(); iter ++)
	{
		AudioConnection& connection = *iter;
		if (connection.sourcePort == sourcePort && connection.source == source && connection.sink == sink)
		{
			return &connection;
		}
	}

	return nullptr;
}


void AudioService::removeAudioConnection(const std::string& source,
                                             int sourcePort,
                                             const std::string& sink)
{
	for (auto iter = mConnections.begin(); iter != mConnections.end(); iter ++)
	{
		AudioConnection& connection = *iter;
		if (connection.sourcePort == sourcePort &&
		    connection.source == source && connection.sink == sink)
		{
			mConnections.erase(iter);
			break;
		}
	}
}

bool AudioService::doDisconnectAudio(AudioConnection& connection)
{
	if (!doMuteAudio(connection, true))
	{
		return false;
	}

	return aval->audio->disconnectInput(connection.audioResourceId);
}

bool AudioService::doMuteAudio(AudioConnection& connection, bool muted)
{
	if (connection.muted == muted)
	{
		return true;
	}

	if (!aval->audio->setMute(connection.audioResourceId, muted))
	{
		return false;
	}

	connection.muted = muted;
	return true;
}
