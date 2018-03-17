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

#include "video/videoservicetypes.h"

const auto Ratio16X9 = 16.0 / 9;
const auto Ratio4X3 = 4.0 / 3;
const auto VertZoomRange = 9; //-8 to 9
const auto AllDirZoomRange = 15;
typedef enum
{
	MINUMUM,
	MODE_16_9 = 0,
	MODE_ORIGINAL,
	FULLWIDE,
	MODE_4_3,
	MODE_VERTICALZOOM,
	MODE_ALLDIRECTIONZOOM,
	MODE_32_9,
	MODE_32_12,
	MODE_TWINZOOM,
	MODE_T_MAX
}ARC_MODE_NAME_MAP_T;

class AspectRatioControl
{
public:
	bool scaleWindow(const VideoRect &screenRect, const VideoRect &frameRect,
	                 VideoRect &inputRect, VideoRect &outputRect);
	void applyOverScan(VideoRect& inputRect, const VideoRect& frameRect);
	void setParams(ARC_MODE_NAME_MAP_T currentAspectMode, int32_t allDirZoomHPosition,
	               int32_t allDirZoomHRatio, int32_t allDirZoomVPosition,
	               int32_t allDirZoomVRatio, int32_t vertZoomVRatio, int32_t vertZoomVPosition);
private:
	ARC_MODE_NAME_MAP_T mCurrentAspectMode;

//
	int32_t mAllDirZoomVRatio; //0 to 15
	int32_t mAllDirZoomVPosition; //-15 to +15

	int32_t mAllDirZoomHRatio; //0-15
	int32_t mAllDirZoomHPosition; //-15 to +15

	int32_t mVertZoomVRatio; //-8 to +9
	int32_t mVertZoomVPosition; //-18 or -17 to 18

	bool mJustScan;
};
