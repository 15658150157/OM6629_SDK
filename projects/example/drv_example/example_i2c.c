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
 * @brief    example for using i2c
 * @details  example for using i2c: read and write eeprom
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


/*******************************************************************************
 * MACROS
 */
/// Test pad i2c scl
#define PAD_I2C0_SCL                    8
/// Test pad mux i2c scl
#define MUX_I2C0_SCL                    PINMUX_PAD8_I2C0_SCK_CFG
/// Test pad i2c sda
#define PAD_I2C0_SDA                    9
/// Test pad mux i2c sda
#define MUX_I2C0_SDA                    PINMUX_PAD9_I2C0_SDA_CFG

/* Use AT24C02 */
/// Address of eeprom
#define EEPROM_ADDR                     0x50U
/// Capacity of eeprom
#define EEPROM_CAPACITY                 256U
/// Test address in eeprom
#define TEST_ADDR                       0x40


/*******************************************************************************
 * TYPEDEFS
 */


/*******************************************************************************
 * CONST & VARIABLES
 */
/// Buffer that stores data to be sent
static uint8_t eeprom_tx_buf[EEPROM_CAPACITY];
/// Buffer that stores data to be read
static uint8_t eeprom_rx_buf[EEPROM_CAPACITY];
/// I2C pin configuration
static const pin_config_t pin_cfg [] = {
    {PAD_I2C0_SCL, {MUX_I2C0_SCL}, PMU_PIN_MODE_OD, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_I2C0_SDA, {MUX_I2C0_SDA}, PMU_PIN_MODE_OD, PMU_PIN_DRIVER_CURRENT_NORMAL},
};


/*******************************************************************************
 * LOCAL FUNCTIONS
 */


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief example of using i2c: read ,write eeprom
 *
 * @note When the I2C operates in master polling mode and there is no slave device connected,
 * @note to prevent getting stuck in the read/write function interface,
 * @note an external pull-up resistor needs to be connected,
 * @note or the pin mode of the clock and data lines should be changed from
 * @note PMU_PIN_MODE_OD (open-drain mode) to PMU_PIN_MODE_OD_PU (open-drain mode with internal pull-up).
 * @note In this way, an internal pull-up resistor will be included.
 *******************************************************************************
 */
void example_i2c(void)
{
    drv_pin_init(pin_cfg, sizeof(pin_cfg) / sizeof(pin_cfg[0]));

    i2c_config_t cfg = {
        .mode  = I2C_MODE_MASTER,        // 7-bit addressing mode
        .speed = I2C_SPEED_400K,
    };
    drv_i2c_init(OM_I2C0, &cfg);

    for (uint8_t i = 1; i < 8; i++) {
        eeprom_tx_buf[i] = 0x80 + i;
    }
    eeprom_tx_buf[0] = TEST_ADDR;

    // write tx_buf[1:4] from address TEST_ADDR
    drv_i2c_master_write(OM_I2C0, EEPROM_ADDR, eeprom_tx_buf, 5, DRV_MAX_DELAY);

    drv_dwt_delay_ms(20);  // eeprom internal write takes time

    // read data from address TEST_ADDR
    uint8_t rd_addr = TEST_ADDR;
    drv_i2c_master_read(OM_I2C0, EEPROM_ADDR, &rd_addr, 1, eeprom_rx_buf, 4, DRV_MAX_DELAY);
}

/** @} */