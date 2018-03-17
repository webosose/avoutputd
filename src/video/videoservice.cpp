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

#include <string>
#include <vector>
#include <unordered_set>
#include <sstream>
#include <aval/aval_video.h>

#include "logging.h"
#include "errors.h"
#include "videoservice.h"

using namespace pbnjson;
using namespace LSHelpers;

VideoService::VideoService(LS::Handle& handle) :
		mService(&handle),
		mDualVideoEnabled(false)
{
	std::vector<AVAL_PLANE_T> supportedPlanes = aval->video->getVideoPlanes();

	//setup the sinks
	for(uint8_t i=0; i<supportedPlanes.size(); i++)
	{
		std::string plane = supportedPlanes[i].planeName;

		if(plane == "MAIN")
			mSinks.push_back(VideoSink(plane, 0, AVAL_VIDEO_WID_0));

		else if(plane == "SUB0")
			mSinks.push_back(VideoSink(plane, 1, AVAL_VIDEO_WID_1));

		else if(plane == "SUB1")
			mSinks.push_back(VideoSink(plane, 2, AVAL_VIDEO_WID_2));

		else if(plane == "SUB2")
			mSinks.push_back(VideoSink(plane, 3, AVAL_VIDEO_WID_3));

		else
			LOG_ERROR(MSGID_SINK_SETUP_ERROR, 0, "Plane %s not supported by service", plane);
	}

	mService.registerMethod("/video", "connect", this, &VideoService::connect);
	mService.registerMethod("/video", "disconnect", this, &VideoService::disconnect);
	mService.registerMethod("/video", "setMediaData", this, &VideoService::setMediaData);
	mService.registerMethod("/video", "getStatus", this, &VideoService::getStatus);
	mService.registerMethod("/video", "blankVideo", this, &VideoService::blankVideo);

	mService.registerMethod("/video/display", "getVideoLimits", this, &VideoService::getVideoLimits);
	mService.registerMethod("/video/display", "getOutputCapabilities", this, &VideoService::getOutputCapabilities);
	mService.registerMethod("/video/display", "setDisplayWindow", this, &VideoService::setDisplayWindow);
	mService.registerMethod("/video/display", "setCompositing", this, &VideoService::setCompositing);
	mService.registerMethod("/video/display", "getSupportedResolutions", this, &VideoService::getSupportedResolutions);
	mService.registerMethod("/video/display", "setDisplayResolution", this, &VideoService::setDisplayResolution);
}

VideoService::~VideoService()
{
	for(auto sink : mSinks)
	{
		doDisconnectVideo(sink);
	}
}

