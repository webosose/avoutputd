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
#include "spdifcontroller.h"

SpdifController::SpdifController()
		: mMode(AVAL_AUDIO_SPDIF_NONE)
{
	aval->audio->connectOutput(AVAL_AUDIO_SPDIF, AVAL_AUDIO_RESOURCE_OUT_SPDIF, AVAL_AUDIO_RESOURCE_MIXER5);
}

SpdifController::~SpdifController()
{
	aval->audio->disconnectOutput(AVAL_AUDIO_SPDIF, AVAL_AUDIO_RESOURCE_OUT_SPDIF, AVAL_AUDIO_RESOURCE_MIXER5);
}

bool SpdifController::setSpdifMode(AVAL_AUDIO_SPDIF_MODE_T mode)
{
	mMode = mode;

	if(aval->audio->setOutputMode(AVAL_AUDIO_SPDIF, mode) != AVAL_ERROR_NONE)
		return false;

	return true;
}

bool SpdifController::onVolumeChanged()
{
	if (aval->audio->setOutputVolume(AVAL_AUDIO_SPDIF, IVolumeController::getVolume()) != AVAL_ERROR_NONE)
		return false;    //TODO: log error

	return true;
}

bool SpdifController::onMuteChanged()
{
	LOG_DEBUG("Spdif mute changed to %d", getMute());

	if(aval->audio->setOutputMute(AVAL_AUDIO_SPDIF, IVolumeController::getMute()) != AVAL_ERROR_NONE)
		return false;    //TODO: log error

	return true;
}
