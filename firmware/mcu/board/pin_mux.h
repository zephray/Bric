/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

/*!
 * @brief MCLK control.: input mode. */
#define MCLKIO_MCLKIO_INPUT 0x00u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_10_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO0_10_FUNC_ALT0 0x00u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_11_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 6. */
#define PIO0_11_FUNC_ALT6 0x06u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_12_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 6. */
#define PIO0_12_FUNC_ALT6 0x06u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_13_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Pull-up current source enable in I2C mode.: Enabled. Pull resistor is conencted. */
#define PIO0_13_ECS_ENABLED 0x01u
/*!
 * @brief Switch between GPIO mode and I2C mode.: I2C mode. */
#define PIO0_13_EGP_I2C_MODE 0x00u
/*!
 * @brief Selects pin function.: Alternative connection 5. */
#define PIO0_13_FUNC_ALT5 0x05u
/*!
 * @brief Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-up. Pull-up resistor enabled. */
#define PIO0_13_MODE_PULL_UP 0x02u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_14_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Pull-up current source enable in I2C mode.: Enabled. Pull resistor is conencted. */
#define PIO0_14_ECS_ENABLED 0x01u
/*!
 * @brief Switch between GPIO mode and I2C mode.: I2C mode. */
#define PIO0_14_EGP_I2C_MODE 0x00u
/*!
 * @brief Selects pin function.: Alternative connection 6. */
#define PIO0_14_FUNC_ALT6 0x06u
/*!
 * @brief Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-up. Pull-up resistor enabled. */
#define PIO0_14_MODE_PULL_UP 0x02u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_17_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 2. */
#define PIO0_17_FUNC_ALT2 0x02u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_18_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO0_18_FUNC_ALT0 0x00u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_19_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 7. */
#define PIO0_19_FUNC_ALT7 0x07u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_1_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO0_1_FUNC_ALT0 0x00u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_20_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 7. */
#define PIO0_20_FUNC_ALT7 0x07u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_21_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 7. */
#define PIO0_21_FUNC_ALT7 0x07u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_22_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO0_22_FUNC_ALT0 0x00u
/*!
 * @brief Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-up. Pull-up resistor enabled. */
#define PIO0_22_MODE_PULL_UP 0x02u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_23_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 1. */
#define PIO0_23_FUNC_ALT1 0x01u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_24_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 2. */
#define PIO0_24_FUNC_ALT2 0x02u
/*!
 * @brief
 * Driver slew rate.
 * : Fast-mode, output slew rate is faster.
 * Refer to the appropriate specific device data sheet for details.
 */
#define PIO0_24_SLEW_FAST 0x01u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_25_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 2. */
#define PIO0_25_FUNC_ALT2 0x02u
/*!
 * @brief
 * Driver slew rate.
 * : Fast-mode, output slew rate is faster.
 * Refer to the appropriate specific device data sheet for details.
 */
#define PIO0_25_SLEW_FAST 0x01u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_26_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO0_26_FUNC_ALT0 0x00u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_27_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO0_27_FUNC_ALT0 0x00u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_29_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO0_29_FUNC_ALT0 0x00u
/*!
 * @brief Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-up. Pull-up resistor enabled. */
#define PIO0_29_MODE_PULL_UP 0x02u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_2_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO0_2_FUNC_ALT0 0x00u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_30_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO0_30_FUNC_ALT0 0x00u
/*!
 * @brief Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-up. Pull-up resistor enabled. */
#define PIO0_30_MODE_PULL_UP 0x02u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_31_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 2. */
#define PIO0_31_FUNC_ALT2 0x02u
/*!
 * @brief
 * Driver slew rate.
 * : Fast-mode, output slew rate is faster.
 * Refer to the appropriate specific device data sheet for details.
 */
#define PIO0_31_SLEW_FAST 0x01u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_3_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 1. */
#define PIO0_3_FUNC_ALT1 0x01u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_4_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_5_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO0_5_FUNC_ALT0 0x00u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_6_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 1. */
#define PIO0_6_FUNC_ALT1 0x01u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_7_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 2. */
#define PIO0_7_FUNC_ALT2 0x02u
/*!
 * @brief
 * Driver slew rate.
 * : Fast-mode, output slew rate is faster.
 * Refer to the appropriate specific device data sheet for details.
 */
#define PIO0_7_SLEW_FAST 0x01u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_8_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 2. */
#define PIO0_8_FUNC_ALT2 0x02u
/*!
 * @brief
 * Driver slew rate.
 * : Fast-mode, output slew rate is faster.
 * Refer to the appropriate specific device data sheet for details.
 */
#define PIO0_8_SLEW_FAST 0x01u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO0_9_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO0_9_FUNC_ALT0 0x00u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO1_0_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 2. */
#define PIO1_0_FUNC_ALT2 0x02u
/*!
 * @brief
 * Driver slew rate.
 * : Fast-mode, output slew rate is faster.
 * Refer to the appropriate specific device data sheet for details.
 */
#define PIO1_0_SLEW_FAST 0x01u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO1_1_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO1_1_FUNC_ALT0 0x00u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO1_2_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO1_2_FUNC_ALT0 0x00u
/*!
 * @brief Selects function mode (on-chip pull-up/pull-down resistor control).: Pull-up. Pull-up resistor enabled. */