pbnjson::JValue VideoService::connect(LSHelpers::JsonRequest& request)
{
	std::string videoSource, videoSinkName, purpose, appId("unknown");
	uint8_t videoSourcePort, videoSinkPort=-1;

	request.get("appId", appId).optional(true);
	request.get("source", videoSource);
	request.get("sourcePort", videoSourcePort);
	request.get("sink", videoSinkName);

	if (videoSinkName == "MAIN")
	{
		request.get("sinkPort", videoSinkPort).optional(true);
	}
	else
	{
		request.get("sinkPort", videoSinkPort);
		videoSinkName = videoSinkName + std::to_string(videoSinkPort);
	}

	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	LOG_DEBUG("Video connect request for source %s, sourcePort %d, sink %s, sinkPort %d",
	          videoSource.c_str(), videoSourcePort, videoSinkName.c_str(), videoSinkPort);

	VideoSink* videoSink = getVideoSink(videoSinkName);
	AVAL_VSC_INPUT_SRC_INFO_T vscInput;

	if (!videoSink)
	{
		return API_ERROR_INVALID_PARAMETERS("Invalid sink value");
	}

	if (videoSource == "VDEC")
	{
		vscInput.type = AVAL_VSC_INPUTSRC_VDEC;
		vscInput.attr = 1;	//Not used for VDEC
		vscInput.resourceIndex = videoSourcePort;
	}
	else if (videoSource == "RGB") //RGB
	{
		vscInput.type = AVAL_VSC_INPUTSRC_ADC;
		vscInput.attr = 1; // RGB
		vscInput.resourceIndex = 0; // Not used
	}
	else if (videoSource == "HDMI")
	{
		vscInput.type = AVAL_VSC_INPUTSRC_HDMI;
		vscInput.attr = 0;
		vscInput.resourceIndex = videoSourcePort; // HDMI port number
	}
	else
	{
		return API_ERROR_INVALID_PARAMETERS("Only VDEC and HDMI video source currently supported");
	}

	if (videoSinkName == "SUB")
	{
		this->setDualVideo(true);
	}

	if (videoSink->connected)
	{
		doDisconnectVideo(*videoSink);
	}

	unsigned int plane;
	if(!aval->video->connect(videoSink->videoSinkId, vscInput, AVAL_VSC_OUTPUT_DISPLAY_MODE, &plane))
	{
		return API_ERROR_HAL_ERROR;
	}

	this->readVideoCapabilities(*videoSink);

	if (!this->applyVideoFilters(*videoSink, videoSource))
	{
		return API_ERROR_HAL_ERROR;
	}

	videoSink->sourceName = videoSource;
	videoSink->sourcePort = videoSourcePort;
	videoSink->connected = true;

	mAppIdChangedNotify (appId);

	/*if (videoSource == "HDMI")
	{
		readHdmiTimingInfo(*videoSink);
	}*/

	LOG_DEBUG("Video connect success");
	this->sendSinkUpdateToSubscribers();

	return JObject{{"returnValue", true}, {"planeID", (int)plane}};
}

pbnjson::JValue VideoService::getVideoLimits(LSHelpers::JsonRequest& request)
{
	std::string sinkName;

	request.get("sink", sinkName);
	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	VideoSink* videoSink = getVideoSink(sinkName);

	if (!videoSink)
	{
		return API_ERROR_INVALID_PARAMETERS("Invalid sink value");
	}

	if (!videoSink->connected)
	{
		return API_ERROR_VIDEO_NOT_CONNECTED;
	}

	return JObject{{"returnValue", true},
	               {"sink", sinkName},
	               {"displaySize", videoSink->maxUpscaleSize.toJValue()},
	               {"minDownscaleSize", videoSink->minDownscaleSize.toJValue()},
	               {"maxUpscaleSize", videoSink->maxUpscaleSize.toJValue()}};
}

pbnjson::JValue VideoService::getOutputCapabilities (LSHelpers::JsonRequest& request)
{
	std::vector<AVAL_PLANE_T> avalPlanes = aval->video->getVideoPlanes();
	size_t planeCount =avalPlanes.size();
	JArray pJArray;
	for(auto plane: avalPlanes)
	{
		pJArray.append(pbnjson::JValue {{"sinkId",plane.planeName},
		                                {"maxDownscaleSize",pbnjson::JValue{{"width", plane.minSizeT.w},{"height", plane.minSizeT.h}}},
		                                {"maxUpscaleSize",pbnjson::JValue{{"width", plane.maxSizeT.w},{"height", plane.maxSizeT.h}}}});

	}
	return JObject { {"returnValue", true},
	                 {"numPlanes", static_cast<int>(planeCount)},
	                 {"planes", pJArray}};

}

pbnjson::JValue VideoService::disconnect(LSHelpers::JsonRequest& request)
{
	std::string videoSinkName;

	request.get("sink", videoSinkName);
	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	LOG_DEBUG("Video disconnect request for sink %s", videoSinkName.c_str());

	VideoSink* videoSink = getVideoSink(videoSinkName);

	if (!videoSink)
	{
		return API_ERROR_INVALID_PARAMETERS("Invalid sink value");
	}

	if (!videoSink->connected)
	{
		return API_ERROR_VIDEO_NOT_CONNECTED;
	}

	if (!this->doDisconnectVideo(*videoSink))
	{
		return API_ERROR_HAL_ERROR;
	}

	LOG_DEBUG("Video disconnect success");
	this->sendSinkUpdateToSubscribers();
	return true;
}

