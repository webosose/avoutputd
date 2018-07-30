/* @@@LICENSE
*
*      Copyright (c) 2016-18 LG Electronics, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* SPDX-License-Identifier: Apache-2.0
* LICENSE@@@ */

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

