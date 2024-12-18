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
#include "om_driver.h"
#include "example.h"


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
int main(void)
{
    static const pin_config_t pin_config[] = {
        {5,  {PINMUX_PAD5_UART1_TRX_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
        {6,  {PINMUX_PAD6_UART1_RX_CFG},  PMU_PIN_MODE_PU, PMU_PIN_DRIVER_CURRENT_NORMAL},
    };

    drv_wdt_init(0);
    drv_pin_init(pin_config, sizeof(pin_config)/sizeof(pin_config[0]));
    om_printf("Example project\r\n");
    // example_gpio();
    // example_gpadc();

    while(1);
}

void om_putchar(char character)
{
    while (OM_ERROR_OK != drv_uart_write(OM_UART1, (uint8_t *)&character, 1, DRV_MAX_DELAY));
}

/** @} */
