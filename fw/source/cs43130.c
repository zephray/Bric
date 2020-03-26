//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include "fsl_common.h"
#include "i2c.h"
#include "systick.h"
#include "pin_mux.h"

// 0110000 if ADR is grounded
#define CS43130_ADDR       0x30
#define CS43130_WRITE_ADDR 0x60
#define CS43130_READ_ADDR  0x61

uint8_t CS43130_ReadReg(uint32_t reg) {
    uint8_t tx_payload[4];
    uint8_t rx_val;

    // Register address
    tx_payload[0] = (reg >> 16) & 0xff;
    tx_payload[1] = (reg >> 8) & 0xff;
    tx_payload[2] = (reg) & 0xff;
    // Control byte
    tx_payload[3] = 0x00;

    I2C_ReadPayload(CS43130_ADDR, tx_payload, 4, &rx_val, 1);

    return rx_val;
}

void CS43130_WriteReg(uint32_t reg, uint8_t val) {
    uint8_t tx_payload[5];

    // Register address
    tx_payload[0] = (reg >> 16) & 0xff;
    tx_payload[1] = (reg >> 8) & 0xff;
    tx_payload[2] = (reg) & 0xff;
    // Control byte
    tx_payload[3] = 0x00;
    // Register value
    tx_payload[4] = val;

    I2C_WritePayload(CS43130_ADDR, tx_payload, 5);
}

/* PCM power up sequence based on Datasheet */
void CS43130_PCMPowerUp() {
    // Pop-free power-up settings
    CS43130_WriteReg(0x10010, 0x99);
    CS43130_WriteReg(0x80032, 0x20);
    // Power up ASP
    CS43130_WriteReg(0x20000, CS43130_ReadReg(0x20000) & 0xBF);
    // Power up HP
    CS43130_WriteReg(0x20000, CS43130_ReadReg(0x20000) & 0xEF);
    // Wait 12 ms
    SysTick_DelayTicks(12);
    // Restore default settings
    CS43130_WriteReg(0x80032, 0x00);
    CS43130_WriteReg(0x10010, 0x00);
}

void CS43130_Reset() {
	GPIO->CLR[BOARD_INITPINS_CODEC_RST_PORT] = 1u << BOARD_INITPINS_CODEC_RST_PIN;
	SysTick_DelayTicks(10);
	GPIO->SET[BOARD_INITPINS_CODEC_RST_PORT] = 1u << BOARD_INITPINS_CODEC_RST_PIN;
	SysTick_DelayTicks(10);
}

/* Initialization sequence based on Datasheet */
void CS43130_Init() {
    // Configure XTAL driver
    CS43130_WriteReg(0x20052, 0x04);
    // Clear pending interrupts
    CS43130_ReadReg(0xf0000);
    // Enable XTAL interrupts
    CS43130_WriteReg(0xf0010, 0xe7);
    // Start XTAL
    CS43130_WriteReg(0x20000, 0xf6);

    // Set ASP sample rate to 44.1kHz
    CS43130_WriteReg(0x1000b, 0x01);
    // Set ASP sample bit size
    CS43130_WriteReg(0x1000c, 0x06); // 0x05: 24bits 0x06: 16bits
    // Set ASP numerator
    CS43130_WriteReg(0x40010, 0x01);
    CS43130_WriteReg(0x40011, 0x00);
    // Set ASP denominator
    CS43130_WriteReg(0x40012, 0x08);
    CS43130_WriteReg(0x40013, 0x00);
    // Set ASP LRCK high time
    CS43130_WriteReg(0x40014, 0x1f);
    CS43130_WriteReg(0x40015, 0x00);
    // Set ASP LRCK period
    CS43130_WriteReg(0x40016, 0x3f);
    CS43130_WriteReg(0x40017, 0x00);
    // Set ASP to master, configure clock polarity
    //CS43130_WriteReg(0x40018, 0x1c);
    // Set ASP to slave, configure clock polarity
    CS43130_WriteReg(0x40018, 0x0c);
    // Configure ASP frame
    CS43130_WriteReg(0x40019, 0x0a);
    // Set ASP channel location
    CS43130_WriteReg(0x50000, 0x00);
    CS43130_WriteReg(0x50001, 0x00);
    // Set ASP channel size and enable
    CS43130_WriteReg(0x5000a, 0x07);
    CS43130_WriteReg(0x5000b, 0x0f);

    // Configure PCM filter
    CS43130_WriteReg(0x90000, 0x02);
    // Set volume for channel B
    CS43130_WriteReg(0x90001, 0x00);
    // Set volume for channel A
    CS43130_WriteReg(0x90002, 0x00);
    // Configure PCM path signal control
    CS43130_WriteReg(0x90003, 0xec);
    CS43130_WriteReg(0x90004, 0x00);

    // Configure class H amplifier
    CS43130_WriteReg(0xb0000, 0x1e);
    // Set HP output to full scale
    CS43130_WriteReg(0x80000, 0x30);
    // Disable headphone detect
    CS43130_WriteReg(0xd0000, 0x04);

    // Enable ASP interrupts
    CS43130_WriteReg(0xf0011, 0x07);

    // Wait for XTAL to be ready
    while (!(CS43130_ReadReg(0xf0000) & 0x10));

    // Switch MCLK source to XTAL
    CS43130_WriteReg(0x10006, 0x04);

    // Wait at least 150us
    SysTick_DelayTicks(1);

    // Enable CLKOUT
    CS43130_WriteReg(0x20000, 0xf4);

    // Enable ASP clocks
    CS43130_WriteReg(0x1000d, 0x02);

    // Power up HP
    CS43130_PCMPowerUp();
}
