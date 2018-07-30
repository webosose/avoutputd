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

#include "logging.h"
#include "errors.h"

#include "settings/settingsadapter.h"
#include <aval_api.h>

using namespace pbnjson;

class SoundQualitySettings
{
public:
	SoundQualitySettings(LS::Handle& service);
	void configureSoundControls(pbnjson::JValue& settingsObj);

private:
	SettingsAdapter <SoundQualitySettings> mAdapter;
	AVAL* aval = AVAL::getInstance(); //TODO::Directly calls aval at settings level?

	int32_t mEqualizerSetting100=0;
	int32_t mEqualizerSetting300=0;
	int32_t mEqualizerSetting1K=0;
	int32_t mEqualizerSetting3K=0;
	int32_t mEqualizerSetting10K=0;

	//methods
	void configureUserEqualizerValues(pbnjson::JValue settingsObj);
	void enableEqualizer(std::string userEqualizer);
};