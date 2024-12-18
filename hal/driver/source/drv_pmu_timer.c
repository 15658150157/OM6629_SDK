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
 * @brief    PMU_TIMER driver source file
 * @details  PMU_TIMER driver source file
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
#if (RTE_PMU_TIMER)
#include "om_device.h"
#include "om_driver.h"


/*******************************************************************************
 * CONST & VARIABLES
 */
pmu_timer_env_t pmu_timer_env = {
    .isr0_cb = NULL,
    .isr1_cb = NULL,
};

static const drv_resource_t pmu_timer_resource = {
    .cap      = 0U,
    .reg      = OM_PMU,
    .env      = &pmu_timer_env,
    .irq_num  = PMU_TIMER_IRQn,
    .irq_prio = RTE_PMU_TIMER_IRQ_PRIORITY,
};


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief initialize pmu_timer
 *
 * @return None
 *******************************************************************************
 */
void drv_pmu_timer_init(void)
{
    OM_PMU_Type *pmu_timer;

    pmu_timer = (OM_PMU_Type *)pmu_timer_resource.reg;

    // Clear and Enable pmu timer IRQ
    NVIC_ClearPendingIRQ(pmu_timer_resource.irq_num);
    NVIC_SetPriority(pmu_timer_resource.irq_num, pmu_timer_resource.irq_prio);
    NVIC_EnableIRQ(pmu_timer_resource.irq_num);

    // start count
    register_set1(&pmu_timer->TIMER_CTRL, PMU_TIMER_CTRL_PMU_TIMER_EN_MASK);
}

void drv_pmu_timer_trig_set(pmu_timer_trig_t trig_type, uint32_t trig_cnt)
{
    OM_PMU_Type *pmu_timer;
    pmu_timer = (OM_PMU_Type *)pmu_timer_resource.reg;

    OM_CRITICAL_BEGIN();
    if (trig_type == PMU_TIMER_TRIG_VAL0) {
        pmu_timer->TIMER_SET0 = pmu_timer_env.TIMER_SET0 = trig_cnt;
        pmu_timer->TIMER_CTRL |= PMU_TIMER_CTRL_PMU_TIMER_INT_VAL0_EN_MASK;
    } else if (trig_type == PMU_TIMER_TRIG_VAL1) {
        pmu_timer->TIMER_SET1 = pmu_timer_env.TIMER_SET1 = trig_cnt;
        pmu_timer->TIMER_CTRL |= PMU_TIMER_CTRL_PMU_TIMER_INT_VAL1_EN_MASK;
    }

    #if (PMU_TIMER_DEBUG)
    pmu_timer_env.record[trig_type].is_enabled = true;
    pmu_timer_env.record[trig_type].cur_cnt_when_set = drv_pmu_timer_cnt_get();
    pmu_timer_env.record[trig_type].cur_cnt_when_overflow = 0;
    pmu_timer_env.record[trig_type].value = trig_cnt;
    #endif
    OM_CRITICAL_END();
}

/**
 *******************************************************************************
 * @brief get pmu timer left time(ms)
 *
 * @param[in] trig_type    trigger type
 *
 * @return
 *  - 0xFFFFFFFF : invalid time
 *  - others : left time(tick)
 *******************************************************************************
 */
__RAM_CODES("PM")
uint32_t drv_pmu_timer_left_time_get(pmu_timer_trig_t trig_type)
{
    uint32_t int_is_en;
    uint32_t left_tick;
    uint32_t timer_set;
    uint32_t timer_cnt;

    int_is_en = OM_PMU->TIMER_CTRL & ((trig_type == PMU_TIMER_TRIG_VAL0) ? PMU_TIMER_CTRL_PMU_TIMER_INT0_MASK : PMU_TIMER_CTRL_PMU_TIMER_INT1_MASK);
    if (int_is_en) {
        timer_set = (uint32_t)drv_pmu_timer_control(trig_type, PMU_TIMER_CONTROL_GET_TIMER_VAL, NULL);
        timer_cnt = drv_pmu_timer_cnt_get();

        if (timer_set > timer_cnt) {
            left_tick = timer_set - timer_cnt;
        } else {
            left_tick = PMU_TIMER_MAX_TICK + timer_set - timer_cnt;
        }
        left_tick = left_tick > PMU_TIMER_MAX_DELAY ? 0U : left_tick;

        return left_tick;
    }
    return PMU_TIMER_MAX_DELAY;
}

