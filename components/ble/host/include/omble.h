/* ----------------------------------------------------------------------------
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 * -------------------------------------------------------------------------- */

/**
 * @addtogroup OMBLE_COMMON BLE_COMMON
 * @brief OMBLE初始化接口
 * @version
 * Version 1.0
 *  - Initial release
 *
 */
/// @{
#ifndef __OMBLE_API_H__
#define __OMBLE_API_H__
#include <stdint.h>
#include <stdlib.h>
#include "omble_gap.h"
#include "omble_gatt.h"
#include "omble_misc.h"
#include "omble_error.h"

/// BLE 消息结构体
typedef struct {
    union {
        omble_gap_evt_t  gap;  ///< gap 消息，参考 @ref omble_gap_evt_t
        omble_gatt_evt_t gatt; ///< gatt 消息，参考 @ref omble_gatt_evt_t
    };
} omble_evt_t;

/// 协议栈初始化参数
struct ob_stack_param {
    uint8_t max_connection;        ///< 最大连接数
    uint8_t max_ext_adv_set;       ///< 最大广播数
    uint16_t max_att_mtu;          ///< MAX ATT MTU
    uint16_t max_gatt_serv_num;    ///< MAX GATT service number
    uint16_t max_gatt_write_cache; ///< MAX GATT server prepare write cache length, Use MAX mtu if set to 0
    bool smp_sc_support;           ///< Secure Connection Pairing support
    uint8_t max_user_event_num;    ///< MAX user event number
    uint8_t max_period_adv_set;    ///< 最大周期广播数
    uint8_t max_period_adv_sync;   ///< 最大周期广播同步数
};

/**@brief BLE 事件回调类型
 * @note 所有蓝牙相关消息均通过该结构体通知上层应用
 * @param[in] evt_id  事件id，参考 @ref OB_GAP_EVENTS, @ref OB_GATT_EVENTS
 * @param[in] evt  事件结构体，参考 @ref omble_evt_t
 */
typedef void (*omble_event_cb_t)(uint16_t evt_id, const omble_evt_t *evt);

/**@brief 注册 BLE 事件回调函数
 * @note回调可以多次注册，各回调函数按照注册顺序依次调用
 * @param[in]  callback   回调函数，参考@ref omble_event_cb_t
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_event_callback_reg(omble_event_cb_t callback);

/**@brief 注销 BLE 事件回调函数
 * @param[in]  callback   回调函数，参考@ref omble_event_cb_t
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_event_callback_unreg(omble_event_cb_t callback);

/**@brief 终止 BLE 事件处理
 *
 * 当调用@ref ob_event_callback_reg 多次注册BLE事件回调后，在@ref omble_event_cb_t
 * 函数中调用后将不再回调其它已注册的回调函数
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_event_abort(void);

/**@brief 不常用的参数配置
 * @param[in]  type  参数类型，参考@ref ob_opt_cfg_type
 * @param[in]  cfg   需要配置的参数，参考@ref ob_opt_cfg_t
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_opt_cfg_set(uint8_t type, ob_opt_cfg_t *cfg);

/**@brief 获取不常用的参数
 * @param[in]  type  参数类型，参考@ref ob_opt_cfg_type
 * @param[in]  cfg   需要获取的参数，参考@ref ob_opt_cfg_t
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_opt_cfg_get(uint8_t type, ob_opt_cfg_t *cfg);

/**@brief 初始化协议栈
 * @param[in]  param  协议栈初始化参数，参考 @ref ob_stack_param
 * @note 该接口只能调用一次，并且必须在调用其它蓝牙接口前调用
 * @return 执行结果，参考@ref ob_error
 */
uint32_t omble_init(struct ob_stack_param *param);

/**@brief 设置用户事件回调函数
 * @param[in]  event_id  事件ID，id值应小于 @ref ob_stack_param 中的max_user_event_num值
 * @param[in]  callback  协议栈初始化参数，参考 @ref ob_stack_param
 * @return 执行结果，参考@ref ob_error
 */
uint32_t omble_set_user_event_callback(uint8_t event_id, void(*callback)(void));

/**@brief 触发用户事件回调函数
 * @param[in]  event_id  事件ID，id值应小于 @ref ob_stack_param 中的max_user_event_num值
 * @return 执行结果，参考@ref ob_error
 */
uint32_t omble_user_event_trigger(uint8_t event_id);

/**@brief 获取Time Tick
 * @return tick
 */
size_t omble_tick_get(void);

/**@brief 计算tick2 - tick1的时间间隔，单位10ms
 * @param[in]  tick1  tick1
 * @param[in]  tick2  tick2
 * @return tick2 - tick1 的时间间隔，单位10ms
 */
int omble_tick_diff_10ms(size_t tick1, size_t tick2);

/**@brief 调度协议栈
 */
void omble_schedule(void);

#endif /* __OMBLE_API_H__ */
/// @}
