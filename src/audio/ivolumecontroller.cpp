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
