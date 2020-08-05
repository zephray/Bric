# Bric Firmware

The firmware is based on the FreeRTOS, designed to run on the Bric.

## Directory Structure

- ```app```: Main firmware logic
- ```mcu```: Build for the Bric hardware
- ```mcu/amazon-freertos```: FreeRTOS port for the target MCU, managed by MCUXpresso
- ```mcu/CMSIS```: ARM Cortex-M Software Interface Standard hardware abstraction, managed by MCUXpresso
- ```mcu/component```: NXP components common code, managed by MCUXpresso
- ```mcu/device```: MCU core function and register definition, managed by MCUXpresso
- ```mcu/doc```: Documents from 3rd-party library, not the document for the firmware, managed by MCUXpresso
- ```mcu/drivers```: MCU peripheral drivers, managed by MCUXpresso
- ```mcu/fatfs```: FATFS filesystem library, managed by MCUXpresso
- ```mcu/libs```: NXP binary power libraries, managed by MCUXpresso
- ```mcu/sdmmc```: NXP SD/MMC driver, managed by MCUXpresso
- ```mcu/source```: Platform-specific application code
- ```mcu/startup```: NXP MCU start-up code
- ```mcu/usb```: NXP USB protocol stack