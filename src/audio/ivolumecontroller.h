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

/**
 * @file IVolumeController.h
 *
 * @brief Interface for volume control on different speaker types like ext speaker,bluetooth etc
 *
 */

#include <aval_api.h>

/**
 * Abstract base class for volume control implementations.
 * Subclass this for different speaker types like ext speaker,bluetooth etc
 */
class IVolumeController
{
public:

	IVolumeController() : mVolume(0), mMuted(true) {};
	virtual ~IVolumeController() {};

	void init(bool muted, SpeakerVolume volume)
	{
		mMuted = muted;
		mVolume = volume;
		onVolumeChanged();
		onMuteChanged();
	}

	/**
	 * Returns current mute value.
    */
	inline bool getMute() const
	{
		return mMuted;
	}

	/**
	 * * Mute/unmute the output.
	 */
	bool setMute(bool muteFlag);

	/**
	 * Get current volume in range 0..100.
	 */
	inline SpeakerVolume getVolume() const
	{
		return mVolume;
	};

	/**
	 * Set new volume.
	 */
	bool setVolume(SpeakerVolume newVolume);

protected:
	/**
	 * Called when volume is changed.
	 * The mVolume field is set to the new volume before calling this.
	 * Performs the appropriate actions to set the new volume.
	 * @return true on success, false if setting new volume failed.
	 */
	virtual bool onVolumeChanged() = 0;

	/**
	 * Called when mute is changed.
	 * The mMute field is set to the new value before calling this.
	 * Performs the appropriate actions to set mute.
	 * @return true on success, false if setting new volume failed.
	 */
	virtual bool onMuteChanged() = 0;

private: // Data members
	SpeakerVolume mVolume;
	bool mMuted;
};