bool VideoService::doDisconnectVideo(VideoSink& video)
{
	if (!video.connected)
	{
		return true;
	}

	//Clear sate first - will be disconnected even if some calls fail.
	//Allows caller to retry connecting if failed state

	//Reset all video sink related fields
	video.fullScreen = false;
	video.frameRect = VideoRect();
	video.inputRect = VideoRect();
	video.outputRect = VideoRect();
	video.realInputRect = VideoRect();
	video.maxUpscaleSize = VideoSize();
	video.minDownscaleSize = VideoSize();
	video.frameRate = 0;
	video.connected = false;

	bool success = true;
	AVAL_VSC_INPUT_SRC_INFO_T vscInput = {AVAL_VSC_INPUTSRC_AVD,0,0}; // Values not used by disconnect

	success &= aval->video->disconnect(video.videoSinkId, vscInput, AVAL_VSC_OUTPUT_DISPLAY_MODE);

	if (video.videoSinkId != AVAL_VIDEO_WID_0)
	{
		success &= this->setDualVideo(false);
	}

	return success;
}

pbnjson::JValue VideoService::blankVideo(LSHelpers::JsonRequest& request)
{

	std::string sinkName;
	bool enableBlank;

	request.get("sink", sinkName);
	request.get("blank", enableBlank);
	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	LOG_DEBUG("blankVideo request for sink %s, set blank to %d", sinkName.c_str(), enableBlank);

	VideoSink* videoSink = getVideoSink(sinkName);

	if (!videoSink)
	{
		return API_ERROR_INVALID_PARAMETERS("Invalid sink value");
	}

	if(enableBlank && videoSink->muted)
	{
		//Already muted, do nothing
		return true;
	}

	if (!aval->video->setWindowBlanking(videoSink->videoSinkId, enableBlank, videoSink->realInputRect.toAVALRect(), videoSink->outputRect.toAVALRect()))
	{
		return API_ERROR_HAL_ERROR;
	}

	videoSink->muted = enableBlank;
	this->sendSinkUpdateToSubscribers();
	return true;
}

pbnjson::JValue VideoService::setMediaData(LSHelpers::JsonRequest& request)
{
	std::string videoSinkName;
	std::string contentType;
	double frameRate;
	uint16_t width;
	uint16_t height;
	std::string scanType;
	bool adaptive;

	request.get("sink", videoSinkName);
	request.get("contentType", contentType);
	request.get("frameRate", frameRate).min(0.0);
	request.get("width", width);
	request.get("height", height);
	request.get("scanType", scanType).allowedValues({"interlaced", "progressive", "VIDEO_PROGRESSIVE", "VIDEO_INTERLACED"});
	request.get("adaptive", adaptive);
	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	LOG_DEBUG("setMediaData called for sink %s with contentType %s, frameRate %f, width %u, height %u, scanType %s, adaptive %d",
	          videoSinkName.c_str(), contentType.c_str(), frameRate, width, height, scanType.c_str(), adaptive);

	VideoSink* videoSink = getVideoSink(videoSinkName);

	if (!videoSink)
	{
		return API_ERROR_INVALID_PARAMETERS("Invalid sink value");
	}

	if (!videoSink->connected)
	{
		return API_ERROR_VIDEO_NOT_CONNECTED;
	}

	//Just save the frame rect and wait for setDisplayWindow to update output window.

	videoSink->frameRect.w = width;
	videoSink->frameRect.h = height;
	videoSink->inputRect = VideoRect(); // Invalidate input rect, need to call setDisplayWindow to set new input rect.
	videoSink->frameRate = frameRate;
	videoSink->adaptive = adaptive;
	videoSink->scanType = scanType == "progressive" || scanType == "VIDEO_PROGRESSIVE" ?
	                     ScanType::PROGRESSIVE :
	                     ScanType::INTERLACED;
	videoSink->contentType = contentType;

	if (videoSink->outputRect.isValid() || videoSink->fullScreen)
	{//only if setDisplayWindow was called earlier apply Video
		VideoRect input =  videoSink->frameRect;
		VideoRect output = videoSink->outputRect;

		if (videoSink->fullScreen)
		{
			VideoRect sinkWindowSize = VideoRect(videoSink->maxUpscaleSize.w, videoSink->maxUpscaleSize.h);
			mAspectRatioControl.scaleWindow(sinkWindowSize, videoSink->frameRect, input, output);
		}

		this->applyVideoOutputRects(*videoSink, input, output, videoSink->frameRect);

	}

	this->sendSinkUpdateToSubscribers();
	return true;
}

