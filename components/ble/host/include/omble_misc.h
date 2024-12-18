/* ----------------------------------------------------------------------------
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 * -------------------------------------------------------------------------- */

/**
 * @addtogroup OMBLE_MISC
 * @brief MISC
 * @version
 * Version 1.0
 *  - Initial release
 *
 */
/// @{

#ifndef __OMBLE_MISC_H__
#define __OMBLE_MISC_H__
#include <stdint.h>
#include "omble_range.h"

/// omble 参数类型
enum ob_opt_cfg_type {
    OB_OPT_CFG_TYPE_GAP_MAX_EXT_ADV_EVENT                   = 0,
    OB_OPT_CFG_TYPE_GAP_ADV_SECD_MAX_SKIP                   = 1,
    OB_OPT_CFG_TYPE_GAP_ADV_SCAN_REQ_NTF_EN                 = 2,
    OB_OPT_CFG_TYPE_GAP_CE_LENGTH                           = 3,
    OB_OPT_CFG_TYPE_GAP_PIN_NONE_REQ_ENABLE                 = 4,
    OB_OPT_CFG_TYPE_SMP_PIN_CODE                            = 5,
    OB_OPT_CFG_TYPE_SMP_PRIVATE_KEY                         = 6,
};

/// omble gap_ce_length
struct gap_ce_length {
    uint16_t min; ///< ce length 最小值
    uint16_t max; ///< ce length 最大值
};

/// omble 参数结构体
typedef union {
    uint8_t gap_max_ext_adv_event;      ///< 扩展广播最大广播事件发送数
    uint8_t gap_max_adv_secd_max_skip;  ///< 扩展广播最大广播skip数
    uint8_t gap_adv_scan_req_ntf_en;    ///< 是否开启扩展广播中的scan notify事件
    struct gap_ce_length gap_ce_length; ///< 配置ce length 参数
    uint8_t gap_pin_none_req_enable;    ///< 是否在NO_INPUT_NO_OUTPUT模式配对时上报OB_GAP_EVT_PIN_REQUEST消息
    struct {                            ///< 配对时的pin code, 只写
        uint8_t conn_idx;
        int pin_code;
    } smp_pin_code;
    uint8_t smp_private_key[32];        ///< Private key, 只写
} ob_opt_cfg_t;

#endif /* __OMBLE_MISC_H__ */

/// @}
