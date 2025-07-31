/* ----------------------------------------------------------------------------
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 * -------------------------------------------------------------------------- */

/**
 * @defgroup WDT WDT
 * @ingroup  DRIVER
 * @brief    WDT driver
 * @details  WDT driver apis and typedefs header file
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __DRV_WDT_H
#define __DRV_WDT_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_driver.h"
#if (RTE_WDT)
#include <stddef.h>
#include "om_device.h"
#include "om_driver.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * TYPEDEFS
 */
/// WDT control
typedef enum {
    WDT_CONTROL_ENABLE_NMI_INT     = 0U,   /* Enable/Disable NMI interrupt, default enable NMI interrupt after wdt enable */
    WDT_CONTROL_CLEAR_INT_FLAG     = 1U,   /* Clear WDT interrupt flag */
} wdt_control_t;


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief Watchdog initialization, enable APB clock, and would not clear reset flag
 *
 * @param[in] timeout_ms     Timeout in unit ms, range in [0, 8191992] ms
 *******************************************************************************
 */
__STATIC_FORCEINLINE void drv_wdt_init(uint32_t timeout_ms)
{
    extern void drv_wdt_enable(uint32_t timeout_ms);
    drv_wdt_enable(timeout_ms);

    if (timeout_ms == 0U) {
        while(OM_PMU->WDT_STATUS & PMU_WDT_STATUS_LD_WDT_KR_STATUS_MASK);
        OM_PMU->WDT_KR_CFG = 0x6666;
    }
}

#if (RTE_WDT_REGISTER_CALLBACK)
/**
 *******************************************************************************
 * @brief wdt register callback
 *
 * @param[in] isr_cb         callback
 *******************************************************************************
 */
extern void drv_wdt_register_isr_callback(drv_isr_callback_t isr_cb);
#endif

/**
 *******************************************************************************
 * @brief The interrupt callback for WDT driver. It is a weak function. User
 *        should define their own callback in user file, other than modify it in
 *        the wdt driver.
 *        If application want to reboot/reset system immediately, user shall call
 *        drv_pmu_reset() with reboot reason as PMU_REBOOT_FROM_WDT.
 *        If application used keep alive in isr or isr_callback, please clear interrupt
 *        flag, call drv_wdt_control with argument as WDT_CONTROL_CLEAR_INT_FLAG.
 *******************************************************************************
 */
extern void drv_wdt_isr_callback(void);

/**
 *******************************************************************************
 * @brief Control WDT
 *
 * @param[in] control   control options
 * @param[in] argu      argument for control options
 *
 *******************************************************************************
 */
static inline void drv_wdt_control(wdt_control_t control, void *argu)
{
    switch (control) {
        case WDT_CONTROL_ENABLE_NMI_INT:
            if ((uint32_t)argu) {
                OM_PMU->WDT_STATUS |= PMU_WDT_STATUS_WDT_INT_NMI_EN_MASK;
            } else {
                OM_PMU->WDT_STATUS &= ~PMU_WDT_STATUS_WDT_INT_NMI_EN_MASK;
            }
            break;
        case WDT_CONTROL_CLEAR_INT_FLAG:
            OM_PMU->WDT_STATUS |= PMU_WDT_STATUS_WDT_FLAG_CLR_MASK;  // clear WDT int flag
            break;
        default:
            break;
    }
}

/**
 *******************************************************************************
 * @brief Kick watchdog, it can open wdt even if it is closed
 *******************************************************************************
 */
extern void drv_wdt_keep_alive(void);

/**
 *******************************************************************************
 * @brief WDT interrupt service routine, It is not recommended to feed the dog in
 *        the WDT ISR.
 *******************************************************************************
 */
extern void drv_wdt_isr(void);


#ifdef __cplusplus
}
#endif

#endif  /* RTE_WDT */

#endif  /* __DRV_WDT_H */


/** @} */
