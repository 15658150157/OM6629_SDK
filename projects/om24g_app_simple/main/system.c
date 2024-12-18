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
 * @brief    power manager for system
 * @details  power manager for system
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
#include "shell.h"
#include "om_driver.h"
#if (CONFIG_PM)
#include "pm.h"
#endif
#include "om_log.h"
// #include "test_common.h"


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
#if (CONFIG_PM)
void pm_sleep_callback(pm_sleep_state_t sleep_state, pm_status_t power_status)
{
    switch (sleep_state) {
        case PM_SLEEP_ENTRY:
            OM_LOG(OM_LOG_DEBUG, "enter\r\n");
            break;
        case PM_SLEEP_RESTORE_HSI:
            break;
        case PM_SLEEP_RESTORE_HSE:
            //shell_restore();
            break;
        case PM_SLEEP_LEAVE_BOTTOM_HALF:
            #if (CONFIG_SHELL)
            shell_restore();
            OM_LOG(OM_LOG_DEBUG, "leave \r\n");
            #endif
            //test_sleep_cnt++;
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
        om_printf("FAULT type: %s\r\n", fault_str[fault_id]);
    }

    if (fault_id == SYS_FAULT_ID_ASSERT) {
        om_printf("FAULT should be caused by OM_ASSERT at %s:%d\r\n", (char *)(&reg[0]), param);
    } else {
        om_printf("FAULT context:R0 :0x%08x, R1 :0x%08x, R2 :0x%08x, R3 :0x%08x\r\n", reg[0], reg[1], reg[2], reg[3]);
        om_printf("              R12:0x%08x, LR :0x%08x, PC :0x%08x, PSR:0x%08x\r\n", reg[4], reg[5], reg[6], reg[7]);
    }
    while (1);
}

/** @} */
