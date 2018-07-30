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
