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
 * @brief    mesh app main entry
 * @details  mesh app main entry
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
#include "omsh_app.h"


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
int main(void)
{
    // Disable WDT
    drv_wdt_init(0);

    // Init internal flash
    flash_config_t config = {
        .clk_div    = 0,
        .delay      = 2,
        .read_cmd   = FLASH_FAST_READ_QIO,
        .write_cmd  = FLASH_PAGE_PROGRAM,
        .spi_mode   = FLASH_SPI_MODE_0,
    };
    drv_flash_init(OM_FLASH0, &config);

    // Init board
    board_init();

    // Initialize CMSIS-RTOS
    osKernelInitialize();

    // Start BLE Mesh Task
    vStartBleMeshTask();

    // Start thread execution
    if (osKernelGetState() == osKernelReady) {
        osKernelStart();
    }

    /* Never run here */
    while(1);
}

/** @} */