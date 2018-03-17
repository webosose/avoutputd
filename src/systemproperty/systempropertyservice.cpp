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

#include "systempropertyservice.h"

SystemPropertyService::SystemPropertyService(LS::Handle& handle)
		: mService(&handle)
{
	mService.registerMethod("/systemproperty", "getProperties", this, &SystemPropertyService::getProperties);
}

SystemPropertyService::~SystemPropertyService()
{
}

pbnjson::JValue SystemPropertyService::getProperties(LSHelpers::JsonRequest& request)
{
	return pbnjson::JObject{
			{"D9", false},
			{"OLED", false},
			{"TVLink", true},
			{"UHD", true},
			{"_3d", false},
			{"builtInSoundbar", false},
			{"cableDigital", false},
			{"cameraReady", "ready"},
			{"ci", false},
			{"ciplus", false},
			{"clearPlus", false},
			{"contiEtc2HourOffUser", false},
			{"contiOpt2PowerOnByVoice", false},
			{"contiVideo14_9", false},
			{"contiVideoFullwide", false},
			{"contiVideoZoom2", true},
			{"digitalEye", "eye_sensor"},
			{"dualView", false},
			{"epa", false},
			{"headphone", true},
			{"instantBooting", "quickStartPlus"},
			{"isf", true},
			{"logolight", false},
			{"movingSpeaker", false},
			{"netflixEsn", "LGTV20162=21000000050"},
			{"panelInch", "55"},
			{"panelUsageTime", "78"},
			{"returnValue", true},
			{"satellite", false},
			{"support3G4GDongle", false},
			{"supportHdmiArc", true},
			{"supportTvSpeakerBar", false},
			{"magicSpaceSound", true},
			{"contiLangCountrySel", "langSelBR"},
			{"tvSystemName", "atsc"}};
}
