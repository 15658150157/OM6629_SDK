/* ----------------------------------------------------------------------------
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 * -------------------------------------------------------------------------- */

/**
 * @defgroup DOC DOC
 * @ingroup  DOCUMENT
 * @brief    CMSIS Device System Source File
 * @details  CMSIS Device System Source File
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */


/*******************************************************************************
 * INCLUDES
 */
#include <stddef.h>
#include "om_device.h"
#include "om_common.h"


/*******************************************************************************
 * TYPEDEFS
 */
/**
 * @brief Exception / Interrupt Handler Function Prototype
 */
typedef void(*VECTOR_TABLE_Type)(void);

/**
 *******************************************************************************
 * @brief Exception / Interrupt Vector table
 *******************************************************************************
 */
extern const VECTOR_TABLE_Type __VECTOR_TABLE[];    /*lint !e526*/


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static inline void register_set(volatile uint32_t *reg, uint32_t mask, uint32_t value)
{
    register uint32_t reg_prev;

    reg_prev = *reg;
    reg_prev &= ~mask;
    reg_prev |= mask & value;
    *reg = reg_prev;
}


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief System initialization function
 *******************************************************************************
 */
void SystemInit(void)
{
    /* check chip version */
    while (((OM_SYS->REV_ID & SYS_REV_CHIP_VER_MASK) >> SYS_REV_CHIP_VER_POS) != (CONFIG_HARDWARE_VERSION - 1U));

    // Retention LDO: 0=0.6v 2=0.7v 3=0.75v 4=0.8v 5=0.85v 6=0.9v 7=0.95v(default) 8=1.0v
    register_set(&OM_PMU->ANA_PD_1, PMU_ANA_PD_1_DIG_RETLDO_TRIM_MASK, (5U << PMU_ANA_PD_1_DIG_RETLDO_TRIM_POS));
    // DCDC on delay is 3*32k
    register_set(&OM_PMU->BASIC, PMU_BASIC_DCDC_ON_DELAY_MASK, (3U << PMU_BASIC_DCDC_ON_DELAY_POS));
    // DCDC频率振荡调节寄存器,CTUNE值越大振荡频率越慢
    register_set(&OM_PMU->ANA_PD, PMU_ANA_PD_DCDC_CTUNE_MASK, (3U << PMU_ANA_PD_DCDC_CTUNE_POS));
    // DCDC PSM/PWM 电流切换调节寄存器,电流大的时候DCDC会从PSM模式切到PWM模式
    register_set(&OM_PMU->ANA_PD_1, PMU_ANA_PD_1_DCDC_PSM_VREF_MASK, (0U << PMU_ANA_PD_1_DCDC_PSM_VREF_POS));

    // TODO: CPFT
    // TODO: FLASH settings
    #if 0
    // power on all sram and icache ram(power on in ROM)
    OM_PMU->PSO_PM |= PMU_PSO_RAM1_2_POWER_ON_MASK | PMU_PSO_RAM3_POWER_ON_MASK
                   | PMU_PSO_RAM4_POWER_ON_MASK | PMU_PSO_RAM5_POWER_ON_MASK
                   | PMU_PSO_ICACHE_POWER_ON_MASK;
    uint32_t power_status = PMU_PSO_RAM1_2_POWER_STATUS_MASK | PMU_PSO_RAM3_POWER_STATUS_MASK
                          | PMU_PSO_RAM4_POWER_STATUS_MASK | PMU_PSO_RAM5_POWER_STATUS_MASK
                          | PMU_PSO_ICACHE_POWER_STATUS_MASK;
    while ((OM_PMU->PSO_PM & power_status) != power_status);
    #endif

    /* icache enable */
    OM_ICACHE->CONFIG = 0;
    OM_ICACHE->CTRL = ICACHE_CTRL_CEN_MASK;
    while((OM_ICACHE->STATUS & ICACHE_STATUS_CSTS_MASK) == 0);

    /**
     * Disable clock & Reset: UART1/EFUSE/GPDMA using in ROM;
     * LCD and LPTIM is enabled as default
     * FLASH1 disabled in pm_init
     * RTC clock disabled
     **/
    OM_CPM->UART1_CFG |= CPM_UART_CFG_GATE_EN_MASK | CPM_UART_CFG_SOFT_RESET_MASK;
    OM_CPM->EFUSE_CFG |= CPM_EFUSE_CFG_GATE_EN_MASK | CPM_EFUSE_CFG_SOFT_RESET_MASK;
    OM_CPM->GPDMA_CFG |= CPM_GPDMA_CFG_GATE_EN_MASK | CPM_GPDMA_CFG_SOFT_RESET_MASK;
    OM_CPM->LCD_CFG |= CPM_LCD_CFG_GATE_EN_MASK;
    OM_PMU->BASIC |= PMU_BASIC_LPTIM_32K_CLK_GATE_MASK;
    if (!(OM_PMU->RSVD_SW_REG[0] & PMU_RSVD_SW_REG_RTC_SECOND_RESTORE_PROCESS_MASK)) {
        OM_PMU->BASIC |= PMU_BASIC_RTC_CLK_GATE_MASK;
    }

    // retention ALL SRAM, icache ram, ble ram, pso ram when deep sleep
    uint32_t ram_ctrl = OM_PMU->RAM_CTRL_2 & (~(0xFFFU << 18));
    OM_PMU->RAM_CTRL_2 =  ram_ctrl | PMU_RAM_CTRL_2_RAM0_DS_HW_CTRL_EN_MASK | PMU_RAM_CTRL_2_RAM1_DS_HW_CTRL_EN_MASK
                         | PMU_RAM_CTRL_2_RAM2_DS_HW_CTRL_EN_MASK | PMU_RAM_CTRL_2_RAM_PSO_DS_HW_CTRL_EN_MASK
                         | PMU_RAM_CTRL_2_RAM_BLE_DS_HW_CTRL_EN_MASK | PMU_RAM_CTRL_2_RAM_IC_DS_HW_CTRL_EN_MASK;
    ram_ctrl = OM_PMU->RAM_CTRL_1 & (~((3U << 30) | (3U << 26) | (3U << 22)));
    OM_PMU->RAM_CTRL_1 = ram_ctrl | PMU_RAM_CTRL_1_RAM3_DS_HW_CTRL_EN_MASK | PMU_RAM_CTRL_1_RAM4_DS_HW_CTRL_EN_MASK | PMU_RAM_CTRL_1_RAM5_DS_HW_CTRL_EN_MASK;

    #if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
    SCB->VTOR = (uint32_t) & (__VECTOR_TABLE[0]);
    #endif

    #if (__FPU_PRESENT == 1U) && (__FPU_USED == 1U)
    SCB->CPACR |= ((3U << 20U) | (3U << 22U));              /* Enable CP10 and CP11 Full Access */
    #endif

    #ifdef UNALIGNED_SUPPORT_DISABLE
    SCB->CCR |= SCB_CCR_UNALIGN_TRP_Msk;
    #endif

    // Enable DWT Cycle counter
    do {
        CoreDebug->DEMCR |= (1U << CoreDebug_DEMCR_TRCENA_Pos);
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    } while(0);
}


/** @} */
