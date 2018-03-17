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

#include <cassert>
#include "ivolumecontroller.h"

bool IVolumeController::setVolume(SpeakerVolume newVolume)
{
	if (newVolume > MAX_VOLUME || newVolume < MIN_VOLUME)
	{
		return false;
	}

	SpeakerVolume oldVolume = mVolume;
	mVolume = newVolume;
	bool success = onVolumeChanged();

	if (!success)
	{
		mVolume = oldVolume;
	}
	return success;
};

bool IVolumeController::setMute(bool muteFlag)
{
	if (muteFlag == mMuted)
	{
		return true;
	}

	bool oldMute = muteFlag;
	mMuted = muteFlag;
	bool success = onMuteChanged();

	if (!success)
	{
		mMuted = oldMute;
	}
	return success;
};
