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

#include "logging.h"
#include "tvspeakercontroller.h"

TvSpeakerController::TvSpeakerController()
{
	aval->audio->connectOutput(AVAL_AUDIO_SPK, AVAL_AUDIO_RESOURCE_OUT_SPK, AVAL_AUDIO_RESOURCE_SE);
}

TvSpeakerController::~TvSpeakerController()
{
	aval->audio->disconnectOutput(AVAL_AUDIO_SPK, AVAL_AUDIO_RESOURCE_OUT_SPK, AVAL_AUDIO_RESOURCE_SE);
};

bool TvSpeakerController::onVolumeChanged()
{
	LOG_DEBUG("In %s", __func__);

	if (aval->audio->setOutputVolume(AVAL_AUDIO_SPK, IVolumeController::getVolume()) != AVAL_ERROR_NONE)
		return false;

	return true;
}

bool TvSpeakerController::onMuteChanged()
{
	LOG_DEBUG("TvSpeakerController mute changed to %d", IVolumeController::getMute());

	if(aval->audio->setOutputMute(AVAL_AUDIO_SPK, getMute()) != AVAL_ERROR_NONE)
		return false;

	return true;
}
