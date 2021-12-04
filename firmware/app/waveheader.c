#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "hal_filesystem.h"
#include "waveheader.h"

void wavhdrWrite(uint8_t *header, uint32_t sampleRate, 
        uint32_t bitsPerSample, uint32_t samples, uint32_t channels) {
    uint32_t audioDataLen = samples * (bitsPerSample / 8u) * channels;
    uint32_t fileSize = audioDataLen + 44u;
    uint32_t totalDataLen = fileSize - 8u;
    uint32_t byteRate = sampleRate * (bitsPerSample / 8u) * channels;
    header[0] = 'R';
    header[1] = 'I';
    header[2] = 'F';
    header[3] = 'F';
    header[4] = (totalDataLen & 0xff); /* file-size (equals file-size - 8) */
    header[5] = ((totalDataLen >> 8U) & 0xff);
    header[6] = ((totalDataLen >> 16U) & 0xff);
    header[7] = ((totalDataLen >> 24U) & 0xff);
    header[8] = 'W'; /* Mark it as type "WAVE" */
    header[9] = 'A';
    header[10] = 'V';
    header[11] = 'E';
    header[12] = 'f'; /* Mark the format section 'fmt ' chunk */
    header[13] = 'm';
    header[14] = 't';
    header[15] = ' ';
    header[16] = 16; /* 4 bytes: size of 'fmt ' chunk, Length of format data.  Always 16 */
    header[17] = 0;
    header[18] = 0;
    header[19] = 0;
    header[20] = 1; /* format = 1 ,Wave type PCM */
    header[21] = 0;
    header[22] = channels;
    header[23] = 0;
    header[24] = (sampleRate & 0xff);
    header[25] = ((sampleRate >> 8U) & 0xff);
    header[26] = ((sampleRate >> 16U) & 0xff);
    header[27] = ((sampleRate >> 24U) & 0xff);
    header[28] = (byteRate & 0xff);
    header[29] = ((byteRate >> 8U) & 0xff);
    header[30] = ((byteRate >> 16U) & 0xff);
    header[31] = ((byteRate >> 24U) & 0xff);
    header[32] = (2 * bitsPerSample / 8); /* block align */
    header[33] = 0;
    header[34] = bitsPerSample; /* bits per sample */
    header[35] = 0;
    header[36] = 'd'; /*"data" marker */
    header[37] = 'a';
    header[38] = 't';
    header[39] = 'a';
    header[40] = (audioDataLen & 0xff); /* data-size (equals file-size - 44).*/
    header[41] = ((audioDataLen >> 8) & 0xff);
    header[42] = ((audioDataLen >> 16) & 0xff);
    header[43] = ((audioDataLen >> 24) & 0xff);
}

uint32_t readUint32(uint8_t *ptr) {
    uint32_t val = ptr[3];
    val <<= 8;
    val |= ptr[2];
    val <<= 8;
    val |= ptr[1];
    val <<= 8;
    val |= ptr[0];
    return val;
}

bool wavhdrRead(File *fp, uint32_t *sampleRate,
        uint32_t *bitsPerSample, uint32_t *dataLen, uint32_t *channels) {

    // Read RIFF chunk descriptor
    uint8_t header[40];
    if (hal_fs_read(fp, header, 12) != 12)
        return false;

    if (strncmp(header, "RIFF", 4)) return false;
    if (strncmp(header + 8, "WAVE", 4)) return false;

    // Read fmt sub chunk
    if (hal_fs_read(fp, header, 24) != 24)
        return false;

    int subChunkSize = readUint32(header + 4);
    if (strncmp(header, "fmt ", 4)) return false;

    *bitsPerSample = (uint32_t)header[22];
    *channels = (uint32_t)header[10];
    *sampleRate = readUint32(header + 12);
    if (subChunkSize > 16) {
        hal_fs_seek(fp, hal_fs_tell(fp) + subChunkSize - 16);
    }

    uint32_t audioDataLen = 0;
    // There could be other blocks before the data block
    // Find data sub chunk
    while (hal_fs_read(fp, header, 8) == 8) {
        subChunkSize = readUint32(header + 4);
        if (strncmp(header, "data", 4) == 0) {
            // found
            audioDataLen = subChunkSize;
            break;
        }
        else {
            // not found
            hal_fs_seek(fp, hal_fs_tell(fp) + subChunkSize);
        }
    }

    *dataLen = audioDataLen;

    return true;
}
