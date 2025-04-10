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
extern void vStartOm24gTask(void);
extern void system_init(void);

/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
int main(void)
{
    board_init();
    system_init();
    drv_rf_init();

    // Initialize CMSIS-RTOS
    osKernelInitialize();

    // Start Om24g Task
    vStartOm24gTask();

    // Start thread execution
    if (osKernelGetState() == osKernelReady) {
        osKernelStart();
    }

    /* Never run here */
    while(1);
}

/** @} */