pbnjson::JValue VideoService::setDisplayWindow(LSHelpers::JsonRequest& request)
{
	std::string videoSinkName;
	bool fullScreen;
	bool opacitySet;
	uint8_t opacity;
	VideoRect displayOutput;
	VideoRect sourceInput;

	request.get("sink", videoSinkName);
	request.get("fullScreen", fullScreen);
	request.get("displayOutput", displayOutput).optional(true);
	request.get("sourceInput", sourceInput).optional(true);
	request.get("opacity", opacity).optional(true).defaultValue(0).checkValueRead(opacitySet);
	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	LOG_DEBUG("setDisplayWindow called for sink %s with fullScreen %d, displayOutput {x:%u, y:%u, w:%u, h:%u},"
			          "sourceInput {x:%u, y:%u, w:%u, h:%u}, opacity %u",
	          videoSinkName.c_str(), fullScreen, displayOutput.x, displayOutput.y, displayOutput.w, displayOutput.h,
	          sourceInput.x, sourceInput.y, sourceInput.w, sourceInput.h, opacity);

	VideoSink* videoSink = getVideoSink(videoSinkName);

	if (!videoSink)
	{
		return API_ERROR_INVALID_PARAMETERS("Invalid sink value");
	}

	VideoRect sinkWindowSize = VideoRect(videoSink->maxUpscaleSize.w, videoSink->maxUpscaleSize.h);
	if (fullScreen)
	{
		displayOutput = sinkWindowSize;
	}
	else
	{
		//Scale to emulate 1080p output resolution.
		double outputScaling = videoSink->maxUpscaleSize.h / 1080;
		displayOutput = displayOutput.scale(outputScaling);
	}

	if (!videoSink->connected)
	{
		return API_ERROR_VIDEO_NOT_CONNECTED;
	}
	else if (!sinkWindowSize.contains(displayOutput))
	{
		return API_ERROR_INVALID_PARAMETERS("displayOutput outside screen");
	}
	else if (videoSink->frameRect.isValid() && sourceInput.isValid() && !videoSink->frameRect.contains(sourceInput))
	{
		return API_ERROR_INVALID_PARAMETERS("sourceInput outside video size");
	}
	else if (displayOutput.w == 0 && displayOutput.h == 0)
	{
		return API_ERROR_INVALID_PARAMETERS("need to specify displayOutput when fullscreen = false");
	}
	else if ((displayOutput.w < sourceInput.w && displayOutput.w < videoSink->minDownscaleSize.w) ||
	         (displayOutput.h < sourceInput.h && displayOutput.h < videoSink->minDownscaleSize.h))
	{
		return API_ERROR_DOWNSCALE_LIMIT(
		                   "unable to downscale below %d,%d, requested, %d,%d",
		                   videoSink->minDownscaleSize.w,
		                   videoSink->minDownscaleSize.h,
		                   displayOutput.w,
		                   displayOutput.h);
	}
	else if ((displayOutput.w > sourceInput.w && displayOutput.w > videoSink->maxUpscaleSize.w) ||
	         (displayOutput.h > sourceInput.h && displayOutput.h > videoSink->maxUpscaleSize.h))
	{
		return API_ERROR_UPSCALE_LIMIT(
		                   "unable to upscale above %d,%d, requested, %d,%d",
		                   videoSink->maxUpscaleSize.w,
		                   videoSink->maxUpscaleSize.h,
		                   displayOutput.w,
		                   displayOutput.h);
	}

	// Apply the values
	videoSink->fullScreen = fullScreen;
	VideoRect output = displayOutput;
	if (videoSink->fullScreen)
	{
		mAspectRatioControl.scaleWindow(displayOutput, videoSink->frameRect, sourceInput, output);
	}

	if (!this->applyVideoOutputRects(*videoSink, sourceInput, output, videoSink->frameRect))
	{
		return API_ERROR_HAL_ERROR;
	}

	if (opacitySet)
	{
		videoSink->opacity = opacity;
	}

	this->sendSinkUpdateToSubscribers();
	return true;
}

