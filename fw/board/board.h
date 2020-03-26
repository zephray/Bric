/*
 * Copyright 2016-2020 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    board.h
 * @brief   Board initialization header file.
 */
 
/* This is a template for board specific configuration created by MCUXpresso IDE Project Wizard.*/

#ifndef _BOARD_H_
#define _BOARD_H_

/**
 * @brief	The board name 
 */
#define BOARD_NAME "Bric"

#define BOARD_SDIF_BASEADDR SDIF
#define BOARD_SDIF_CLKSRC kCLOCK_SDio
#define BOARD_SDIF_CLK_FREQ CLOCK_GetSdioClkFreq()
#define BOARD_SDIF_CLK_ATTACH kMAIN_CLK_to_SDIO_CLK
#define BOARD_SDIF_IRQ SDIO_IRQn
#define BOARD_MMC_VCC_SUPPLY kMMC_VoltageWindows270to360
#define BOARD_SD_CARD_DETECT_PIN 17
#define BOARD_SD_CARD_DETECT_PORT 0
#define BOARD_SD_CARD_DETECT_GPIO GPIO
//#define BOARD_SD_DETECT_TYPE kSDMMCHOST_DetectCardByHostCD
#define BOARD_SD_DETECT_TYPE kSDMMCHOST_DetectCardByHostDATA3

#define SDMMC_ENABLE_LOG_PRINT

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * @brief 	Initialize board specific settings.
 */
void BOARD_InitDebugConsole(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif /* _BOARD_H_ */


