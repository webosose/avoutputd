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