/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/
/*
 * How to set up clock using clock driver functions:
 *
 * 1. Setup clock sources.
 *
 * 2. Set up wait states of the flash.
 *
 * 3. Set up all dividers.
 *
 * 4. Set up all selectors to provide selected clocks.
 */

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Clocks v7.0
processor: LPC5528
package_id: LPC5528JBD64
mcu_data: ksdk2_0
processor_version: 7.0.1
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

#include "fsl_power.h"
#include "fsl_clock.h"
#include "clock_config.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* System clock frequency. */
extern uint32_t SystemCoreClock;

/*******************************************************************************
 ************************ BOARD_InitBootClocks function ************************
 ******************************************************************************/
void BOARD_InitBootClocks(void)
{
    BOARD_BootClockRUN();
}

/*******************************************************************************
 ********************** Configuration BOARD_BootClockRUN ***********************
 ******************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!Configuration
name: BOARD_BootClockRUN
called_from_default_init: true
outputs:
- {id: FXCOM1_clock.outFreq, value: 12 MHz}
- {id: FXCOM3_clock.outFreq, value: 12 MHz}
- {id: FXCOM7_clock.outFreq, value: 22.5792 MHz}
- {id: System_clock.outFreq, value: 12 MHz}
settings:
- {id: PLL0_Mode, value: Normal}
- {id: ANALOG_CONTROL_FRO192M_CTRL_ENDI_FRO_12M_CFG, value: Disable}
- {id: ENABLE_CLKIN_ENA, value: Enabled}
- {id: ENABLE_SYSTEM_CLK_OUT, value: Enabled}
- {id: SYSCON.FCCLKSEL1.sel, value: SYSCON.MAINCLKSELB}
- {id: SYSCON.FCCLKSEL3.sel, value: SYSCON.MAINCLKSELB}
- {id: SYSCON.FCCLKSEL7.sel, value: SYSCON.mclk_in}
- {id: SYSCON.FRGCTRL1_DIV.scale, value: '256', locked: true}
- {id: SYSCON.MAINCLKSELA.sel, value: SYSCON.CLK_IN_EN}
- {id: SYSCON.MAINCLKSELB.sel, value: SYSCON.PLL0_BYPASS}
- {id: SYSCON.PLL0CLKSEL.sel, value: SYSCON.CLK_IN_EN}
- {id: SYSCON.PLL0M_MULT.scale, value: '16'}
- {id: SYSCON.PLL0_PDEC.scale, value: '32'}
sources:
- {id: SYSCON.XTAL32M.outFreq, value: 24 MHz, enabled: true}
- {id: SYSCON.mclk_in.outFreq, value: 22.5792 MHz, enabled: true}
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/*******************************************************************************
 * Variables for BOARD_BootClockRUN configuration
 ******************************************************************************/
/*******************************************************************************
 * Code for BOARD_BootClockRUN configuration
 ******************************************************************************/
