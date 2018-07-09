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

#include <string>
#include <vector>
#include <unordered_map>
#include <ls2-helpers/ls2-helpers.hpp>

#include <aval_api.h>
#include "videoservicetypes.h"
#include "settings/picturesettings.h"
#include "settings/aspectratiosetting.h"

class VideoSink
{
public:
#if defined(UMS_INTERNAL_API_VERSION2)
	VideoSink(const std::string& _name, uint8_t _zorder, AVAL_VIDEO_WID_T wId):
			name(_name),
			sourcePort(0),
			videoSinkId(wId),
			connected(false),
			muted(true),
			fullScreen(false),
			frameRate(0.),
			adaptive(false),
			opacity(255),
			zorder(_zorder)
	{}

	std::string name; // "MAIN", "SUB"
	std::string sourceName; //TODO: use enum?
	uint8_t sourcePort;
	AVAL_VIDEO_WID_T videoSinkId; // 0 = main, 1 = sub
	bool connected;
	bool muted;

	VideoRect frameRect; // video frame size, x,y always 0,
	VideoSize maxUpscaleSize; // max video size on screen if it's bigger than actual size
	VideoSize minDownscaleSize; // min video size on screen if it's smaller than actual size

	bool fullScreen;
	double frameRate;
	VideoRect inputRect; // w=0,h=0 means not set, will use frame rect instead
	VideoRect outputRect; // before scaling applied
	VideoRect realInputRect; // real input rect used
	bool adaptive; //TODO: adaptive variable is not used aval-rpi.

	//Zorder related things.
	uint8_t opacity;
	uint8_t zorder;
};
#else
	VideoSink(const std::string& _name, uint8_t _zorder, AVAL_VIDEO_WID_T wId):
			name(_name),
			sourcePort(0),
			videoSinkId(wId),
			connected(false),
			muted(true),
			fullScreen(false),
			frameRate(0.),
			adaptive(false),
			opacity(255),
			zorder(_zorder)
	{}

	std::string name; // "MAIN", "SUB"
	std::string sourceName; //TODO: use enum?
	uint8_t sourcePort;
	AVAL_VIDEO_WID_T videoSinkId; // 0 = main, 1 = sub
	bool connected;
	bool muted;

	VideoRect frameRect; // video frame size, x,y always 0,
	VideoSize maxUpscaleSize; // max video size on screen if it's bigger than actual size
	VideoSize minDownscaleSize; // min video size on screen if it's smaller than actual size

	bool fullScreen;
	double frameRate;
	VideoRect inputRect; // w=0,h=0 means not set, will use frame rect instead
	VideoRect outputRect; // before scaling applied
	VideoRect realInputRect; // real input rect used
	bool adaptive;
	ScanType scanType;
	std::string contentType;

	//Zorder related things.
	uint8_t opacity;
	uint8_t zorder;
};
#endif

class VideoService
{
public:
	VideoService(LS::Handle& handle);
	~VideoService();
	VideoService(const VideoService &) = delete;
	VideoService& operator=(const VideoService& ) = delete;

public:
// Luna handlers
	pbnjson::JValue connect(LSHelpers::JsonRequest& request);
	pbnjson::JValue disconnect(LSHelpers::JsonRequest& request);
	pbnjson::JValue blankVideo(LSHelpers::JsonRequest& request);
	pbnjson::JValue setDisplayWindow(LSHelpers::JsonRequest& request);
	pbnjson::JValue setMediaData(LSHelpers::JsonRequest& request);
	pbnjson::JValue setCompositing(LSHelpers::JsonRequest& request);
	pbnjson::JValue getVideoLimits(LSHelpers::JsonRequest& request);
	pbnjson::JValue getOutputCapabilities(LSHelpers::JsonRequest& request);
	pbnjson::JValue getStatus(LSHelpers::JsonRequest& request);

	pbnjson::JValue getSupportedResolutions(LSHelpers::JsonRequest& request);
	pbnjson::JValue setDisplayResolution(LSHelpers::JsonRequest& request);


	JValue
	setAspectRatio(ARC_MODE_NAME_MAP_T settings, int32_t i, int32_t i1, int32_t i2, int32_t i3, int32_t i4, int32_t i5);
	inline void setAppIdChangedObserver(AspectRatioSetting* object,
	                             void (AspectRatioSetting::* callbackHandler) (std::string& appId))
	{
		mAppIdChangedNotify = std::bind(callbackHandler, object, std::placeholders::_1);
	};

	pbnjson::JValue setBasicPictureCtrl(int8_t brightness, int8_t contrast, int8_t saturation, int8_t hue);
	pbnjson::JValue setSharpness (int8_t sharpness,int8_t hSharpness, int8_t vSharpness);

private:
	AVAL* aval = AVAL::getInstance();

private:
	VideoSink* getVideoSink(const std::string& sinkName);
	bool setDualVideo(bool enable);

	void sendSinkUpdateToSubscribers();

	void readVideoCapabilities(VideoSink& context);
	bool applyVideoOutputRects(VideoSink& context, VideoRect& inputRect, VideoRect& outputRect, VideoRect& frameRect);
	bool applyVideoFilters(VideoSink& context, const std::string& sourceName);
	bool applyCompositing();

	bool doDisconnectVideo(VideoSink& video);

	bool initI2C();

	pbnjson::JValue buildStatus();
	pbnjson::JValue buildVideoSinkStatus(VideoSink& vsink);

	//void readHdmiTimingInfo(VideoSink& sink);

// Data members
	VideoRect mDisplayResolution;
	std::vector<VideoSink> mSinks;

	LSHelpers::ServicePoint mService;
	LSHelpers::SubscriptionPoint mSinkStatusSubscription;

	bool mDualVideoEnabled;

	AspectRatioControl mAspectRatioControl;

	typedef std::function<void(std::string&)> AppIDChangeSettingsCallback;
	AppIDChangeSettingsCallback mAppIdChangedNotify;
};

class Composition: public JsonDataObject
{
public:
	std::string sink;
	int opacity;
	int zOrder;

	void parseFromJson(const pbnjson::JValue &value) override
	{
		JsonParser parser{value};
		parser.get("sink", sink);
		parser.get("opacity", opacity);
		parser.get("zOrder", zOrder);
		parser.finishParseOrThrow();
	}
};