#define PIO1_2_MODE_PULL_UP 0x02u
/*!
 * @brief Select Digital mode.: Enable Digital mode. Digital input is enabled. */
#define PIO1_3_DIGIMODE_DIGITAL 0x01u
/*!
 * @brief Selects pin function.: Alternative connection 0. */
#define PIO1_3_FUNC_ALT0 0x00u

/*! @name PIO0_23 (number 12), MCLK
  @{ */
#define BOARD_INITPINS_MCLK_PORT 0U /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_MCLK_PIN 23U /*!<@brief 0U pin index: 23 */
                                    /* @} */

/*! @name PIO0_1 (number 2), EPD_DC
  @{ */
#define BOARD_INITPINS_EPD_DC_GPIO GPIO /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_EPD_DC_PORT 0U   /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_EPD_DC_PIN 1U    /*!<@brief 0U pin index: 1 */
                                        /* @} */

/*! @name PIO0_10 (number 13), LED
  @{ */
#define BOARD_INITPINS_LED_GPIO GPIO /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_LED_PORT 0U   /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_LED_PIN 10U   /*!<@brief 0U pin index: 10 */
                                     /* @} */

/*! @name PIO0_27 (number 18), PMIC_ON_REQ
  @{ */
#define BOARD_INITPINS_PMIC_ON_REQ_GPIO GPIO /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_PMIC_ON_REQ_PORT 0U   /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_PMIC_ON_REQ_PIN 27U   /*!<@brief 0U pin index: 27 */
                                             /* @} */

/*! @name PIO0_9 (number 37), SD_PWR_EN
  @{ */
#define BOARD_INITPINS_SD_PWR_EN_GPIO GPIO /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_SD_PWR_EN_PORT 0U   /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_SD_PWR_EN_PIN 9U    /*!<@brief 0U pin index: 9 */
                                           /* @} */

/*! @name PIO0_18 (number 38), CODEC_RST
  @{ */
#define BOARD_INITPINS_CODEC_RST_GPIO GPIO /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_CODEC_RST_PORT 0U   /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_CODEC_RST_PIN 18U   /*!<@brief 0U pin index: 18 */
                                           /* @} */

/*! @name PIO1_1 (number 39), EPD_RST
  @{ */
#define BOARD_INITPINS_EPD_RST_GPIO GPIO /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_EPD_RST_PORT 1U   /*!<@brief PORT device name: 1U */
#define BOARD_INITPINS_EPD_RST_PIN 1U    /*!<@brief 1U pin index: 1 */
                                         /* @} */

/*! @name PIO0_26 (number 40), USB_SUSPEND
  @{ */
#define BOARD_INITPINS_USB_SUSPEND_GPIO GPIO /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_USB_SUSPEND_PORT 0U   /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_USB_SUSPEND_PIN 26U   /*!<@brief 0U pin index: 26 */
                                             /* @} */

/*! @name PIO1_2 (number 41), CHRG
  @{ */
#define BOARD_INITPINS_CHRG_GPIO GPIO /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_CHRG_PORT 1U   /*!<@brief PORT device name: 1U */
#define BOARD_INITPINS_CHRG_PIN 2U    /*!<@brief 1U pin index: 2 */
                                      /* @} */

/*! @name PIO1_3 (number 42), PB_STAT
  @{ */
#define BOARD_INITPINS_PB_STAT_GPIO GPIO /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_PB_STAT_PORT 1U   /*!<@brief PORT device name: 1U */
#define BOARD_INITPINS_PB_STAT_PIN 3U    /*!<@brief 1U pin index: 3 */
                                         /* @} */

/*! @name PIO0_22 (number 50), SW_SEL
  @{ */
#define BOARD_INITPINS_SW_SEL_GPIO GPIO /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_SW_SEL_PORT 0U   /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_SW_SEL_PIN 30U   /*!<@brief 0U pin index: 30 */
                                        /* @} */

/*! @name PIO0_5 (number 56), SW_PLUS
  @{ */
#define BOARD_INITPINS_SW_PLUS_GPIO GPIO /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_SW_PLUS_PORT 0U   /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_SW_PLUS_PIN 5U    /*!<@brief 0U pin index: 5 */
                                         /* @} */

/*! @name PIO0_29 (number 59), SW_MIN
  @{ */
#define BOARD_INITPINS_SW_MIN_GPIO GPIO /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_SW_MIN_PORT 0U   /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_SW_MIN_PIN 29U   /*!<@brief 0U pin index: 29 */
                                        /* @} */

/*! @name PIO0_30 (number 60), SW_BACK
  @{ */
#define BOARD_INITPINS_SW_BACK_GPIO GPIO /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_SW_BACK_PORT 0U   /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_SW_BACK_PIN 22U   /*!<@brief 0U pin index: 22 */
                                         /* @} */

/*! @name PIO0_2 (number 52), EPD_BUSY
  @{ */
#define BOARD_INITPINS_EPD_BUSY_GPIO GPIO /*!<@brief GPIO device name: GPIO */
#define BOARD_INITPINS_EPD_BUSY_PORT 0U   /*!<@brief PORT device name: 0U */
#define BOARD_INITPINS_EPD_BUSY_PIN 2U    /*!<@brief 0U pin index: 2 */
                                          /* @} */

/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void); /* Function assigned for the Cortex-M33 (Core #0) */

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
