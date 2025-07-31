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
 * @brief    WDT driver source file
 * @details  WDT driver source file
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
#include "RTE_driver.h"
#if (RTE_WDT)
#include <stddef.h>
#include "om_device.h"
#include "om_driver.h"


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    drv_isr_callback_t      isr_cb;
} wdt_env_t;


/*******************************************************************************
 * CONST & VARIABLES
 */
#if (RTE_WDT_REGISTER_CALLBACK)
static wdt_env_t wdt_env = {
    .isr_cb = NULL,
};
#endif


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
#if (RTE_WDT_REGISTER_CALLBACK)
/**
 * @brief wdt register callback
 *
 * @param[in] isr_cb  callback
 *
 * @return None
 **/
void drv_wdt_register_isr_callback(drv_isr_callback_t isr_cb)
{
    wdt_env.isr_cb = isr_cb;
    if (isr_cb != NULL) {
        // Enable WDT cpu en int
        REGW1(&OM_PMU->WDT_STATUS, PMU_WDT_STATUS_WDT_INT_CPU_EN_MASK);

        // Enable WDT NVIC int
        NVIC_ClearPendingIRQ(WDT_IRQn);
        NVIC_SetPriority(WDT_IRQn, RTE_WDT_IRQ_PRIORITY);
        NVIC_EnableIRQ(WDT_IRQn);
    } else {
        REGW0(&OM_PMU->WDT_STATUS, PMU_WDT_STATUS_WDT_INT_CPU_EN_MASK);
        NVIC_DisableIRQ(WDT_IRQn);
    }
}
#endif

__WEAK void drv_wdt_isr_callback(void)
{
    #if (RTE_WDT_REGISTER_CALLBACK)
    if (wdt_env.isr_cb) {
        wdt_env.isr_cb(NULL, DRV_EVENT_COMMON_GENERAL, NULL, NULL);
    }
    #endif
}

/**
 * @brief watch dog keepalive
 *
 * @return None
 **/
void drv_wdt_keep_alive(void)
{
    // keep wdt alive can open wdt even if it has been closed
    OM_PMU->WDT_KR_CFG = 0xAAAA;
}

/**
 * @brief enable watch dog
 *
 * @param[in] timeout_ms  timeout with second
 **/
void drv_wdt_enable(uint32_t timeout_ms)
{
    REGW0(&OM_PMU->WDT_STATUS, PMU_WDT_STATUS_WDT_INT_CPU_EN_MASK);
    if (timeout_ms) {
        drv_wdt_control(WDT_CONTROL_ENABLE_NMI_INT, (void *)1U);
        while(OM_PMU->WDT_STATUS & PMU_WDT_STATUS_LD_WDT_KR_STATUS_MASK);
        OM_PMU->WDT_RLR_CFG = timeout_ms * 128 / 1000;
        OM_PMU->WDT_KR_CFG = 0x5555;
        while(OM_PMU->WDT_STATUS & PMU_WDT_STATUS_LD_WDT_KR_STATUS_MASK);
        OM_PMU->WDT_KR_CFG = 0xAAAA;
    }
}

void drv_wdt_isr(void)
{
    drv_wdt_isr_callback();
}


#endif  /* RTE_WDT */

/** @} */
