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