JValue VideoService::setAspectRatio(ARC_MODE_NAME_MAP_T currentAspectMode, int32_t allDirZoomHPosition,
                                    int32_t allDirZoomHRatio, int32_t allDirZoomVPosition,
                                    int32_t allDirZoomVRatio, int32_t vertZoomVRatio, int32_t vertZoomVPosition)
{
	mAspectRatioControl.setParams(currentAspectMode, allDirZoomHPosition, allDirZoomHRatio, allDirZoomVPosition,
	                                  allDirZoomVRatio, vertZoomVRatio, vertZoomVPosition);

	//TODO:: update to cater to both subSink and mainSink
	VideoSink mainSink = mSinks[0];
	if(mainSink.fullScreen)
	{
		VideoRect input, output;

		VideoRect sinkWindowSize = VideoRect(mainSink.maxUpscaleSize.w, mainSink.maxUpscaleSize.h);
		mAspectRatioControl.scaleWindow(sinkWindowSize, mainSink.frameRect, input, output);

		if (!this->applyVideoOutputRects(mainSink, input, output, mainSink.frameRect))
		{
			return API_ERROR_HAL_ERROR;
		}
	}

	return true;
}

pbnjson::JValue VideoService::setBasicPictureCtrl(int8_t brightness, int8_t contrast, int8_t saturation, int8_t hue)
{
	LOG_DEBUG("set basic pictureControl properties %d %d %d %d", brightness, contrast, saturation, hue);
	int32_t uiVal[] = {brightness,contrast,saturation,hue};
	return aval->controls->configureVideoSettings (PQ_Control, AVAL_VIDEO_WID_1,uiVal );
	//return aval->controls->configureVideoSettings (PQ_Control, AVAL_VIDEO_WID_1, (int32_t[]) {brightness,contrast,saturation,hue});
}

pbnjson::JValue VideoService::setSharpness(int8_t sharpness,int8_t hSharpness, int8_t vSharpness)
{
	LOG_DEBUG("set setSharpness properties %d %d %d", sharpness, hSharpness, vSharpness);

	int32_t uiVal[] = {1, sharpness, hSharpness, vSharpness, 1, 0, 7};
	return aval->controls->configureVideoSettings(SHARPNESS_Control, AVAL_VIDEO_WID_1 , uiVal);
}