void BOARD_BootClockRUN(void)
{
#ifndef SDK_SECONDARY_CORE
    /*!< Set up the clock sources */
    /*!< Configure FRO192M */
    POWER_DisablePD(kPDRUNCFG_PD_FRO192M);               /*!< Ensure FRO is on  */
    CLOCK_SetupFROClocking(12000000U);                   /*!< Set up FRO to the 12 MHz, just for sure */
    CLOCK_AttachClk(kFRO12M_to_MAIN_CLK);                /*!< Switch to FRO 12MHz first to ensure we can change the clock setting */

    /*!< Configure XTAL32M */
    POWER_DisablePD(kPDRUNCFG_PD_XTAL32M);                        /* Ensure XTAL32M is powered */
    POWER_DisablePD(kPDRUNCFG_PD_LDOXO32M);                       /* Ensure XTAL32M is powered */
    CLOCK_SetupExtClocking(24000000U);                            /* Enable clk_in clock */
    SYSCON->CLOCK_CTRL |= SYSCON_CLOCK_CTRL_CLKIN_ENA_MASK;       /* Enable clk_in from XTAL32M clock  */
    ANACTRL->XO32M_CTRL |= ANACTRL_XO32M_CTRL_ENABLE_SYSTEM_CLK_OUT_MASK;    /* Enable clk_in to system  */

    CLOCK_SetupI2SMClkClocking(22579200U);                            /* Enable MCLK_IN clock */

    POWER_SetVoltageForFreq(22579200U);                  /*!< Set voltage for the one of the fastest clock outputs: FXCOM7 clock output */
    CLOCK_SetFLASHAccessCyclesForFreq(12000000U);          /*!< Set FLASH wait states for core */

    /*!< Set up PLL */
    CLOCK_AttachClk(kEXT_CLK_to_PLL0);                    /*!< Switch PLL0CLKSEL to EXT_CLK */
    POWER_DisablePD(kPDRUNCFG_PD_PLL0);                  /* Ensure PLL is on  */
    POWER_DisablePD(kPDRUNCFG_PD_PLL0_SSCG);
    const pll_setup_t pll0Setup = {
        .pllctrl = SYSCON_PLL0CTRL_CLKEN_MASK | SYSCON_PLL0CTRL_SELI(11U) | SYSCON_PLL0CTRL_SELP(5U),
        .pllndec = SYSCON_PLL0NDEC_NDIV(1U),
        .pllpdec = SYSCON_PLL0PDEC_PDIV(16U),
        .pllsscg = {0x0U,(SYSCON_PLL0SSCG1_MDIV_EXT(16U) | SYSCON_PLL0SSCG1_SEL_EXT_MASK)},
        .pllRate = 12000000U,
        .flags =  PLL_SETUPFLAG_WAITLOCK
    };
    CLOCK_SetPLL0Freq(&pll0Setup);                       /*!< Configure PLL0 to the desired values */

    /*!< Set up dividers */
    CLOCK_SetClkDiv(kCLOCK_DivFlexFrg1, 0U, true);               /*!< Reset FRGCTRL1_DIV divider counter and halt it */
    CLOCK_SetClkDiv(kCLOCK_DivFlexFrg1, 256U, false);         /*!< Set FRGCTRL1_DIV divider to value 256 */
    CLOCK_SetClkDiv(kCLOCK_DivFlexFrg3, 0U, true);               /*!< Reset FRGCTRL3_DIV divider counter and halt it */
    CLOCK_SetClkDiv(kCLOCK_DivFlexFrg3, 256U, false);         /*!< Set FRGCTRL3_DIV divider to value 256 */
    CLOCK_SetClkDiv(kCLOCK_DivFlexFrg7, 0U, true);               /*!< Reset FRGCTRL7_DIV divider counter and halt it */
    CLOCK_SetClkDiv(kCLOCK_DivFlexFrg7, 256U, false);         /*!< Set FRGCTRL7_DIV divider to value 256 */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U, false);         /*!< Set AHBCLKDIV divider to value 1 */

    /*!< Set up clock selectors - Attach clocks to the peripheries */
    CLOCK_AttachClk(kPLL0_to_MAIN_CLK);                 /*!< Switch MAIN_CLK to PLL0 */
    CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM1);                 /*!< Switch FLEXCOMM1 to MAIN_CLK */
    CLOCK_AttachClk(kMAIN_CLK_to_FLEXCOMM3);                 /*!< Switch FLEXCOMM3 to MAIN_CLK */
    CLOCK_AttachClk(kMCLK_to_FLEXCOMM7);                 /*!< Switch FLEXCOMM7 to MCLK */
    SYSCON->MAINCLKSELA = ((SYSCON->MAINCLKSELA & ~SYSCON_MAINCLKSELA_SEL_MASK) | SYSCON_MAINCLKSELA_SEL(1U));    /*!< Switch MAINCLKSELA to EXT_CLK even it is not used for MAINCLKSELB */

    ANACTRL->FRO192M_CTRL &= ~ANACTRL_FRO192M_CTRL_ENA_12MHZCLK_MASK;    /* Disable FRO 12 MHz output */

    /*< Set SystemCoreClock variable. */
    SystemCoreClock = BOARD_BOOTCLOCKRUN_CORE_CLOCK;
#endif
}
