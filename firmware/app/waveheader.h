#pragma once

void wavhdrWrite(uint8_t *header, uint32_t sampleRate, 
        uint32_t bitsPerSample, uint32_t samples, uint32_t channels);
bool wavhdrRead(File *fp, uint32_t *sampleRate,
        uint32_t *bitsPerSample, uint32_t *dataLen, uint32_t *channels);
