//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#ifndef __HAL_AUDIO_H__
#define __HAL_AUDIO_H__

#include <stdint.h>

typedef enum {
    AF_S16LE,
    AF_S32LE
} AudioFormat;

typedef size_t (* AudioCallback) (void *userdata, uint8_t *stream, uint32_t len);

int hal_audio_init();
int hal_audio_start(int samplerate, AudioFormat format, AudioCallback cb, void *userdata);
int hal_audio_stop();
// 0 - minimum (mute), 255 - maximum
int hal_audio_set_volume(uint8_t vol);

#endif