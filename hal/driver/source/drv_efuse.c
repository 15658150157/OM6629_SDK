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
 * @brief    EFUSE driver source file
 * @details  EFUSE driver source file
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
#if (RTE_EFUSE)
#include <stdint.h>
#include <string.h>
#include "om_device.h"
#include "om_driver.h"


/*******************************************************************************
 * MACROS
 */
#define EFUSE_PROGRAM_EN(ctrl)          (OM_EFUSE->PROGRAM_ENABLE = (ctrl) ? 1 : 0)


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    drv_isr_callback_t  isr_cb;
    uint16_t            prgm_cnt;
    uint8_t            *prgm_buf;
    uint16_t            prgm_num;
    uint16_t            prgm_addr;
} efuse_env_t;


/*******************************************************************************
 * CONST & VARIABLES
 */
static efuse_env_t efuse_env = {
    .isr_cb = NULL,
    .prgm_cnt = 0U,
    .prgm_buf = NULL,
    .prgm_num = 0U,
    .prgm_addr = 0U,
};

static const drv_resource_t efuse_resource = {
    .cap = CAP_EFUSE,
    .reg = OM_EFUSE,
    .env = &efuse_env,
    .irq_num = EFUSE_IRQn,
    .irq_prio = RTE_EFUSE_IRQ_PRIORITY,
};


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static uint32_t efuse_get_size(void)
{
    return register_get(&efuse_resource.cap, MASK_POS(CAP_EFUSE_SIZE));
}


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief Initialize efuse
 *
 * @return om_error
 *******************************************************************************
 */
om_error_t drv_efuse_init(void)
{
    om_error_t error;

    DRV_RCC_RESET(RCC_CLK_EFUSE);

    DRV_WAIT_US_UNTIL_TO(!(OM_EFUSE->STATUS & EFUSE_STATUS_CTRL_STATE_MASK), 100, error);

    NVIC_ClearPendingIRQ(efuse_resource.irq_num);
    NVIC_SetPriority(efuse_resource.irq_num, efuse_resource.irq_prio);
    NVIC_EnableIRQ(efuse_resource.irq_num);

    return error;
}

/**
 *******************************************************************************
 * @brief Write efuse
 * NOTE: 1. before calling this function, the efuse program must be enabled
 *       2. write efuse need calibed rc32mhz or xtal32mhz
 *
 * @param[in] addr          Efuse address
 * @param[in] data          Pointer to data to be written
 * @param[in] length        Length of data to be written
 *
 * @return om_error
 *******************************************************************************
 */
om_error_t drv_efuse_write(uint32_t addr, const void *data, uint32_t length)
{
    OM_ASSERT(length != 0);

    if (addr + length > efuse_get_size()) {
        EFUSE_PROGRAM_EN(0);
        return OM_ERROR_OUT_OF_RANGE;
    }

    while (!(OM_EFUSE->STATUS & EFUSE_STATUS_CTRL_STATE_MASK));

    for (uint32_t i = 0; i < length; i++) {
        OM_EFUSE->PROGRAM_ADDRESS = addr++;
        OM_EFUSE->PROGRAM_DATA = ((uint8_t *)data)[i];

        OM_EFUSE->PROGRAM_START |= EFUSE_PROGRAM_START_MASK;
        while (OM_EFUSE->PROGRAM_START & EFUSE_PROGRAM_START_MASK);
    }

    EFUSE_PROGRAM_EN(0);

    return OM_ERROR_OK;
}

/**
 *******************************************************************************
 * @brief Write efuse using interrupt
 *
 * @param[in] addr          Efuse address
 * @param[in] data          Pointer to data to be written
 * @param[in] length        Length of data to be written
 *
 * @return om_error
 *******************************************************************************
 */
om_error_t drv_efuse_write_int(uint32_t addr, const void *data, uint32_t length)
{
    OM_ASSERT(length != 0);

    if (addr + length > efuse_get_size()) {
        EFUSE_PROGRAM_EN(0);
        return OM_ERROR_PARAMETER;
    }

    efuse_env.prgm_addr = addr;
    efuse_env.prgm_buf  = (uint8_t *)data;
    efuse_env.prgm_num  = length;
    efuse_env.prgm_cnt  = 0;

    OM_EFUSE->PROGRAM_INTR |= EFUSE_PROGRAM_INTR_INT_EN_MASK;

    OM_EFUSE->PROGRAM_ADDRESS = addr;
    OM_EFUSE->PROGRAM_DATA = ((uint8_t *)data)[0];
    OM_EFUSE->PROGRAM_START |= EFUSE_PROGRAM_START_MASK;

    return OM_ERROR_OK;
}

/**
 *******************************************************************************
 * @brief Read efuse
 *
 * @param[in] addr          Efuse address
 * @param[out] data         Pointer to buffer to store read data
 * @param[in] length        Length of data to be read
 *
 * @return om_error
 *******************************************************************************
 */
om_error_t drv_efuse_read(uint32_t addr, void *data, uint32_t length)
{
    OM_ASSERT(length != 0);

    memcpy(data, (uint8_t *)OM_EFUSE->READ_DATA + addr, length);

    return OM_ERROR_OK;
}

#if (RTE_EFUSE_REGISTER_CALLBACK)
/**
 *******************************************************************************
 * @brief Register efuse callback
 *
 * @param[in] cb            Callback function
 *******************************************************************************
 */
