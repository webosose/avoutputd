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
