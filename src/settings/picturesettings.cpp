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

#include "picturesettings.h"
#include "picturemode.h"
#include "video/videoservice.h"

using namespace LSHelpers;

PictureSettings::PictureSettings(LS::Handle& handle, VideoService& video)
		:mModeData(PictureMode::defaultJson)
		,mAdapter(handle,this, &PictureSettings::handleSubscriptionResponse)
		,mVideoServiceRef(video)
{
	mAdapter.subscribeToSettings(pbnjson::JObject{{"subscribe", true},
	                                              {"category", "picture"}});
}

// This cancels previous fetch call.
void PictureSettings::fetchPictureModeParams(const std::string& modeName)
{
	mAdapter.makeOneCall(pbnjson::JObject {{"dimension",{{"_3dStatus", "2d"},
	                                       {"input", "default"},
	                                       {"pictureMode",modeName}}},
	                                       {"category","picture"}});
};

//Called on both picture mode change and new picture more settings read and picture mode settings change.
void PictureSettings::handleSubscriptionResponse(JValue& settingsRespone)
{
	std::string modeChangeName;
	std::string settingsResponseForMode;
	JsonParser parser {settingsRespone};
	JsonParser dimObj = parser.getObject("dimension");
	JsonParser settingsObj = parser.getObject("settings");
	settingsObj.get("pictureMode",modeChangeName).optional().defaultValue("");

	// When getting response from mode settings read, picture mode is in dimension.pictureMode.
	dimObj.get("pictureMode", settingsResponseForMode).optional().defaultValue("");

	FINISH_PARSE_OR_RETURN_FALSE(dimObj);
	FINISH_PARSE_OR_RETURN_FALSE(parser);
	FINISH_PARSE_OR_RETURN_FALSE(settingsObj);

	if (!modeChangeName.empty() && modeChangeName != mCurrentMode)
	{
		mCurrentMode = modeChangeName;
		// new mode, fetch the settings for it.
		fetchPictureModeParams(modeChangeName);
	}
	else if (!settingsResponseForMode.empty() && settingsResponseForMode == mCurrentMode) // response from new picture mode params query or subscription update
	{
		mModeData.setProperties(settingsObj.getJson());

		//Update Settings that were changed
		if (settingsObj.hasKey("brightness") || settingsObj.hasKey("contrast")
		    ||settingsObj.hasKey("color") || settingsObj.hasKey("tint"))
		{
			mVideoServiceRef.setBasicPictureCtrl(mModeData.brightness(), mModeData.contrast(), mModeData.color(), mModeData.tint());
		}
		if (settingsObj.hasKey("sharpness") || settingsObj.hasKey("hSharpness") ||
		    settingsObj.hasKey("vSharpness"))
		{
			mVideoServiceRef.setSharpness(mModeData.sharpness(), mModeData.hSharpness(), mModeData.vSharpness());
		}
	}
	return;
}
