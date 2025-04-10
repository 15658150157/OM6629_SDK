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
 * @brief    example for using lcd
 * @details  example for using lcd:
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
#include "../../../bsp/source/glcd.h"

/*******************************************************************************
 * MACROS
 */
#define PAD_LCD_CS0     21
#define PAD_LCD_SCK     14
#define PAD_LCD_IO0     18
#define PAD_LCD_IO1     15
#define PAD_LCD_IO2     20
#define PAD_LCD_IO3     19
#define PAD_LCD_RST     8

#define MUX_LCD_CS0     PINMUX_PAD21_LCD_CS0_CFG
#define MUX_LCD_SCK     PINMUX_PAD14_LCD_SCK_CFG
#define MUX_LCD_IO0     PINMUX_PAD18_LCD_IO0_CFG
#define MUX_LCD_IO1     PINMUX_PAD15_LCD_IO1_CFG
#define MUX_LCD_IO2     PINMUX_PAD20_LCD_IO2_CFG
#define MUX_LCD_IO3     PINMUX_PAD19_LCD_IO3_CFG
#define MUX_LCD_RST     PINMUX_PAD8_GPIO_MODE_CFG

#define PIC_BUFF_LEN    (100*100*3)
/*******************************************************************************
 * TYPEDEFS
 */


/*******************************************************************************
 * CONST & VARIABLES
 */
static uint8_t pic_buf_rgb565[PIC_BUFF_LEN];

/// Pinmux Configuration
static pin_config_t pin_config[] = {
    {PAD_LCD_CS0, {MUX_LCD_CS0}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_LCD_SCK, {MUX_LCD_SCK}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_LCD_IO0, {MUX_LCD_IO0}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_LCD_IO1, {MUX_LCD_IO1}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_LCD_IO2, {MUX_LCD_IO2}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_LCD_IO3, {MUX_LCD_IO3}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_LCD_RST, {MUX_LCD_RST}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
};

/// GPIO Configuration
static gpio_config_t gpio_config[] = {
    {OM_GPIO0, PAD_LCD_RST,  GPIO_DIR_OUTPUT, GPIO_LEVEL_HIGH, GPIO_TRIG_NONE},
};

/*******************************************************************************
 * LOCAL FUNCTIONS
 */

/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief example of using lcd
 *
 *******************************************************************************
 */
void example_lcd(void)
{
    // white pic
    for (uint32_t i = 0; i < PIC_BUFF_LEN; i++) {
        pic_buf_rgb565[i] = 0xFF;
    }

    uint8_t  clk_div = 0;
    uint16_t x1 = 0;
    uint16_t y1 = 0;
    uint16_t x2 = 200;
    uint16_t y2 = 200;
    uint8_t* display_data = (uint8_t*)pic_buf_rgb565;

    drv_pin_init(pin_config, sizeof(pin_config) / sizeof(pin_config[0]));
    drv_gpio_init(gpio_config, sizeof(gpio_config) / sizeof(gpio_config[0]));

    glcd_init(clk_div, (uint32_t)OM_GPIO0, PAD_LCD_RST);
    glcd_set_disp_window(x1, y1, x2, y2);
    glcd_disp_flush(display_data, (x2 - x1 + 1) * (y2 - y1 + 1) * LCD_BYTES_PER_PIX);
    glcd_disp_flush_finish();
}



/** @} */