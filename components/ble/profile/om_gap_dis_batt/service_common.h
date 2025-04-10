/* ----------------------------------------------------------------------------
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 * -------------------------------------------------------------------------- */

/**
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */
/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>

#ifndef __SERVICE_COMMON_H__
#define __SERVICE_COMMON_H__

/*********************************************************************
 * MACROS
 */
/// GAP Device Name, 
#define GAP_DEVICE_NAME   "OM6626 Simple"
/// GAP APPEARANCE
#define GAP_APPEARANCE    "\xc2\x03"
/// DIS SYSTEM ID
#define DIS_SYSTEM_ID     "\x00\x00\x00\x00\x00\x00\x00\x00"
/// DIS HARD VERSION
#define DIS_HARD_VERSION  "1.0.0.0"
/// DIS SOFT VERSION 
#define DIS_SOFT_VERSION  "2.0.2.3"
/// DIS MANU NAME STR
#define DIS_MANU_NAME_STR "Beijing OnMicro Electronics Co., Ltd."

/// DIS PNP ID
#define DIS_PNP_ID        "\x01\x02\x03\x04\x05\x06\x07"
/// Battery service, set 1 to Enable
#define SERVICE_BATTARY   1

/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief Init common services, GAP, DIS, BATT included.
 *******************************************************************************
 */
void service_common_init(void);

#if SERVICE_BATTARY
/**
 *******************************************************************************
 * @brief Notify battery level changed.
 *
 * @param[in] val  The value of battary level
 *******************************************************************************
 */
void batt_level_change(uint8_t val);
#endif

#endif /* __SERVICE_COMMON_H__ */

/** @} */
