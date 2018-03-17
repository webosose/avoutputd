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

#include "videoservicetypes.h"

void VideoRect::parseFromJson(const pbnjson::JValue& value)
{
	LSHelpers::JsonParser::parseValue(value["x"], x);
	LSHelpers::JsonParser::parseValue(value["y"], y);
	LSHelpers::JsonParser::parseValue(value["width"], w);
	LSHelpers::JsonParser::parseValue(value["height"], h);
}

bool VideoRect::contains(VideoRect& inside)
{
	return x <= inside.x &&
	       y <= inside.y &&
	       x + w >= inside.x + inside.w &&
	       y + h >= inside.y + inside.h;
}

pbnjson::JValue VideoRect::toJValue()
{
	return pbnjson::JValue{{"x", this->x},
	                       {"y", this->y},
	                       {"width", this->w},
	                       {"height", this->h}};
}

bool VideoRect::operator==(VideoRect& other)
{
	return x == other.x&& y == other.y && w == other.w && h == other.h;
}
pbnjson::JValue VideoSize::toJValue()
{
	return pbnjson::JValue{{"width", this->w},
	                       {"height", this->h}};
}