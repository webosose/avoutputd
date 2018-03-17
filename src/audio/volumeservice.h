// @@@LICENSE
//
//      Copyright (c) 2016 LG Electronics, Inc.
//
// Confidential computer software. Valid license from LG required for
// possession, use or copying. Consistent with FAR 12.211 and 12.212,
// Commercial Computer Software, Computer Software Documentation, and
// Technical Data for Commercial Items are licensed to the U.S. Government
// under vendor's standard commercial license.
//
// LICENSE@@@

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
