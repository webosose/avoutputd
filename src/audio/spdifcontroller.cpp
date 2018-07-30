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