pbnjson::JValue VideoService::setCompositing(LSHelpers::JsonRequest& request)
{
	std::vector<Composition> composeOrdering;
	request.getArray("composeOrder", composeOrdering);
	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	int maxZOrder = mSinks.size()-1;
	std::unordered_set<int> uniqueZorders;
	std::unordered_set<std::string> inputSinks;

	//Validate input array of composition objects
	for(Composition& composition : composeOrdering)
	{
		LOG_DEBUG("%s: Sink %s, opacity %d, zorder %d", __func__, composition.sink.c_str(), composition.opacity, composition.zOrder);
		VideoSink* vsink = this->getVideoSink(composition.sink);

		if(!vsink)
		{
			return API_ERROR_INVALID_PARAMETERS("Invalid sink value");
		}

		if(composition.opacity > 255 || composition.opacity < 0 || composition.zOrder > maxZOrder || composition.zOrder < 0)
		{
			return API_ERROR_INVALID_PARAMETERS("Zorder values must be in the range 0-%d and opacity values must be in the range 0-255", maxZOrder);
		}

		//Make sure the given zorders are unique
		if(!(uniqueZorders.insert(composition.zOrder)).second)
		{
			return API_ERROR_INVALID_PARAMETERS("Two windows cannot have the same zOrder");
		}
		inputSinks.insert(composition.sink);
	}

	//Sanity test that no two zorders are the same - out of given sinks and rest of the sinks
	for(auto &sink : mSinks)
	{
		//This sink's zorder is already registered in uniqueZorder
		if(inputSinks.find(sink.name) != inputSinks.end())
			continue;

		if(!(uniqueZorders.insert(sink.zorder)).second)
		{
			return API_ERROR_INVALID_PARAMETERS("Two windows cannot have the same zOrder");
		}
	}

	//Input is good - set the zorders
	for(Composition& comp : composeOrdering)
	{
		VideoSink* vsink = this->getVideoSink(comp.sink);
		vsink->opacity = comp.opacity;
		vsink->zorder = comp.zOrder;

		LOG_DEBUG("Setting opacity %d, zorder %d for sink %s", vsink->opacity, vsink->zorder, vsink->name.c_str());
	}

	if (!this->applyCompositing())
	{
		//TODO: Roll back the vsink zorders values
		return API_ERROR_HAL_ERROR;
	}

	this->sendSinkUpdateToSubscribers();
	return true;
}

pbnjson::JValue VideoService::getStatus(LSHelpers::JsonRequest& request)
{
	bool subscribe;

	request.get("subscribe", subscribe).optional(true).defaultValue(false);
	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}

	JValue response = this->buildStatus();
	response.put("subscribed", subscribe);
	response.put("returnValue", true);

	if (subscribe)
	{
		this->mSinkStatusSubscription.post(response);
		this->mSinkStatusSubscription.addSubscription(request);
	}

	return response;
}

void VideoService::sendSinkUpdateToSubscribers()
{
	if (!this->mSinkStatusSubscription.hasSubscribers())
	{
		return;
	}

	JValue response = this->buildStatus();
	response.put("subscribed", true);
	this->mSinkStatusSubscription.post(response);
}

pbnjson::JValue VideoService::buildStatus()
{
	JArray videoStatus;
	for (VideoSink& sink : mSinks)
	{
		videoStatus.append(buildVideoSinkStatus(sink));
	}

	return JObject{{"video", videoStatus}};
}

pbnjson::JValue VideoService::buildVideoSinkStatus(VideoSink& vsink)
{
	return JObject{{"sink", vsink.name},
	               {"connectedSource", vsink.connected ? vsink.sourceName : JValue()}, // Set to null when not connected
	               {"connectedSourcePort", vsink.connected ? vsink.sourcePort : 0},
	               {"muted", vsink.muted},
	               {"frameRate", vsink.frameRate},
	               {"contentType", vsink.contentType},	//TODO: remove?; adaptive field missing
	               {"scanType", vsink.scanType == ScanType::INTERLACED ? "interlaced" : "progressive"},
	               {"width", vsink.frameRect.w},
	               {"height", vsink.frameRect.h},
	               {"fullScreen", vsink.fullScreen},
	               {"displayOutput", vsink.outputRect.toJValue()},
	               {"sourceInput", vsink.realInputRect.toJValue()},
	               {"opacity", vsink.opacity},
	               {"zOrder", vsink.zorder}};
}

void VideoService::readVideoCapabilities(VideoSink& context)
{
	AVAL_VIDEO_SIZE_T minDownSize,maxUpScale;

	std::vector<AVAL_PLANE_T> supportedPlanes = aval->video->getVideoPlanes();
	if (supportedPlanes.size() > context.videoSinkId)
	{
		context.minDownscaleSize = supportedPlanes[context.videoSinkId].minSizeT;
		context.maxUpscaleSize = supportedPlanes[context.videoSinkId].maxSizeT;
	}
	else
	{
		LOG_ERROR(MSGID_SINK_SETUP_ERROR,0,"Invalid SinkId");
	}
}

