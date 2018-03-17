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