#if (RTE_PMU_TIMER_REGISTER_CALLBACK)
/**
 *******************************************************************************
 * @brief pmu timer register callback
 *
 * @param[in] cb    callback
 *
 * @return None
 *******************************************************************************
 */
void drv_pmu_timer_register_isr_callback(pmu_timer_trig_t trig_type, drv_isr_callback_t cb)
{
    if (trig_type == PMU_TIMER_TRIG_VAL0) {
        pmu_timer_env.isr0_cb = cb;
    } else if (trig_type == PMU_TIMER_TRIG_VAL1) {
        pmu_timer_env.isr1_cb = cb;
    }
}
#endif

__WEAK void drv_pmu_timer_isr_callback(pmu_timer_trig_t trig_type, uint32_t timer_set)
{
    #if (RTE_PMU_TIMER_REGISTER_CALLBACK)
    #if (PMU_TIMER_DEBUG)
    pmu_timer_env.record[trig_type].cur_cnt_when_overflow = drv_pmu_timer_cnt_get();
    #endif

    if (trig_type == PMU_TIMER_TRIG_VAL0) {
        if (pmu_timer_env.isr0_cb) {
            pmu_timer_env.isr0_cb(OM_PMU, DRV_EVENT_COMMON_GENERAL, (void *)trig_type, (void *)timer_set);
        }
    } else if (trig_type == PMU_TIMER_TRIG_VAL1) {
        if (pmu_timer_env.isr1_cb) {
            pmu_timer_env.isr1_cb(OM_PMU, DRV_EVENT_COMMON_GENERAL, (void *)trig_type, (void *)timer_set);
        }
    }
    #endif
}

/**
 *******************************************************************************
 * @brief pmu timer interrupt service routine
 *
 *******************************************************************************
 */
void drv_pmu_timer_isr(void)
{
    uint32_t status;
    OM_PMU_Type *pmu_timer;

    pmu_timer = (OM_PMU_Type *)pmu_timer_resource.reg;
    while (1) {
        status = pmu_timer->TIMER_CTRL;

        // while(1){}: prevent digital lost INT0 irq when INT1 irq is also pending
        if ((status & (PMU_TIMER_CTRL_PMU_TIMER_INT0_MASK | PMU_TIMER_CTRL_PMU_TIMER_INT1_MASK)) == 0) {
            break;
        }

        if (status & PMU_TIMER_CTRL_PMU_TIMER_INT0_MASK) {
            // clear interrupt status
            // Fix: when 2 pmutimers are running as same time, stop one, may lead another stoped issue.
            REGW0(&pmu_timer->TIMER_CTRL, PMU_TIMER_CTRL_PMU_TIMER_INT_VAL0_EN_MASK);
            REGW1(&pmu_timer->TIMER_CTRL, PMU_TIMER_CTRL_PMU_TIMER_INT_VAL0_CLR_MASK);
            REGW0(&pmu_timer->TIMER_CTRL, PMU_TIMER_CTRL_PMU_TIMER_INT_VAL0_CLR_MASK);
            // callback
            drv_pmu_timer_isr_callback(PMU_TIMER_TRIG_VAL0, pmu_timer_env.TIMER_SET0);
        }

        if (status & PMU_TIMER_CTRL_PMU_TIMER_INT1_MASK) {
            // clear interrupt status
            REGW0(&pmu_timer->TIMER_CTRL, PMU_TIMER_CTRL_PMU_TIMER_INT_VAL1_EN_MASK);
            REGW1(&pmu_timer->TIMER_CTRL, PMU_TIMER_CTRL_PMU_TIMER_INT_VAL1_CLR_MASK);
            REGW0(&pmu_timer->TIMER_CTRL, PMU_TIMER_CTRL_PMU_TIMER_INT_VAL1_CLR_MASK);
            // callback
            drv_pmu_timer_isr_callback(PMU_TIMER_TRIG_VAL1, pmu_timer_env.TIMER_SET1);
        }
    }
}


#endif  /* RTE_PMU_TIMER */


/** @} */