//TODO:: Move this to AspectRatioSetting (Rename AspectRatioSetting to appropriate name)
bool VideoService::applyVideoOutputRects(VideoSink& context, VideoRect& inputRect, VideoRect& outputRect, VideoRect& frameRect)
{
	LOG_DEBUG("applyVideoOutputRects called with inputRect {x:%u, y:%u, w:%u, h:%u},"
			  "outputRect {x:%u, y:%u, w:%u, h:%u}, frameRect {x:%u, y:%u, w:%u, h:%u}",
			   inputRect.x, inputRect.y, inputRect.w, inputRect.h, outputRect.x, outputRect.y, outputRect.w, outputRect.h,
			   frameRect.x, frameRect.y, frameRect.w, frameRect.h);

	//already set
	if (inputRect == context.inputRect && outputRect == context.outputRect && frameRect == context.frameRect)
	{
		LOG_DEBUG("\n av  Rectangle are same");
		return true;
	}

	context.frameRect = frameRect;
	context.outputRect = outputRect;
	context.inputRect = inputRect;

	if (!context.frameRect.isValid())
	{
		LOG_DEBUG("\n input Rectangle is invalid");
		// Wait for frame rect to be set (setVideoMediaData) before setting up outputs.
		// setVideoMediaData will set a different frame rect and continue the execution here.
		return true;
	}

	context.realInputRect = inputRect.isValid() ? inputRect : frameRect;

	if (!context.outputRect.isValid())
	{
		LOG_DEBUG("\n output Rectangle invalid");
		// Wait for output rect to be set.
		// setDisplayWidow will set a different output rect and continue the execution here.
		return true;
	}

/*	if (context.sourceName == "HDMI")
	{
		readHdmiTimingInfo(context);
	}
*/
	return aval->video->applyScaling(context.videoSinkId, context.frameRect.toAVALRect(), context.adaptive,
	                                 context.realInputRect.toAVALRect(),
	                                 outputRect.toAVALRect());
}

bool VideoService::applyCompositing()
{
	// The parameters work like this:
	// [0].wId = windowId for TOP layer
	// [0].uAlpha - TOP layer opacity
	// [1].wId = windowId for next layer
	// [1].uAlpha - Next layer opacity
	// so on

	std::vector<AVAL_WINDOW_INFO_T> zorder;
	zorder.resize(mSinks.size());

	for (VideoSink &sink : mSinks)
	{
		int zOrdering = sink.zorder;

		zorder[zOrdering].wId = sink.videoSinkId;
		zorder[zOrdering].uAlpha = sink.opacity;
		zorder[zOrdering].inputRegion = sink.realInputRect.toAVALRect();
		zorder[zOrdering].outputRegion = sink.outputRect.toAVALRect();
	}

	LOG_DEBUG("The zorder array is ");
	for(AVAL_WINDOW_INFO_T zsink : zorder)
		LOG_DEBUG("wId %d, uAlpha %d", zsink.wId, zsink.uAlpha);

	return aval->video->setCompositionParams(zorder);
}

