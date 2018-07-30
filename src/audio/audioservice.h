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
#include "volumeservice.h"
#include <aval_api.h>

using namespace pbnjson;

class AudioConnection
{
public:
	AudioConnection(){};

	std::string source; //ADEC, ..
	std::string sink; // "MAIN", "SUB"
	std::string outputMode; // "tv_speaker"
	std::string audioType; // "media"
	uint8_t sourcePort = 0;
	bool muted = false;

	AVAL_AUDIO_RESOURCE_T audioResourceId = AVAL_AUDIO_RESOURCE_NO_CONNECTION;
};

class AudioService
{
private:
	AVAL* aval = AVAL::getInstance();

public:
	AudioService(LS::Handle &handle, VolumeService& volumeService);
	~AudioService();

	AudioService(const AudioService &) = delete;
	AudioService &operator=(const AudioService &) = delete;

	// Audio methods
	pbnjson::JValue connect(LSHelpers::JsonRequest& request);
	pbnjson::JValue disconnect(LSHelpers::JsonRequest& request);
	pbnjson::JValue mute(LSHelpers::JsonRequest& request);
	pbnjson::JValue getStatus(LSHelpers::JsonRequest& request);

private:
	void sendStatusUpdate();
	JValue buildStatus();
	JValue buildAudioStatus(const AudioConnection& connection);

	AudioConnection* findAudioConnection(const std::string& source, int sourcePort, const std::string& sink);
	void removeAudioConnection(const std::string& source, int sourcePort, const std::string& sink);
	bool doDisconnectAudio(AudioConnection& connection);
	bool doMuteAudio(AudioConnection& connection, bool muted);

	VolumeService& mVolumeService;

	std::vector<AudioConnection> mConnections;
	LSHelpers::ServicePoint mService;
	LSHelpers::SubscriptionPoint mStatusSubscription;
};
