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

#include <cmath>
#include <aval_api.h>
#include <ls2-helpers/ls2-helpers.hpp>
#include <cmath>

enum class ScanType
{
	INTERLACED = 0,
	PROGRESSIVE = 1
};

class VideoSize
{
public:
	VideoSize():w(0),h(0){};
	VideoSize& operator=(const AVAL_VIDEO_SIZE_T &avalSize){
		w = avalSize.w; h = avalSize.h; return *this;
	}
public:
	uint16_t w;
	uint16_t h;

	pbnjson::JValue toJValue();
};

class VideoRect: public LSHelpers::JsonDataObject
{
public:
	VideoRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h):x(x),y(y),w(w),h(h){};
	VideoRect(uint16_t w, uint16_t h):x(0),y(0),w(w),h(h){};
	VideoRect():x(0),y(0),w(0),h(0){};
	//TODO:: Can we Move this to aval or use aval_video_rect
	VideoRect(AVAL_VIDEO_RECT_T avalRect):x(avalRect.x), y(avalRect.y), w(avalRect.w), h(avalRect.h){};
	void parseFromJson(const pbnjson::JValue& value) override;
	pbnjson::JValue toJValue();
	bool contains(VideoRect& inside);

	bool operator== (VideoRect& other);

	VideoRect& operator=(const VideoRect& other)
	{
		x = other.x;
		y = other.y;
		w = other.w;
		h = other.h;

		return *this;
	}

	AVAL_VIDEO_RECT_T toAVALRect()
	{
		return AVAL_VIDEO_RECT_T{x,y,w,h};
	}

	VideoRect scale(double scale)
	{
		return VideoRect((uint16_t)round(x*scale), (uint16_t)round(y*scale), (uint16_t)round(w*scale), (uint16_t)round(h*scale));
	}

	bool isValid() const {return w > 0 && h > 0;}

public:
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
};