// TODO: move this to PQ section!!!
bool VideoService::applyVideoFilters(VideoSink& context, const std::string& sourceName)
{
	// Just a copy of what TVService is calling, with parameters taken from tvservice as well.

	int32_t sharpness_control[7];

	/* * set black level
	 *	- UINT8 *pBlVal :
	 *		[0] : uBlackLevel, 0:low,1:high
	 *		[1] : nInputInfo(see HAL_VPQ_INPUT_T)
	 *		[2] : nHDRmode, 0:off,1:hdr709,2:hdr2020,3:dolby709,4:dolby2020
	 *	- void *pstData :
	 *		see CHIP_CSC_COEFF_T
	*/
	int32_t black_levels[3] = {0,0,0};
	int32_t picture_control[] = {25,25,25,25};
	if (sourceName == "VDEC")
	{
		sharpness_control[0] = 0; //sSharpnessCtrlType, 0:normal, 1:h,v seperated
		sharpness_control[1] = 25;//sSharpnessValue, 0~50
		sharpness_control[2] = 10;//sHSharpnessValue, 0~50
		sharpness_control[3] = 10;//sVSharpnessValue, 0~50
		sharpness_control[4] = 2; //sEdgeEnhancerValue, 0,1:off,on
		sharpness_control[5] = 1; //sSuperResValue, 0~3 off,low,medium,high
		sharpness_control[6] = AVAL_VPQ_INPUT_MEDIA_MOVIE;

		black_levels[1] = AVAL_VPQ_INPUT_MEDIA_MOVIE;
	}
	else if (sourceName == "HDMI")
	{
		sharpness_control[0] = 0; //sSharpnessCtrlType, 0:normal, 1:h,v seperated
		sharpness_control[1] = 25;//sSharpnessValue, 0~50
		sharpness_control[2] = 10;//sHSharpnessValue, 0~50
		sharpness_control[3] = 10;//sVSharpnessValue, 0~50
		sharpness_control[4] = 1; //sEdgeEnhancerValue, 0,1:off,on
		sharpness_control[5] = 2; //sSuperResValue, 0~3 off,low,medium,high
		sharpness_control[6] = AVAL_VPQ_INPUT_HDMI_TV;


		//HAL_METHOD_CHECK_RETURN_FALSE(HAL_VSC_SetRGB444Mode(FALSE));
	}
	else if (sourceName == "RGB")
	{
		black_levels[1] = AVAL_VPQ_INPUT_RGB_PC;
		//HAL_METHOD_CHECK_RETURN_FALSE(HAL_VSC_SetRGB444Mode(FALSE));
	}
	else
	{
		LOG_ERROR(MSGID_UNKNOWN_SOURCE_NAME, 0, "Internal error - unknown source name for picture quality: %s", sourceName.c_str());
		return true;
	}

	aval->controls->configureVideoSettings(SHARPNESS_Control, context.videoSinkId, sharpness_control);
	aval->controls->configureVideoSettings(PQ_Control, context.videoSinkId, picture_control);
	aval->controls->configureVideoSettings(BLACK_LEVEL_Control, context.videoSinkId, black_levels);

	return true;
}

bool VideoService::setDualVideo(bool enable)
{
	if (enable == this->mDualVideoEnabled)
	{
		return true;
	}

	if (!aval->video->setDualVideo(enable))
	{
		return false;
	}

	this->mDualVideoEnabled = enable;
	return false;
}

VideoSink* VideoService::getVideoSink(const std::string& sinkName)
{
	for(VideoSink& sink : mSinks)
	{
		if(sink.name == sinkName)
			return &sink;
	}

	return nullptr;
}

pbnjson::JValue VideoService::setDisplayResolution(LSHelpers::JsonRequest &request)
{
	uint16_t w,h;

	request.get("w", w);
	request.get("h", h);

	if (!request.finishParse())
	{
		return API_ERROR_SCHEMA_VALIDATION(request.getError());
	}
	AVAL_VIDEO_SIZE_T res; res.h = h; res.w = w;
	aval->video->setDisplayResolution(res);
	return true;
}

pbnjson::JValue VideoService::getSupportedResolutions(LSHelpers::JsonRequest &request)
{

	JArray modeArray;
	auto modeList = aval->video->getSupportedResolutions();
	for (auto m : modeList)
	{

		std::stringstream s;
		s << m.w << "x" << m.h;
		std::cout << s.str() << std::endl;
		JValue mode = JObject{{"name", s.str()},
		                      {"w",    m.w},
		                      {"h",    m.h}};
		modeArray.append(mode);

	}
	return JObject { {"returnValue", true},
	                 {"modes", modeArray}};

}

/*
void VideoService::readHdmiTimingInfo(VideoSink& sink)
{
	// Read timing info to trigger connect in HAL/KAdapter.
	// We don't use the value, just trigger the read.
	HAL_VFE_HDMI_TIMING_INFO_T info;
	DTV_STATUS_T status;
	HAL_METHOD_CHECK(status, HAL_VFE_HDMI_GetPortTimingInfo(sink.sourcePort, &info));
	LOG_INFO("HDMI_info", 0, "w = %d, h = %d", info.active.w, info.active.h);
}
*/
