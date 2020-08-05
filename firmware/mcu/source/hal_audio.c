//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include <stdio.h>
#include "hal_audio.h"
#include "fsl_dma.h"
#include "fsl_i2s.h"
#include "fsl_i2s_dma.h"
#include "cs43130.h"

//#define AUDIO_I2S_MASTER_CLOCK_FREQUENCY (22579200)
#define AUDIO_I2S_MASTER_CLOCK_FREQUENCY (11289600)
#define AUDIO_I2S_TX (I2S7)
#define AUDIO_DMA (DMA0)
#define AUDIO_I2S_TX_CHANNEL (19)
#define ADUIO_I2S_TX_MODE (kI2S_MasterSlaveNormalSlave)
#define AUDIO_I2S_DATA (32) // 32 bit per sample
#define AUDIO_I2S_FRAME (64) // 64 bit per frame
#define I2S_CLOCK_DIVIDER (AUDIO_I2S_MASTER_CLOCK_FREQUENCY / 44100U / AUDIO_I2S_FRAME)

static dma_handle_t s_DmaTxHandle;
static i2s_config_t s_TxConfig;
static i2s_dma_handle_t s_TxHandle;
static i2s_transfer_t s_TxTransfer;

static AudioCallback user_callback;

#define DMA_BUFFER_SIZE (1152*2*2)
uint8_t dma_buffer_a[DMA_BUFFER_SIZE] __attribute__((aligned(4)));
uint8_t dma_buffer_b[DMA_BUFFER_SIZE] __attribute__((aligned(4)));
uint8_t *dma_buffers[2] = {dma_buffer_a, dma_buffer_b};
size_t dma_buffer_pointer = 0;
// Pointer + 0 -> buffer transferring
// Pointer + 1 -> buffer in the DMA queue

void hal_audio_i2s_dma_callback(I2S_Type *base, i2s_dma_handle_t *handle,
		status_t completionStatus, void *userData) {
    int size = user_callback(userData, dma_buffers[dma_buffer_pointer], 
            DMA_BUFFER_SIZE);
    if (size > 0) {
        s_TxTransfer.data = dma_buffers[dma_buffer_pointer];
        s_TxTransfer.dataSize = size;
    }
    else {
    	memset(dma_buffers[dma_buffer_pointer], 0, 1152*2*2);
    	s_TxTransfer.data = dma_buffers[dma_buffer_pointer];
    	s_TxTransfer.dataSize = 1152*2*2;
    }
    I2S_TxTransferSendDMA(AUDIO_I2S_TX, &s_TxHandle, s_TxTransfer);
}

int hal_audio_init() {
    // Initialize the DAC
    CS43130_Reset();
    CS43130_Init();

    // reset FLEXCOMM for I2S
	RESET_PeripheralReset(kFC7_RST_SHIFT_RSTn);
	// reset NVIC for FLEXCOMM7
	NVIC_ClearPendingIRQ(FLEXCOMM7_IRQn);

    /*
     * masterSlave = kI2S_MasterSlaveNormalSlave;
     * mode = kI2S_ModeI2sClassic;
     * rightLow = false;
     * leftJust = false;
     * pdmData = false;
     * sckPol = false;
     * wsPol = false;
     * divider = 1;
     * oneChannel = false;
     * dataLength = 16;
     * frameLength = 32;
     * position = 0;
     * watermark = 4;
     * txEmptyZero = true;
     * pack48 = false;
     */
    I2S_TxGetDefaultConfig(&s_TxConfig);
    s_TxConfig.divider = I2S_CLOCK_DIVIDER;
    s_TxConfig.masterSlave = ADUIO_I2S_TX_MODE;
    s_TxConfig.dataLength = AUDIO_I2S_DATA;
    s_TxConfig.frameLength = AUDIO_I2S_FRAME;
    I2S_TxInit(AUDIO_I2S_TX, &s_TxConfig);

    DMA_Init(AUDIO_DMA);

	DMA_EnableChannel(AUDIO_DMA, AUDIO_I2S_TX_CHANNEL);
	DMA_SetChannelPriority(AUDIO_DMA, AUDIO_I2S_TX_CHANNEL,
			kDMA_ChannelPriority3);
	DMA_CreateHandle(&s_DmaTxHandle, AUDIO_DMA, AUDIO_I2S_TX_CHANNEL);

	return 0;
}

int hal_audio_start(int samplerate, AudioFormat format, AudioCallback cb, void *userdata) {
	user_callback = cb;

	if (format == AF_S16LE) {
		s_TxConfig.dataLength = 16;
		I2S_TxInit(AUDIO_I2S_TX, &s_TxConfig);
		CS43130_SwtichSampleDepth(16);
	}
	else if (format == AF_S32LE) {
		s_TxConfig.dataLength = 32;
		I2S_TxInit(AUDIO_I2S_TX, &s_TxConfig);
		CS43130_SwtichSampleDepth(32);
	}

	CS43130_PCMPowerUp();

	memset(dma_buffer_a, 0, DMA_BUFFER_SIZE);
    memset(dma_buffer_b, 0, DMA_BUFFER_SIZE);
    
	s_TxTransfer.dataSize = DMA_BUFFER_SIZE;

	I2S_TxTransferCreateHandleDMA(AUDIO_I2S_TX, &s_TxHandle, &s_DmaTxHandle,
			hal_audio_i2s_dma_callback, userdata);

    s_TxTransfer.data     = dma_buffer_a;
	I2S_TxTransferSendDMA(AUDIO_I2S_TX, &s_TxHandle, s_TxTransfer);

    s_TxTransfer.data     = dma_buffer_b;
    I2S_TxTransferSendDMA(AUDIO_I2S_TX, &s_TxHandle, s_TxTransfer);

    // Buffer A will start transfer, B is in the queue.
    // When the first callback comes, the A has finished transferring and B
    // has started transferring, so the application can start to fill in A.
    return 0;
}

int hal_audio_stop() {
    I2S_TransferAbortDMA(AUDIO_I2S_TX, &s_TxHandle);
    CS43130_PCMPowerDown();
    return 0;
}

// 0 - minimum (mute), 255 - maximum
int hal_audio_set_volume(uint8_t vol) {
    printf("Volume set to %d", vol);
    CS43130_SetVolume(vol);
    return 0;
}
