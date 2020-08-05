//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include <SDL/SDL.h>
#include "hal_audio.h"

static AudioCallback user_cb;

int hal_audio_init() {
    // SDL should be initialized in SDL_Init called in video init
}

void hal_audio_sdl_callback(void *userdata, uint8_t *stream, int len) {

}

int hal_audio_start(int samplerate, AudioFormat format, AudioCallback cb, void *userdata) {
    SDL_AudioSpec as_request;
    SDL_AudioSpec as_got;

    if (format != AUDIO_S16) {
        fprintf(stderr, "Format other than S16 is not yet supported in linux framework\n");
        return -1;
    }

    as_request.freq = samplerate;
    as_request.format = AUDIO_S16;
    as_request.channels = 2;
    as_request.samples = 4096;
    as_request.callback = hal_audio_sdl_callback;
    as_request.userdata = userdata;
    if (SDL_OpenAudio(&as_request, &as_got) < 0)
    {
        fprintf(stderr, "couldn't open audio: %s\n", SDL_GetError());
        return -1;
    }
    
    user_cb = cb;

    return 0;
}

int hal_audio_stop() {
    SDL_PauseAudio(1);
    SDL_CloseAudio();
}

// 0 - minimum (mute), 255 - maximum
int hal_audio_set_volume(uint8_t vol) {
    fprintf(stdout, "Volume set to %d", vol);
}
