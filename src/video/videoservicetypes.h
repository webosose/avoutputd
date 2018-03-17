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
