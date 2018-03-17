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
#include <aval_api.h>

class SpdifController : public IVolumeController
{
private:
	AVAL* aval = AVAL::getInstance();

public:
	SpdifController();
	~SpdifController();

	SpdifController(const SpdifController &) = delete;
	SpdifController &operator=(const SpdifController &) = delete;

	bool setSpdifMode(AVAL_AUDIO_SPDIF_MODE_T mode);
	inline AVAL_AUDIO_SPDIF_MODE_T getSpdifMode() { return mMode;};

protected:
	bool onVolumeChanged() override;
	bool onMuteChanged() override;

	AVAL_AUDIO_SPDIF_MODE_T mMode;
};
