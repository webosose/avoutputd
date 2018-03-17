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

#include "soundqualitysetting.h"

SoundQualitySettings::SoundQualitySettings(LS::Handle& service) :mAdapter(service,this, &SoundQualitySettings::configureSoundControls)
{
	mAdapter.subscribeToSettings(pbnjson::JObject{{"subscribe", true}, {"category","sound"}});
}

void SoundQualitySettings::enableEqualizer(std::string userEqualizer)
{
	LOG_DEBUG("userEqualizer has changed to %s", userEqualizer.c_str());

	bool success = true;
	int32_t uiVal[] = {0,0,0,0,0};

	if (userEqualizer == "on")
	{
		uiVal[0]= mEqualizerSetting100;
		uiVal[1]= mEqualizerSetting300,
		uiVal[2]= mEqualizerSetting1K,
		uiVal[3]= mEqualizerSetting3K,
		uiVal[4]= mEqualizerSetting10K;
	}
	else
	{
		//uival is 0;
	}
	success = aval->controls->configureSoundSettings("setEqualizerValues", uiVal);

	//TODO: Call getSetting for equalizer value (for bootup with 'on' scenario)

	if(!success)
	{
		LOG_WARNING(MSGID_CONFIG_EQUALIZER_ERROR, 0, "Could not configure equalizer control");
	}
}

void SoundQualitySettings::configureUserEqualizerValues(pbnjson::JValue settingsObj)
{
	LOG_DEBUG("userEqualizerValues has changed");

	//0th index refers to 100Hz equalizer value
	settingsObj["userEqualizerValues"][0].asNumber(mEqualizerSetting100);
	LOG_DEBUG("userEqualizerValues 0 has changed to %d", mEqualizerSetting100);

	//1st index refers to 300Hz equalizer value
	settingsObj["userEqualizerValues"][1].asNumber(mEqualizerSetting300);
	LOG_DEBUG("userEqualizerValues 1 has changed to %d", mEqualizerSetting300);

	//2nd index refers to 1KHz equalizer value
	settingsObj["userEqualizerValues"][2].asNumber(mEqualizerSetting1K);
	LOG_DEBUG("userEqualizerValues 2 has changed to %d", mEqualizerSetting1K);

	//3rd index refers to 3KHz equalizer value
	settingsObj["userEqualizerValues"][3].asNumber(mEqualizerSetting3K);
	LOG_DEBUG("userEqualizerValues 3 has changed to %d", mEqualizerSetting3K);

	//4th index refers to 10KHz equalizer value
	settingsObj["userEqualizerValues"][4].asNumber(mEqualizerSetting10K);
	LOG_DEBUG("userEqualizerValues 4 has changed to %d", mEqualizerSetting10K);

	int32_t uiVal[] = {mEqualizerSetting100, mEqualizerSetting300, mEqualizerSetting1K, mEqualizerSetting3K, mEqualizerSetting10K};
	bool success = aval->controls->configureSoundSettings("setEqualizerValues", uiVal);
	if(!success)
	{
		LOG_WARNING(MSGID_CONFIG_EQUALIZER_VALUES_ERROR, 0, "Could not configure equalizer values");
	}
}




void SoundQualitySettings::configureSoundControls(pbnjson::JValue& settingsObj)
{

	if (settingsObj.hasKey("userEqualizerValues"))
	{
		configureUserEqualizerValues(settingsObj);
	}

	if (settingsObj.hasKey("userEqualizer"))
	{
		enableEqualizer(settingsObj["userEqualizer"].asString());
	}

	if (settingsObj.hasKey("volumeControl"))
	{
		aval->controls->configureSoundSettings("configureVolumeControl", settingsObj["volumeControl"].asString());
	}

	if(settingsObj.hasKey("soundMode"))
	{
		aval->controls->configureSoundSettings("configureSoundMode", settingsObj["soundMode"].asString());
	}

	if(settingsObj.hasKey("smartSoundMode"))
	{
		aval->controls->configureSoundSettings("configureSmartSound",settingsObj["smartSoundMode"].asString());
	}

	if (settingsObj.hasKey("soundOutput"))
	{
		aval->controls->configureSoundSettings("configureSoundOutput",settingsObj["soundOutput"].asString());
	}

	if (settingsObj.hasKey("avSync"))
	{
		aval->controls->configureSoundSettings("configureAvSync",settingsObj["avSync"].asString());
	}


}
