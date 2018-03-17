// @@@LICENSE
//
//      Copyright (c) 2016-17 LG Electronics, Inc.
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
#include "video/aspectratiocontrol.h"

class AspectRatioSetting;
class VideoService;

class AspectRatioSetting
{
public:
	AspectRatioSetting(LS::Handle& serviceHandle, VideoService& video);
	void processResponse(pbnjson::JValue& settingsResponse);
	void fetchAspectRatioforApp(std::string& appId);
private:
	SettingsAdapter<AspectRatioSetting> mSettingsAdapter;
	VideoService& mVideoService;

	ARC_MODE_NAME_MAP_T mCurrentAspectMode;

	int32_t mAllDirZoomVRatio; //0 to 15
	int32_t mAllDirZoomVPosition; //-15 to +15

	int32_t mAllDirZoomHRatio; //0-15
	int32_t mAllDirZoomHPosition; //-15 to +15

	int32_t mVertZoomVRatio; //-8 to +9
	int32_t mVertZoomVPosition; //-18 or -17 to 18

	bool mJustScan;

};
