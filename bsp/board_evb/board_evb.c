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
 * @brief    EVB board pin define
 * @details  EVB board pin define
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
#include "om_driver.h"
#include "board_evb.h"


/*******************************************************************************
 * MACROS
 */
/* see @pmu_32k_sel_t, selection in RC32K/XTAL32K/DIV from XTAL32M */
#define BOARD_32K_SELECT            PMU_32K_SEL_RC


/*******************************************************************************
 * CONSTANTS
 */
static const pin_config_t pin_config[] = {
    #if (RTE_UART0)
    {PAD_UART0_TXD, {PINMUX_PAD16_UART0_TRX_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_UART0_RXD, {PINMUX_PAD17_UART0_RX_CFG},  PMU_PIN_MODE_PU, PMU_PIN_DRIVER_CURRENT_NORMAL},
    #endif

    #if (RTE_UART1)
    {PAD_UART1_TXD, {PINMUX_PAD4_UART1_TRX_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_UART1_RXD, {PINMUX_PAD3_UART1_RX_CFG},  PMU_PIN_MODE_PU, PMU_PIN_DRIVER_CURRENT_NORMAL},
    #endif

    #if (defined(PAD_RF_TXEN) && defined(PAD_RF_RXEN))
    {PAD_RF_RXEN, {PINMUX_PAD3_RX_EXT_PD_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_RF_TXEN, {PINMUX_PAD4_TX_EXT_PD_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    #endif

    {PAD_LED_0, {PINMUX_GPIO_MODE_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_LED_1, {PINMUX_GPIO_MODE_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_LED_2, {PINMUX_GPIO_MODE_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_LED_3, {PINMUX_GPIO_MODE_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_LED_4, {PINMUX_GPIO_MODE_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_LED_5, {PINMUX_GPIO_MODE_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},

    {PAD_BUTTON_0, {PINMUX_GPIO_MODE_CFG}, PMU_PIN_MODE_PU, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_BUTTON_1, {PINMUX_GPIO_MODE_CFG}, PMU_PIN_MODE_PU, PMU_PIN_DRIVER_CURRENT_NORMAL},
};

static const gpio_config_t gpio_config[] = {
    {OM_GPIO0, PAD_LED_0,    GPIO_DIR_OUTPUT, LED_OFF_LEVEL, GPIO_TRIG_NONE},
    {OM_GPIO0, PAD_LED_1,    GPIO_DIR_OUTPUT, LED_OFF_LEVEL, GPIO_TRIG_NONE},
    {OM_GPIO0, PAD_LED_2,    GPIO_DIR_OUTPUT, LED_OFF_LEVEL, GPIO_TRIG_NONE},
    {OM_GPIO0, PAD_LED_3,    GPIO_DIR_OUTPUT, LED_OFF_LEVEL, GPIO_TRIG_NONE},
    {OM_GPIO0, PAD_LED_4,    GPIO_DIR_OUTPUT, LED_OFF_LEVEL, GPIO_TRIG_NONE},
    {OM_GPIO0, PAD_LED_5,    GPIO_DIR_OUTPUT, LED_OFF_LEVEL, GPIO_TRIG_NONE},
    {OM_GPIO0, PAD_BUTTON_0, GPIO_DIR_INPUT,  LED_OFF_LEVEL, GPIO_TRIG_RISING_FAILING_EDGE},
    {OM_GPIO0, PAD_BUTTON_1, GPIO_DIR_INPUT,  LED_OFF_LEVEL, GPIO_TRIG_RISING_FAILING_EDGE},
};


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
void board_init(void)
{
    // Use DCDC (Default is LDO mode)
    drv_pmu_dcdc_enable(true);

    drv_pmu_xtal32m_startup();
    drv_pmu_select_32k(PMU_32K_SEL_RC);
    drv_pin_init(pin_config, sizeof(pin_config) / sizeof(pin_config[0]));
    drv_gpio_init(gpio_config, sizeof(gpio_config) / sizeof(gpio_config[0]));
}


/** @} */
