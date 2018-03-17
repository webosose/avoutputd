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

#include <string>
#include <pbnjson/cxx/JValue.h>
#include <map>
#include "errors.h"


const uint8_t MIN_SHARPNESS = 0;
const uint8_t MAX_SHARPNESS = 50;

const uint8_t MIN_BASICPQ_VALUE = 0;
const uint8_t MAX_BASICPQ_VALUE = 100;

const uint8_t MAX_TINT_VALUE = 50;

class PictureMode
{

public:

	PictureMode(const pbnjson::JValue modeJson);

	bool setProperties(pbnjson::JValue modeJson);

	int8_t tint() {return mTint;}
	uint8_t color() {return mColor;}
	uint8_t contrast() {return mContrast;}
	uint8_t brightness() {return mBrightness;}

	uint8_t sharpness() {return mSharpness;}
	uint8_t hSharpness() {return mHSharpness;}
	uint8_t vSharpness() {return mVSharpness;}

	const static pbnjson::JValue defaultJson;


private:
	uint8_t mBrightness=MAX_BASICPQ_VALUE;
	uint8_t  mContrast=MAX_BASICPQ_VALUE;

	uint8_t mColor=MAX_BASICPQ_VALUE;

	int8_t mTint=0;

	uint8_t mSharpness=MAX_SHARPNESS;
	uint8_t mHSharpness=MAX_SHARPNESS;
	uint8_t mVSharpness=MAX_SHARPNESS;

};

