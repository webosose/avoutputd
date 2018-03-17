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

#include "picturemode.h"
#include <ls2-helpers/ls2-helpers.hpp>
#include "logging.h"

using namespace pbnjson;
const pbnjson::JValue PictureMode::defaultJson =
		pbnjson::JObject {{"color",55},
		                   {"brightness",50},
		                  {"hSharpness",25},
		                  {"vSharpness",25},
		                  {"sharpness",25},
		                  {"tint",0},
		                  {"contrast",95}};

PictureMode::PictureMode(const JValue modeJson)
{
	setProperties(modeJson);
}

bool PictureMode::setProperties(pbnjson::JValue modeJson)
{
	LSHelpers::JsonParser modeParser {modeJson};
	modeParser.get("contrast", mContrast).optional().min(MIN_BASICPQ_VALUE).max( MAX_BASICPQ_VALUE);
	modeParser.get("brightness", mBrightness).optional().min(MIN_BASICPQ_VALUE).max( MAX_BASICPQ_VALUE);
	modeParser.get("color", mColor).optional().min(MIN_BASICPQ_VALUE).max( MAX_BASICPQ_VALUE);
	modeParser.get("tint", mTint).optional().min(-MAX_TINT_VALUE).max( MAX_TINT_VALUE);
	modeParser.get("sharpness", mSharpness).optional().min(MIN_SHARPNESS).max( MAX_SHARPNESS);
	modeParser.get("hSharpness", mHSharpness).optional().min(MIN_SHARPNESS).max( MAX_SHARPNESS);
	modeParser.get("vSharpness", mVSharpness).optional().min(MIN_SHARPNESS).max( MAX_SHARPNESS);
	FINISH_PARSE_OR_RETURN_FALSE(modeParser);
	return true;
}