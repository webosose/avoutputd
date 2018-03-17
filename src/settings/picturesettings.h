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

#include <ls2-helpers/ls2-helpers.hpp>

#include "settingsadapter.h"
#include "picturemode.h"

using namespace pbnjson;

class PictureSettings
{

public:
	PictureSettings(LS::Handle& serviceHandle, VideoService& video);

private:
	void handleSubscriptionResponse(JValue& settingsJValue);
	void fetchPictureModeParams(const std::string& modeName);

	//TODO::Where and how to load the Tables.

	std::string mCurrentMode;
	PictureMode mModeData;

	SettingsAdapter<PictureSettings> mAdapter;
	VideoService& mVideoServiceRef;
};

