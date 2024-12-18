/* ----------------------------------------------------------------------------
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 * -------------------------------------------------------------------------- */

/**
 * @defgroup RADIO RADIO
 * @ingroup  DRIVER
 * @brief    RADIO driver
 * @details  RADIO driver apis and typedefs header file
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __DRV_RADIO_H
#define __DRV_RADIO_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_driver.h"
#if (RTE_RADIO)
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * TYPEDEFS
 */
typedef enum {
    RF_TX_POWER_8P5DBM  = 105,
    RF_TX_POWER_8DBM    = 104,
    RF_TX_POWER_7DBM    = 103,
    RF_TX_POWER_6DBM    = 102,
    RF_TX_POWER_5P5DBM  = 101,
    RF_TX_POWER_5DBM    = 18,
    RF_TX_POWER_4P5DBM  = 16,
    RF_TX_POWER_4DBM    = 15,
    RF_TX_POWER_3P5DBM  = 13,
    RF_TX_POWER_3DBM    = 12,
    RF_TX_POWER_2P5DBM  = 11,
    RF_TX_POWER_2DBM    = 10,
    RF_TX_POWER_1P5DBM  = 9,
    RF_TX_POWER_1DBM    = 8,
    RF_TX_POWER_0P5DBM  = 7,
    RF_TX_POWER_0DBM    = 6,
    RF_TX_POWER_N1DBM   = 5,
    RF_TX_POWER_N5DBM   = 4,
    RF_TX_POWER_N9DBM   = 3,
    RF_TX_POWER_N18DBM  = 2,
    RF_TX_POWER_N47DBM  = 1,

    // Humanized description
    RF_TX_POWER_MAX    = RF_TX_POWER_8DBM,
    RF_TX_POWER_MIN    = RF_TX_POWER_N47DBM,
    RF_TX_POWER_NORMAL = RF_TX_POWER_0DBM,
} rf_tx_power_t;


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief  drv rf init
 *******************************************************************************
 */
extern void drv_rf_init(void);

/**
 *******************************************************************************
 * @brief  rf txrx pin enable
 *
 * @param[in] enable  enable
 * @param[in] pol  polarity, 0 or 1
 *******************************************************************************
 **/
extern void drv_rf_txrx_pin_enable(bool enable, int pol);

/**
 *******************************************************************************
 * @brief  rf carrier enable
 *
 * @param[in] enable  enable
 * @param[in] freq  2402MHz ... 2480MHz, If set freq to 2402.123456MHZ, freq_channel = 2402, fractFreq = 0.123456
 *******************************************************************************
 **/
extern void drv_rf_carrier_enable(bool enable, uint32_t freq, float fract_freq);

/**
 *******************************************************************************
 * @brief  rf single tone enable
 *
 * @param[in] enable  enable
 * @param[in] freq  2402MHz ... 2480MHz
 * @param[in] payload  payload (0-255)
 *******************************************************************************
 */
extern void drv_rf_single_tone_enable(bool enable, uint32_t freq, uint8_t payload);

/**
 *******************************************************************************
 * @brief  rf full rx enable
 *
 * @param[in] enable  enable
 * @param[in] freq  2402MHz ... 2480MHz
 *******************************************************************************
 **/
extern void drv_rf_full_rx_enable(bool enable, uint32_t freq);

/**
 *******************************************************************************
 * @brief  rf tx power set
 *
 * @param[in] auto_ctrl_by_ble  false: control by power param; true: auto control by BLE STACK
 * @param[in] power  power
 *******************************************************************************
 **/
extern void drv_rf_tx_power_set(bool auto_ctrl_by_ble, rf_tx_power_t power);


#ifdef __cplusplus
}
#endif

#endif  /* RTE_RADIO */

#endif  /* __DRV_RADIO_H */


/** @} */
