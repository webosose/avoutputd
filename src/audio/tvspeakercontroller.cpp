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
