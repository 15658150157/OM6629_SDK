/* ----------------------------------------------------------------------------
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 * -------------------------------------------------------------------------- */

/**
 * @file     obc.h
 * @brief    obc
 * @date     15 December 2021
 * @author   OnMicro SW Team
 *
 * @defgroup obc obc
 * @ingroup  OBC
 * @brief    obc Driver
 * @details  obc Driver

 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __OBC_H__
#define __OBC_H__

#ifdef __cplusplus
extern "C"
{ /*}*/
#endif

/*********************************************************************
 * INCLUDES
 */
#include "obc_hci_h4.h"
#include "obc_pta.h"

/*********************************************************************
 * MACROS
 */


/*********************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * EXTERN VARIABLES
 */


/*********************************************************************
 * EXTERN FUNCTIONS
 */

/**
 *******************************************************************************
 * @brief  obc init
 *******************************************************************************
 */
void obc_init(void);

/**
 *******************************************************************************
 * @brief  obc isr
 *******************************************************************************
 */
void obc_isr(void);

/**
 *******************************************************************************
 * @brief  obc bb frame ongoing callback register
 *
 * @param[in] cb  cb
 * @param[in] is_ongoing  is ongoing
 *******************************************************************************
 */
void obc_bb_frame_ongoing_callback_register(void (*cb)(bool is_ongoing));

#ifdef __cplusplus
/*{*/ }
#endif

#endif

/** @} */

