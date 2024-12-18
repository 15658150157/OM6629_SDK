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
 * @brief    system
 * @details  system
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
#include "om_log.h"
#include "om_driver.h"
#if (CONFIG_PM)
#include "pm.h"
#endif
#if (CONFIG_SHELL)
#include "shell.h"
#endif


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
#if (CONFIG_PM)
void pm_sleep_callback(pm_sleep_state_t sleep_state, pm_status_t power_status)
{
    switch (sleep_state) {
        case PM_SLEEP_ENTRY:
            break;
        case PM_SLEEP_RESTORE_HSI:
            break;
        case PM_SLEEP_RESTORE_HSE:
            break;
        case PM_SLEEP_LEAVE_BOTTOM_HALF:
            #if (CONFIG_SHELL)
            shell_restore();
            #endif
            #if (CONFIG_OM_LOG)
            om_log_restore();
            #endif
            break;
        default:
            break;
    }
}
#endif


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
void system_init(void)
{
    const flash_config_t config = {
        .clk_div = 0,
        .delay = 2,
        .read_cmd = FLASH_FAST_READ_DIO,
        .write_cmd = FLASH_PAGE_PROGRAM,
        .spi_mode = FLASH_SPI_MODE_0,
    };
    drv_wdt_init(0);
    drv_flash_init(OM_FLASH0, &config);

    #if (CONFIG_PM)
    pm_init();
    pm_sleep_enable(true);
    pm_sleep_notify_user_callback_register(pm_sleep_callback);
    #endif
}

void SystemFaultContextStore(sys_fault_id_t fault_id, uint8_t *context, uint32_t context_len, uint32_t param)
{
    const char *fault_str[] = {
        "Assert",
        "low voltage detect",
        "WDT interrupt",
        "Hard fault",
        "user check fault",
    };
    uint32_t *reg = (uint32_t *)context;

    if (fault_id <= SYS_FAULT_ID_USER) {
        OM_LOG(OM_LOG_ERROR, "FAULT type: %s\r\n", fault_str[fault_id]);
    }

    if (fault_id == SYS_FAULT_ID_ASSERT) {
        OM_LOG(OM_LOG_ASSERT, "FAULT should be caused by OM_ASSERT at %s:%d\r\n", (char *)(&reg[0]), param);
    } else {
        OM_LOG(OM_LOG_ERROR, "FAULT should be caused when PC run 0x%08x\r\ncontext:\r\n", reg[6]);
        OM_LOG_HEXDUMP(OM_LOG_ERROR, (uint8_t*)context, context_len, sizeof(uint32_t));
    }
    OM_LOG_FLUSH();

    while (1);
}

/** @} */
