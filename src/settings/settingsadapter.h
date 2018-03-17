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

#include <functional>
#include <ls2-helpers/ls2-helpers.hpp>

#include "logging.h"
#include "errors.h"


using namespace pbnjson;
using namespace LSHelpers;
class VideoService;
class AudioService;
class PictureSettings;

template<typename T>
class SettingsAdapter
{
public:
	SettingsAdapter (LS::Handle& handle, T* object, void (T::* callbackHandler) (JValue &settingsJval))
			:mLunaClient(&handle)
			,mSettingsSubscription()
			,mCallToken(0)
	{
		mCallback = std::bind(callbackHandler, object, std::placeholders::_1);
	};

	~SettingsAdapter() {};
	SettingsAdapter(const SettingsAdapter &) = delete;
	SettingsAdapter &operator=(const SettingsAdapter &) = delete;
	typedef std::function<void(JValue& settingsJValue)> SettingsResponseHandler;

	void subscribeToSettings(pbnjson::JValue jobject)
	{
		mSettingsSubscription.subscribe (
				mLunaClient,
				"luna://com.webos.service.settings/getSystemSettings",
				jobject,
				this,
				&SettingsAdapter::processResponse);
	};

	// Cancels previous call if still in progress.
	void makeOneCall(pbnjson::JValue jobject)
	{
		if (mCallToken)
		{
			mLunaClient.cancelCall(mCallToken);
		}

		mCallToken = mLunaClient.callOneReply(
				"luna://com.webos.service.settings/getSystemSettings",
				jobject,
				[this] (JsonResponse &response)
				{
					mCallToken = 0;
					processResponse(response);
				});
	};

	void processResponse(JsonResponse &response)
	{
		LOG_DEBUG("Got settings response");
		bool returnValue;
		std::string category;
		pbnjson::JValue settings;

		pbnjson::JValue errorKey;
		bool subscribed;
		std::string method;
		std::string app_id;
		std::string caller;

		response.get("returnValue", returnValue);
		response.get("category", category);

		response.get("method", method);
		response.get("settings", settings);
		response.get("subscribed", subscribed).optional();
		response.get("app_id", app_id).optional();
		response.get("caller", caller).optional();
		response.get("errorKey", errorKey).optional();
		response.finishParse();

		if (returnValue == false)
		{
			LOG_WARNING(MSGID_GET_SYSTEM_SETTINGS_ERROR, 0, "Could not register requested settings");
			return;
		}
		if (category == "sound")
		{
			mCallback(settings);
		}
		if (category == "picture" || category == "aspectRatio")
		{
			settings = response.getJson();
			mCallback(settings);
		}

	};
private:
	LSHelpers::ServicePoint mLunaClient;
	PersistentSubscription mSettingsSubscription;
	SettingsResponseHandler mCallback;
	LSMessageToken mCallToken;
};
