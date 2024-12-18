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
 * @brief    templete
 * @details  device description
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
#include "om_device.h"
#include "om_common.h"


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
#if (CONFIG_OM_ASSERT)

/* used for compiler happy when RTE_ISR is disable */
__WEAK void SystemFaultCallback(uint32_t fault_id, uint8_t *context, uint32_t context_len, uint32_t param)
{
    while(1);
}

/**
 *******************************************************************************
 * @brief  Reports the name of the source file and the source line number where
 *         the om_assert error has occurred. Applications maybe re-write this function.
 *
 * @param  file: pointer to the source file name
 * @param  line: om_assert error line source number
 *
 * @return None
 *******************************************************************************
 */
__WEAK void om_assert_failed(char *file, uint32_t line)
{
    extern void SystemFaultCallback(uint32_t fault_id, uint8_t *context, uint32_t context_len, uint32_t param);
    SystemFaultCallback(0U, (uint8_t *)file, strlen(file), line);   /* 0U is SYS_FAULT_ID_ASSERT define in drv_isr.h */

    while(1);
}
#endif  /* (CONFIG_OM_ASSERT) */


/** @} */
