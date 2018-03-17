// @@@LICENSE
//
//      Copyright (c) 2016-2017 LG Electronics, Inc.
//
// Confidential computer software. Valid license from LG required for
// possession, use or copying. Consistent with FAR 12.211 and 12.212,
// Commercial Computer Software, Computer Software Documentation, and
// Technical Data for Commercial Items are licensed to the U.S. Government
// under vendor's standard commercial license.
//
// LICENSE@@@

#pragma once

#include "ivolumecontroller.h"
#include  <aval_api.h>

class TvSpeakerController : public IVolumeController
{
private:
	AVAL* aval = AVAL::getInstance();

public:
	TvSpeakerController();
	~TvSpeakerController();

	TvSpeakerController(const TvSpeakerController &) = delete;
	TvSpeakerController &operator=(const TvSpeakerController &) = delete;

protected:
	bool onVolumeChanged() override;
	bool onMuteChanged() override;
};
