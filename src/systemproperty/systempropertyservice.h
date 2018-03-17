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

#pragma once

#include <string>
#include <unordered_map>

#include <ls2-helpers/ls2-helpers.hpp>

class SystemPropertyService
{
public:
	SystemPropertyService(LS::Handle& handle);
	~SystemPropertyService();
	SystemPropertyService(const SystemPropertyService &) = delete;
	SystemPropertyService& operator=(const SystemPropertyService& ) = delete;

private:
// Luna handlers
	pbnjson::JValue getProperties(LSHelpers::JsonRequest& request);

	LSHelpers::ServicePoint mService;

};
