/* ----------------------------------------------------------------------------
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 * -------------------------------------------------------------------------- */

/**
 * @defgroup SYSTEM SYSTEM
 * @ingroup  DEVICE
 * @brief    SYSTEM device
 * @details  SYSTEM device apis and typedefs header file
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */


#ifndef __SYSTEM_DEVICE_H
#define __SYSTEM_DEVICE_H
/*******************************************************************************
 * INCLUDES
 */
#include <stdint.h>
#include "RTE_OM662x.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (RTE_SYSTEM_USING_ROM_SYMBOL)
#define SystemEnterDeepSleep                rom_SystemEnterDeepSleep
#define SystemExitDeepSleep                 rom_SystemExitDeepSleep
#define SystemFromRomExitDeepSleepEnable    rom_SystemFromRomExitDeepSleepEnable
#define SystemRunTo                         rom_SystemRunTo
#endif

/*******************************************************************************
 * TYPEDEFS
 */
/**
  \brief Exception / Interrupt Handler Function Prototype
*/
typedef void (*VECTOR_TABLE_Type)(void);


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief Setup the microcontroller system.
 *
 *******************************************************************************
 */
extern void SystemInit(void);

/**
 *******************************************************************************
 * @brief  system init post
 *******************************************************************************
 */
extern void SystemInitPost(void);

/**
 *******************************************************************************
 * @brief  system init post
 *******************************************************************************
 */
extern void SystemInitPostLib(void);

/**
 *******************************************************************************
 * @brief  system config
 *******************************************************************************
 */
void SystemConfig(void);

/**
 *******************************************************************************
 * @brief  system enter deep sleep
 *******************************************************************************
 **/
extern void SystemEnterDeepSleep(void);

/**
 *******************************************************************************
 * @brief  system exit deep sleep
 *******************************************************************************
 **/
extern void SystemExitDeepSleep(void);

/**
 *******************************************************************************
 * @brief  system from rom exit deep sleep enable
 *******************************************************************************
 **/
extern void SystemFromRomExitDeepSleepEnable(void);

/**
 *******************************************************************************
 * @brief  system run to
 *
 * @param[in] vector_base  vector base
 *******************************************************************************
 */
extern void SystemRunTo(uint32_t vector_base);

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_DEVICE_H */


/** @} */