void drv_efuse_resgister_isr_callback(drv_isr_callback_t cb)
{
    efuse_env.isr_cb = cb;
}
#endif

/**
 *******************************************************************************
 * @brief Efuse interrupt handler
 *
 * @param[in] event         Event type
 * @param[in] addr          Efuse wrote address
 * @param[in] num           Efuse wrote length
 *******************************************************************************
 */
__WEAK void drv_efuse_isr_callback(drv_event_t event, uint32_t addr, uint32_t num)
{
    #if (RTE_EFUSE_REGISTER_CALLBACK)
    if (efuse_env.isr_cb != NULL) {
        efuse_env.isr_cb(OM_EFUSE, event, (void *)addr, (void *)num);
    }
    #endif
}

/**
 *******************************************************************************
 * @brief Efuse control
 *
 * @param[in] control       Efuse control type
 * @param[in] argu          Control argument
 *
 * @return om_error or efuse size
 *******************************************************************************
 */
void *drv_efuse_control(efuse_control_t control, void *argu)
{
    uint32_t ret;
    uint32_t prog_forbid_type;
    uint8_t last_byte;
    uint8_t last_index;

    ret = (uint32_t)OM_ERROR_OK;
    switch (control) {
        case EFUSE_CONTROL_GET_SIZE:
            ret = efuse_get_size();
            break;

        case EFUSE_CONTROL_PROGRAM_EN:
            EFUSE_PROGRAM_EN((uint32_t)argu);
            break;

        case EFUSE_CONTROL_FETCH_UID:
            OM_EFUSE->CTRL |= EFUSE_CTRL_UID_READ_MASK;
            while(OM_EFUSE->CTRL & EFUSE_CTRL_UID_READ_MASK);
            break;

        case EFUSE_CONTROL_CFG_FORBID:
            prog_forbid_type = (uint32_t)argu;
            last_index = efuse_get_size() - 1;
            drv_efuse_read(last_index, &last_byte, 1);
            if (prog_forbid_type == EFUSE_PROG_FORBID_0_63) {
                last_byte |= 0x10;
            } else if (prog_forbid_type == EFUSE_PROG_FORBID_64_239) {
                last_byte |= 0x20;
            } else if( prog_forbid_type == EFUSE_PROG_FORBID_240_500) {
                last_byte |= 0x40;
            } else if (prog_forbid_type == EFUSE_PROG_FORBID_501_507) {
                last_byte |= 0x80;
            } else {
                return (void *)OM_ERROR_PARAMETER;
            }
            EFUSE_PROGRAM_EN(1);
            drv_efuse_write(last_index, &last_byte, 1);
            EFUSE_PROGRAM_EN(0);
            OM_EFUSE->CTRL |= EFUSE_CTRL_PROGRAM_FORBID_MASK;
            while ((OM_EFUSE->CTRL & EFUSE_CTRL_PROGRAM_FORBID_MASK) || (!(OM_EFUSE->STATUS & EFUSE_STATUS_CTRL_STATE_MASK)));
            break;

        case EFUSE_CONTROL_GET_FORBID_STATUS:
            ret = register_get(&OM_EFUSE->CTRL, MASK_POS(EFUSE_CTRL_PMU_PROG_FORBID));
            break;

        case EFUSE_CONTROL_CLK_SRC_SWITCH:
            if ((uint32_t)argu) {
                OM_CPM->EFUSE_CFG |= CPM_EFUSE_CFG_CLK_SRC_SEL_MASK;
            } else {
                OM_CPM->EFUSE_CFG &= ~CPM_EFUSE_CFG_CLK_SRC_SEL_MASK;
            }
            while (!(OM_CPM->EFUSE_CFG & CPM_EFUSE_CFG_CLK_SYNC_DONE_MASK));

        default:
            ret = OM_ERROR_PARAMETER;
            break;
    }

    return (void *)ret;
}

/**
 *******************************************************************************
 * @brief Efuse interrupt service routine
 *******************************************************************************
 */
void drv_efuse_isr(void)
{
    if (OM_EFUSE->PROGRAM_INTR & EFUSE_PROGRAM_INTR_INT_GEN_MASK) {
        OM_EFUSE->PROGRAM_INTR |= EFUSE_PROGRAM_INTR_INT_CLR_MASK;

        efuse_env.prgm_cnt++;
        if (efuse_env.prgm_cnt < efuse_env.prgm_num) {
            OM_EFUSE->PROGRAM_ADDRESS = efuse_env.prgm_addr + efuse_env.prgm_cnt;
            OM_EFUSE->PROGRAM_DATA = ((uint8_t *)efuse_env.prgm_buf)[efuse_env.prgm_cnt];
            OM_EFUSE->PROGRAM_START |= EFUSE_PROGRAM_START_MASK;
        } else {
            EFUSE_PROGRAM_EN(0);
            OM_EFUSE->PROGRAM_INTR &= ~EFUSE_PROGRAM_INTR_INT_EN_MASK;
            drv_efuse_isr_callback(DRV_EVENT_COMMON_WRITE_COMPLETED, (uint32_t)efuse_env.prgm_addr, (uint32_t)efuse_env.prgm_num);
        }
    }
}


#endif

/** @} */
