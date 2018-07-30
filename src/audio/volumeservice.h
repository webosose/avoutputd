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

#pragma once

#include <string>
#include <unordered_map>
#include <ls2-helpers/ls2-helpers.hpp>

#include "ivolumecontroller.h"
#include "tvspeakercontroller.h"
#include "spdifcontroller.h"

struct AudioOutput
{
	AudioOutput(const std::string& _name, IVolumeController* _volumeController)
			: name(_name)
			, userMute(true)
			, volumeController(_volumeController)
	{};

	std::string name;
	bool userMute;
	IVolumeController* volumeController;
	LSHelpers::SubscriptionPoint subscription;
};

class VolumeService final
{
public:
	VolumeService(LS::Handle &handle);
	VolumeService(const VolumeService &) = delete;
	VolumeService &operator=(const VolumeService &) = delete;

	// Call after media streams are set up to unmute outputs.
	void unmuteOutputs();

	// Call after media streams are closed to mute outputs.
	void muteOutputs();

private:
	// Luna handlers
	pbnjson::JValue set(LSHelpers::JsonRequest& request);
	pbnjson::JValue up(LSHelpers::JsonRequest& request);
	pbnjson::JValue down(LSHelpers::JsonRequest& request);
	pbnjson::JValue muteSpeaker(LSHelpers::JsonRequest& request);
	pbnjson::JValue getStatus(LSHelpers::JsonRequest& request);
	pbnjson::JValue spdifSetMode(LSHelpers::JsonRequest& request);

	void sendStatusUpdate(AudioOutput& output);

	pbnjson::JValue buildAudioStatus();
	pbnjson::JValue buildAudioStatus(AudioOutput& output);
	AudioOutput* findOutput(const std::string &soundOutputType);

	// Data members
	TvSpeakerController mSpeaker;
	SpdifController mSpdif;

	std::unordered_map<std::string, AudioOutput> mOutputs;
	bool mOutputsMuted;
	LSHelpers::ServicePoint mService;
	LSHelpers::SubscriptionPoint mAllOutputsSubscription;
};
