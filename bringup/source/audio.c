/*
 * Copyright (C) 2019 Wenting Zhang <zephray@outlook.com>
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <math.h>
#include "fsl_dma.h"
#include "fsl_i2s.h"
#include "fsl_i2s_dma.h"

#define M_PI 3.14159265358979323846264338327950288

//#define AUDIO_I2S_MASTER_CLOCK_FREQUENCY (22579200)
#define AUDIO_I2S_MASTER_CLOCK_FREQUENCY (11289600)
#define AUDIO_I2S_TX (I2S7)
#define AUDIO_DMA (DMA0)
#define AUDIO_I2S_TX_CHANNEL (19)
#define AUDIO_I2S_TX_MODE (kI2S_MasterSlaveNormalSlave)
//#define AUDIO_I2S_TX_MODE (kI2S_MasterSlaveNormalMaster)
#define AUDIO_I2S_DATA (32) // 32 bit per sample
#define AUDIO_I2S_FRAME (64) // 64 bit per frame
#define I2S_CLOCK_DIVIDER (AUDIO_I2S_MASTER_CLOCK_FREQUENCY / 44100U / AUDIO_I2S_FRAME)

#if defined(__GNUC__) /* GNU Compiler */
#ifndef __ALIGN_END
#define __ALIGN_END __attribute__((aligned(4)))
#endif
#ifndef __ALIGN_BEGIN
#define __ALIGN_BEGIN
#endif
#else
#ifndef __ALIGN_END
#define __ALIGN_END
#endif
#ifndef __ALIGN_BEGIN
#if defined(__CC_ARM) || defined(__ARMCC_VERSION) /* ARM Compiler */
#define __ALIGN_BEGIN __attribute__((aligned(4)))
#elif defined(__ICCARM__) /* IAR Compiler */
#define __ALIGN_BEGIN
#endif
#endif
#endif

static void TxCallback(I2S_Type *base, i2s_dma_handle_t *handle,
		status_t completionStatus, void *userData);

/* 100 samples => time about 2 ms */
//__ALIGN_BEGIN static uint8_t s_Buffer[400] __ALIGN_END;
static dma_handle_t s_DmaTxHandle;
static i2s_config_t s_TxConfig;
static i2s_dma_handle_t s_TxHandle;
static i2s_transfer_t s_TxTransfer;

// Up to 100 samples
__ALIGN_BEGIN int32_t bufa[100*2] __ALIGN_END;
__ALIGN_BEGIN int32_t bufb[100*2] __ALIGN_END;

int32_t *front_buf;
float phase = 0;
int sample_count;
int freq = 20;

void AUDIO_Init() {
	/* reset FLEXCOMM for I2S */
	RESET_PeripheralReset(kFC7_RST_SHIFT_RSTn);
	/* reset NVIC for FLEXCOMM7 */
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
    s_TxConfig.masterSlave = AUDIO_I2S_TX_MODE;
    s_TxConfig.dataLength = AUDIO_I2S_DATA;
    s_TxConfig.frameLength = AUDIO_I2S_FRAME;
    I2S_TxInit(AUDIO_I2S_TX, &s_TxConfig);

    DMA_Init(AUDIO_DMA);

	DMA_EnableChannel(AUDIO_DMA, AUDIO_I2S_TX_CHANNEL);
	DMA_SetChannelPriority(AUDIO_DMA, AUDIO_I2S_TX_CHANNEL,
			kDMA_ChannelPriority3);
	DMA_CreateHandle(&s_DmaTxHandle, AUDIO_DMA, AUDIO_I2S_TX_CHANNEL);
}

static __inline__ float getphase(int freq, int t, float phase) {
	float p = 2*M_PI*(float)freq*((float)t/44100.0f) + phase;
	p = fmodf(p, 2*M_PI);
	return p;
}

static __inline__ float getsin(int freq, int t, float phase) {
	return sinf(getphase(freq, t, phase));
}

void AUDIO_Start() {
	s_TxTransfer.data = (uint8_t *)bufa;
	s_TxTransfer.dataSize = 100*2*4;

	front_buf = bufa;
	sample_count = 0;

	I2S_TxTransferCreateHandleDMA(AUDIO_I2S_TX, &s_TxHandle, &s_DmaTxHandle,
			TxCallback, NULL);
	I2S_TxTransferSendDMA(AUDIO_I2S_TX, &s_TxHandle, s_TxTransfer);

	s_TxTransfer.data = (uint8_t *)bufb;
	I2S_TxTransferSendDMA(AUDIO_I2S_TX, &s_TxHandle, s_TxTransfer);
}

static void TxCallback(I2S_Type *base, i2s_dma_handle_t *handle,
		status_t completionStatus, void *userData) {
	// Front buf has just finished transferring
    for (int i = 0; i < 100; i++) {
    	int32_t sample = (int32_t)(getsin(freq, i, phase) * (2147483647.0f * 0.891f));
    	front_buf[i * 2] = sample;
    	front_buf[i * 2 + 1] = sample;
    }
    phase = getphase(freq, 100, phase);

    s_TxTransfer.data = (uint8_t *)front_buf;
    if (front_buf == bufa)
    	front_buf = bufb;
    else
    	front_buf = bufa;

    sample_count ++;
    if (sample_count == 882) // 2s
    {
    	sample_count = 0;
    	freq += 200;
    	if (freq >= 20000)
    		freq = 10;
    }

    I2S_TxTransferSendDMA(base, handle, s_TxTransfer);
}
