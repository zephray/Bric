//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
// This file is partially derived from waveshare e-paper EPD driver
#include "fsl_common.h"
#include "fsl_spi.h"
#include "fsl_gpio.h"
#include "pin_mux.h"
#include "epd.h"

#define EPD_SPI SPI3

const unsigned char EPD_LUTFullUpdate[]= {
    0x80,0x60,0x40,0x00,0x00,0x00,0x00,             //LUT0: BB:     VS 0 ~7
    0x10,0x60,0x20,0x00,0x00,0x00,0x00,             //LUT1: BW:     VS 0 ~7
    0x80,0x60,0x40,0x00,0x00,0x00,0x00,             //LUT2: WB:     VS 0 ~7
    0x10,0x60,0x20,0x00,0x00,0x00,0x00,             //LUT3: WW:     VS 0 ~7
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT4: VCOM:   VS 0 ~7

    0x03,0x03,0x00,0x00,0x02,                       // TP0 A~D RP0
    0x09,0x09,0x00,0x00,0x02,                       // TP1 A~D RP1
    0x03,0x03,0x00,0x00,0x02,                       // TP2 A~D RP2
    0x00,0x00,0x00,0x00,0x00,                       // TP3 A~D RP3
    0x00,0x00,0x00,0x00,0x00,                       // TP4 A~D RP4
    0x00,0x00,0x00,0x00,0x00,                       // TP5 A~D RP5
    0x00,0x00,0x00,0x00,0x00,                       // TP6 A~D RP6

    0x15,0x41,0xA8,0x32,0x30,0x0A,
};

const unsigned char EPD_LUTPartialUpdate[]= { //20 bytes
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT0: BB:     VS 0 ~7
    0x80,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT1: BW:     VS 0 ~7
    0x40,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT2: WB:     VS 0 ~7
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT3: WW:     VS 0 ~7
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,             //LUT4: VCOM:   VS 0 ~7

    0x0A,0x00,0x00,0x00,0x00,                       // TP0 A~D RP0
    0x00,0x00,0x00,0x00,0x00,                       // TP1 A~D RP1
    0x00,0x00,0x00,0x00,0x00,                       // TP2 A~D RP2
    0x00,0x00,0x00,0x00,0x00,                       // TP3 A~D RP3
    0x00,0x00,0x00,0x00,0x00,                       // TP4 A~D RP4
    0x00,0x00,0x00,0x00,0x00,                       // TP5 A~D RP5
    0x00,0x00,0x00,0x00,0x00,                       // TP6 A~D RP6

    0x15,0x41,0xA8,0x32,0x30,0x0A,
};

static void EPD_DC_H() {
	GPIO->SET[BOARD_INITPINS_EPD_DC_PORT] = 1u << BOARD_INITPINS_EPD_DC_PIN;
}

static void EPD_DC_L() {
	GPIO->CLR[BOARD_INITPINS_EPD_DC_PORT] = 1u << BOARD_INITPINS_EPD_DC_PIN;
}

static uint32_t EPD_BUSY() {
	return (uint32_t)GPIO->B[BOARD_INITPINS_EPD_BUSY_PORT][BOARD_INITPINS_EPD_BUSY_PIN];
}

static void EPD_Write(uint8_t val) {
	spi_transfer_t xfer;
	uint8_t rx_buff; // 1 byte

	xfer.txData = &val;
	xfer.rxData = &rx_buff;
	xfer.dataSize = 1;
	xfer.configFlags = kSPI_FrameAssert;
	SPI_MasterTransferBlocking(EPD_SPI, &xfer);
}

/*static void EPD_BulkWrite(uint8_t *buf, size_t len) {
	spi_transfer_t xfer;
	uint8_t rx_buff; // 1 byte

	xfer.txData = buf;
	xfer.rxData = &rx_buff;
	xfer.dataSize = 1;
	xfer.configFlags = kSPI_FrameAssert;
	SPI_MasterTransferBlocking(EPD_SPI, &xfer);
}*/

void EPD_WriteCmd(uint8_t cmd) {
	EPD_DC_L();
	EPD_Write(cmd);
}

void EPD_WriteDat(uint8_t dat) {
	EPD_DC_H();
	EPD_Write(dat);
}

void EPD_Wait() {
	while (1) {
		if (EPD_BUSY() == 0) {
			break;
		}
		//yield();
	}
}

void EPD_Init() {
	EPD_Wait();
	EPD_WriteCmd(0x12); // Soft reset
	EPD_Wait();

	EPD_WriteCmd(0x74); // Analog block control
	EPD_WriteDat(0x54);
	EPD_WriteCmd(0x7e); // Digital block control
	EPD_WriteDat(0x3b);

	EPD_WriteCmd(0x01); // Drive output control
	EPD_WriteDat(0xf9);
	EPD_WriteDat(0x00);
	EPD_WriteDat(0x00);

	EPD_WriteCmd(0x11); // Data entry mode
	EPD_WriteDat(0x01);

	EPD_WriteCmd(0x44); // Set RAM X address start/end position
	EPD_WriteDat(0x00);
	EPD_WriteDat(0x0f);

	EPD_WriteCmd(0x45); // Set RAM Y address start/end position
	EPD_WriteDat(0xf9);
	EPD_WriteDat(0x00);
	EPD_WriteDat(0x00);
	EPD_WriteDat(0x00);

	EPD_WriteCmd(0x3c); // Border waveform
	EPD_WriteDat(0x03);

	EPD_WriteCmd(0x2c); // VCOM voltage
	EPD_WriteDat(0x55);

	EPD_WriteCmd(0x03);
	EPD_WriteDat(EPD_LUTFullUpdate[70]);

	EPD_WriteCmd(0x04);
	EPD_WriteDat(EPD_LUTFullUpdate[71]);
	EPD_WriteDat(EPD_LUTFullUpdate[72]);
	EPD_WriteDat(EPD_LUTFullUpdate[73]);

	EPD_WriteCmd(0x3a); // Dummy line
	EPD_WriteDat(EPD_LUTFullUpdate[74]);
	EPD_WriteCmd(0x3b); // Gate time
	EPD_WriteDat(EPD_LUTFullUpdate[75]);

	EPD_WriteCmd(0x32);
	for (int i = 0; i < 70; i++) {
		EPD_WriteDat(EPD_LUTFullUpdate[i]);
	}

	EPD_WriteCmd(0x4e); // Set RAM X address
	EPD_WriteDat(0x00);

	EPD_WriteCmd(0x4f);
	EPD_WriteDat(0xf9);
	EPD_WriteDat(0x00);

	EPD_Wait();
}

void EPD_Clear() {
	EPD_WriteCmd(0x24);
	for (int i = 0; i < (16 * 250); i++) {
		EPD_WriteDat(0xff);
	}
	EPD_WriteCmd(0x22);
	EPD_WriteDat(0xc7);
	EPD_WriteCmd(0x20);
	EPD_Wait();
}
