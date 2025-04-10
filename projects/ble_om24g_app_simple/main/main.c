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
 * @brief    main entry
 * @details  main entry
 * @version
 *
 * Version 1.0
 *  - Initial release
 *
 * @{
 */


/*******************************************************************************
 * INCLUDES
 */
#include "cmsis_os2.h"
#include "bsp.h"
#include "om_driver.h"

/*******************************************************************************
 * Extern FUNCTIONS
 */
extern void vStartEvtTask(void);
extern void system_init(void);

/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
int main(void)
{
#if 1
    drv_icache_enable();
    flash_config_t config = {
        .clk_div = 0,
        .delay = 2,
        .read_cmd = FLASH_FAST_READ_DIO,
        .write_cmd = FLASH_PAGE_PROGRAM,
        .spi_mode = FLASH_SPI_MODE_0,
    };
    drv_flash_init(OM_FLASH0, &config);
#endif
    board_init();
    system_init();
    drv_wdt_init(0);

    // Initialize CMSIS-RTOS
    osKernelInitialize();

    // Start Evt Task
    vStartEvtTask();

    // Start thread execution
    if (osKernelGetState() == osKernelReady) {
        osKernelStart();
    }

    /* Never run here */
    while(1);
}

/** @} */

