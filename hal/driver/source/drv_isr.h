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
 * @brief    interrupt service routine
 * @details  interrupt service routine
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __DRV_ISR_H
#define __DRV_ISR_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_driver.h"
#if (RTE_ISR)
#include <stdint.h>

#ifdef  __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */
typedef enum {
    SYS_FAULT_ID_ASSERT       = 0U,    /* Assert Fault */
    SYS_FAULT_ID_LOW_VOLTAGE  = 1U,    /* low voltage detect */
    SYS_FAULT_ID_WDT          = 2U,    /* WDT interrupt */
    SYS_FAULT_ID_HARD_FAULT   = 3U,    /* hard fault */
    SYS_FAULT_ID_USER         = 4U,    /* user check fault */

    SYS_FAULT_ID_MAX          = 0xFFFFFFFFU,
} sys_fault_id_t;


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief System Fault Callback. The function will be called when system fault.
 *        Hardfault（MemManage_Handler/BusFault_Handler/UsageFault_Handler）,
 *        OM_ASSERT, WDT（NMI）Handler，call it.
 *        User self-check failed may call this function, It need User added.
 *
 *        The processing as follows:
 *        1. The disable global interrupt
 *        2. keep alive for WDT
 *        3. saved RTC second counter
 *        4. store fault context. call Fault_Context_Store()
 *        5. reset system
 *
 *        Context is different by the fault_id. As follows:
 *        SYS_FAULT_ID_ASSERT:
 *                             context is __FILE__ name string, context_len is
 *                             __FILE__ name string len, param is __LINE__ number.
 *
 *        SYS_FAULT_ID_LOW_VOLTAGE:
 *        SYS_FAULT_ID_WDT:
 *        SYS_FAULT_ID_HARD_FAULT:
 *                              context is pointer to stacked regs, as follows:
 *                              struct stacked_reg_tag {
 *                                  uint32_t stacked_r0;
 *                                  uint32_t stacked_r1;
 *                                  uint32_t stacked_r2;
 *                                  uint32_t stacked_r3;
 *                                  uint32_t stacked_r12;
 *                                  uint32_t stacked_lr;
 *                                  uint32_t stacked_pc;
 *                              } stacked_regs;
 *                              and context_len is fixed 28 Byte;
 *                              param is exception return code(EXC_RETURN).
 *
 * @param fault_id      fault id
 * @param context       the context for the fault_id
 * @param context_len   valid length in bytes for context
 * @param param         extra parameter for fault_id
 *
 *******************************************************************************
 **/
extern void SystemFaultCallback(sys_fault_id_t fault_id, uint8_t *context, uint32_t context_len, uint32_t param);

/**
 *******************************************************************************
 * @brief Fault Context store. It is a weak function. The default Context store is
 *        endless loop. the SystemFaultContextStore is called by SystemFaultCallback.
 *        User may define own callback in user/project file, not modify it in the
 *        isr driver. the parameter is the same as SystemFaultCallback function.
 *
 *        Context is different by the fault_id. As follows:
 *        SYS_FAULT_ID_ASSERT:
 *                             context is __FILE__ name string, context_len is
 *                             __FILE__ name string len, param is __LINE__ number.
 *
 *        SYS_FAULT_ID_LOW_VOLTAGE:
 *        SYS_FAULT_ID_WDT:
 *        SYS_FAULT_ID_HARD_FAULT:
 *                              context is pointer to stacked regs, as follows:
 *                              struct stacked_reg_tag {
 *                                  uint32_t stacked_r0;
 *                                  uint32_t stacked_r1;
 *                                  uint32_t stacked_r2;
 *                                  uint32_t stacked_r3;
 *                                  uint32_t stacked_r12;
 *                                  uint32_t stacked_lr;
 *                                  uint32_t stacked_pc;
 *                              } stacked_regs;
 *                              and context_len is fixed 28 Byte;
 *                              param is exception return code(EXC_RETURN).
 *
 * @param fault_id      fault id
 * @param context       the context for the fault_id
 * @param context_len   valid length in bytes for context
 * @param param         extra parameter for fault_id
 *
 *******************************************************************************
 **/
extern void SystemFaultContextStore(sys_fault_id_t fault_id, uint8_t *context, uint32_t context_len, uint32_t param);

#ifdef  __cplusplus
}
#endif

#endif  /* (RTE_ISR) */

#endif  /* __DRV_ISR_H */

/** @} */
