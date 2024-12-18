/* ----------------------------------------------------------------------------
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 * -------------------------------------------------------------------------- */

/**
 * @addtogroup OMBLE_GATT
 * @brief GATT
 * @details GATT相关接口主要控制蓝牙的GATT client、GATT server通讯
 * @version
 * Version 1.0
 *  - Initial release
 *
 */
/// @{

#ifndef __OMBLE_GATT_H__
#define __OMBLE_GATT_H__
#include <stdint.h>
#include "omble_range.h"

/// UUID 16bit 长度值
#define OB_UUID_16BIT    2
/// UUID 128bit 长度值
#define OB_UUID_128BIT  16

// GATT

/// OB_GATT_EVENTS
enum OB_GATT_EVENTS {
    /// Notification 或 Write Command发送完成事件，参考 @ref ob_gatt_evt_tx_complete_t
    OB_GATT_EVT_TX_COMPLETE              = OB_GATT_EVTS_BASE + 0,
    /// MTU 更新完成事件，参考 @ref ob_gatt_evt_mtu_exchanged_t
    OB_GATT_EVT_MTU_EXCHANGED            = OB_GATT_EVTS_BASE + 1,
    /// 请求超时事件，参考 @ref ob_gatt_evt_timeout_t
    OB_GATT_EVT_TIMEOUT                  = OB_GATT_EVTS_BASE + 2,
    /// Indication响应事件，参考 @ref ob_gatts_evt_indicate_cfm_t
    OB_GATTS_EVT_INDICATE_CFM            = OB_GATT_EVTS_BASE + 3,
    /// 读请求事件，参考 @ref ob_gatts_evt_read_req_t
    OB_GATTS_EVT_READ_REQ                = OB_GATT_EVTS_BASE + 4,
    /// 写请求事件，参考 @ref ob_gatts_evt_write_req_t
    OB_GATTS_EVT_WRITE_REQ               = OB_GATT_EVTS_BASE + 5,
    /// 服务发现完成事件，参考 @ref ob_gattc_evt_find_serv_rsp_t
    OB_GATTC_EVT_FIND_SERV_RSP           = OB_GATT_EVTS_BASE + 6,
    /// 服务发现完成事件，参考 @ref ob_gattc_evt_find_serv_by_uuid_rsp_t
    OB_GATTC_EVT_FIND_SERV_BY_UUID_RSP   = OB_GATT_EVTS_BASE + 7,
    /// 特征发现完成事件，参考 @ref ob_gattc_evt_find_char_rsp_t
    OB_GATTC_EVT_FIND_CHAR_RSP           = OB_GATT_EVTS_BASE + 8,
    /// 描述发现完成事件，参考 @ref ob_gattc_evt_find_desc_rsp_t
    OB_GATTC_EVT_FIND_DESC_RSP           = OB_GATT_EVTS_BASE + 9,
    /// 读完成事件，参考 @ref ob_gattc_evt_read_rsp_t
    OB_GATTC_EVT_READ_RSP                = OB_GATT_EVTS_BASE + 10,
    /// 读完成事件，参考 @ref ob_gattc_evt_read_by_uuid_rsp_t
    OB_GATTC_EVT_READ_BY_UUID_RSP        = OB_GATT_EVTS_BASE + 11,
    /// 写完成事件，参考 @ref ob_gattc_evt_write_rsp_t
    OB_GATTC_EVT_WRITE_RSP               = OB_GATT_EVTS_BASE + 12,
    /// notification或indication接收事件，参考 @ref ob_gattc_evt_hvx_ind_t
    OB_GATTC_EVT_HVX_IND                 = OB_GATT_EVTS_BASE + 13,
};

/// att属性
enum ob_att_prop {
    OB_ATT_PROP_IND       = 1 << 5,
    OB_ATT_PROP_NTF       = 1 << 4,
    OB_ATT_PROP_WRITE     = 1 << 3,
    OB_ATT_PROP_WRITE_CMD = 1 << 2,
    OB_ATT_PROP_READ      = 1 << 1,
};

/// att属性结构体
typedef struct {
    const uint8_t *uuid;    ///< ATT UUID
    uint8_t uuid_len;       ///< ATT UUID长度
    uint8_t att_prop;       ///< ATT 属性，参考@ref ob_att_prop
    uint8_t att_perm_read;  ///< ATT 读权限，非0时表示只有链路加密后GATT client才能进行read操作
    uint8_t att_perm_write; ///< ATT 写权限，非0时表示只有链路加密后GATT client才能进行write操作
} ob_gatt_item_t;

/// gatt服务结构体
typedef struct {
    const uint8_t *uuid;        ///< Service UUID
    uint16_t uuid_len;          ///< Service UUID长度
    uint16_t att_num;           ///< att数量
    const ob_gatt_item_t *item; ///< ATT属性数组，用于定义GATT服务中包含的character和descriptor
} ob_gatt_serv_t;

/// OB_GATT_HVX_TYPE
enum ob_gatt_hvx_type {
    OB_HANDLE_VALUE_NTF,  ///< notification 类型
    OB_HANDLE_VALUE_IND,  ///< indication 类型
};

/// GATT写类型
enum ob_gattc_write_type {
    OB_GATTC_WRITE_REQ,  ///< write request 类型
    OB_GATTC_WRITE_CMD,  ///< write no response 类型
};

/// gatt handle value notify/indicate结构体
typedef struct {
    uint8_t type;           ///< OB_HANDLE_VALUE_NTF or OB_HANDLE_VALUE_IND, ref @ref ob_gatt_hvx_type
    uint16_t att_hdl;       ///< att handle
    const uint8_t *data;    ///< 数据，长度不应大于当前MTU值
    int len;                ///< 数据长度
    uint16_t id;            ///< 与OB_GATT_EVT_TX_COMPLETE事件中的id值对应，用于发送完成的消息
} ob_gatts_hvx_t;

/// Event structure for @ref OB_GATT_EVT_TX_COMPLETE.
typedef struct {
    uint16_t id;            ///< Notification 或 Write Command消息已经发送完成时数据对应id
} ob_gatt_evt_tx_complete_t;

/// Event structure for @ref OB_GATT_EVT_MTU_EXCHANGED.
typedef struct {
    uint16_t mtu;           ///< 更新后的MTU值
} ob_gatt_evt_mtu_exchanged_t;

/// Event structure for @ref OB_GATT_EVT_TIMEOUT.
typedef struct {
    uint8_t att_opcode;     ///< 超时的指令
} ob_gatt_evt_timeout_t;

/// Event structure for @ref OB_GATTS_EVT_INDICATE_CFM.
typedef struct {
    uint16_t att_hdl;       ///< att handle
} ob_gatts_evt_indicate_cfm_t;

/// Event structure for @ref OB_GATTS_EVT_READ_REQ.
typedef struct {
    uint16_t att_hdl;       ///< att handle
    uint16_t offset;        ///< data offset
} ob_gatts_evt_read_req_t;

/// Event structure for @ref OB_GATTS_EVT_WRITE_REQ.
typedef struct {
    uint16_t att_hdl;       ///< att handle
    const uint8_t *data;    ///< data
    uint16_t len;           ///< data length
} ob_gatts_evt_write_req_t;

/// Gatt Service结构体
typedef struct {
    uint16_t start_hdl;     ///< start handle
    uint16_t end_hdl;       ///< end handle
    const uint8_t *uuid;    ///< service UUID
    uint8_t uuid_len;       ///< length of UUID
} ob_gatt_service_t;

/// Event structure for @ref OB_GATTC_EVT_FIND_SERV_RSP.
typedef struct {
    uint32_t status;                ///< 状态
    ob_gatt_service_t *service;     ///< gatt service 数组
    uint8_t service_num;            ///< gatt service 数量
} ob_gattc_evt_find_serv_rsp_t;

/// Event structure for @ref OB_GATTC_EVT_FIND_SERV_BY_UUID_RSP.
typedef struct {
    uint32_t status;                ///< 状态
    ob_gatt_service_t *service;     ///< gatt service 数组
    uint8_t service_num;            ///< gatt service 数量
} ob_gattc_evt_find_serv_by_uuid_rsp_t;

/// Gatt Characteristic结构体
typedef struct {
    uint16_t value_hdl;     ///< value handle
    uint8_t properties;     ///< properties
    const uint8_t *uuid;    ///< char UUID
    uint8_t uuid_len;       ///< length of UUID
} ob_gatt_characteristic_t;

/// Event structure for @ref OB_GATTC_EVT_FIND_CHAR_RSP.
typedef struct {
    uint32_t status;                          ///< 状态
    ob_gatt_characteristic_t *characteristic; ///< gatt char 数组
    uint8_t char_num;                         ///< gatt char 数量
} ob_gattc_evt_find_char_rsp_t;

/// Gatt Descriptor结构体
typedef struct {
    uint16_t att_hdl;       ///< att handle
    const uint8_t *uuid;    ///< char UUID
    uint8_t uuid_len;       ///< length of UUID
} ob_gatt_descriptor_t;

/// Event structure for @ref OB_GATTC_EVT_FIND_DESC_RSP.
typedef struct {
    uint32_t status;                    ///< 状态
    ob_gatt_descriptor_t *descriptor;   ///< gatt descriptor 数组
    uint8_t desc_num;                   ///< gatt descriptor 数量
} ob_gattc_evt_find_desc_rsp_t;

/// Event structure for @ref OB_GATTC_EVT_READ_RSP.
typedef struct {
    uint32_t status;        ///< 状态
    uint16_t att_hdl;       ///< att handle
    uint16_t offset;        ///< read offset
    const uint8_t *data;    ///< data
    uint16_t len;           ///< data length
} ob_gattc_evt_read_rsp_t;

/// Event structure for @ref OB_GATTC_EVT_READ_BY_UUID_RSP.
typedef struct {
    uint32_t status;        ///< 状态
    uint16_t att_hdl;       ///< att handle
    const uint8_t *data;    ///< data
    uint16_t len;           ///< data length
} ob_gattc_evt_read_by_uuid_rsp_t;

/// Event structure for @ref OB_GATTC_EVT_WRITE_RSP.
typedef struct {
    uint32_t status;        ///< 写入状态
    uint16_t att_hdl;       ///< att handle
} ob_gattc_evt_write_rsp_t;

/// Event structure for @ref OB_GATTC_EVT_HVX_IND.
typedef struct {
    uint8_t type;           ///< @ref ob_gatt_hvx_type
    uint16_t att_hdl;       ///< att handle
    const uint8_t *data;    ///< data
    uint16_t len;           ///< data length
} ob_gattc_evt_hvx_ind_t;

/// GATT 消息结构体
typedef struct {
    uint8_t conn_idx; ///< 连接 index
    union {
        ob_gatt_evt_tx_complete_t             tx_complete;            ///< Notify 或 Write Cmd发送完成事件参数
        ob_gatt_evt_mtu_exchanged_t           mtu_exchanged;          ///< MTU 更新完成事件参数
        ob_gatt_evt_timeout_t                 timeout;                ///< 请求超时事件参数
        ob_gatts_evt_indicate_cfm_t           indicate_cfm;           ///< Indication响应事件参数
        ob_gatts_evt_read_req_t               read_req;               ///< 读请求事件参数
        ob_gatts_evt_write_req_t              write_req;              ///< 写请求事件参数
        ob_gattc_evt_find_serv_rsp_t          find_serv_rsp;          ///< 服务发现完成事件参数
        ob_gattc_evt_find_serv_by_uuid_rsp_t  find_serv_by_uuid_rsp;  ///< 服务发现完成事件参数
        ob_gattc_evt_find_char_rsp_t          find_char_rsp;          ///< 特征发现完成事件参数
        ob_gattc_evt_find_desc_rsp_t          find_desc_rsp;          ///< 描述发现完成事件参数
        ob_gattc_evt_read_rsp_t               read_rsp;               ///< 读完成事件参数
        ob_gattc_evt_read_by_uuid_rsp_t       read_by_uuid_rsp;       ///< 读完成事件参数
        ob_gattc_evt_write_rsp_t              write_rsp;              ///< 写完成事件参数
        ob_gattc_evt_hvx_ind_t                hvx_ind;                ///< notification或indication接收事件参数
    };
} omble_gatt_evt_t;

/// ATT Primary Service定义
extern const uint8_t ob_att_serv_def[2];
/// ATT Primary Service定义
extern const uint8_t ob_att_secs_def[2];
/// ATT Include Service定义
extern const uint8_t ob_att_incl_def[2];
/// ATT Characteristic定义
extern const uint8_t ob_att_char_def[2];
/// ATT Characteristic User Description Descriptor定义
extern const uint8_t ob_att_cudd_def[2];
/// ATT Client Characteristic Configuration Descriptor定义
extern const uint8_t ob_att_cccd_def[2];
/// ATT Report Reference Descriptor定义
extern const uint8_t ob_att_rrd_def[2];

/**@brief 添加GATT服务
 * @param[in]   att_serv       服务内容，参考@ref ob_gatt_serv_t
 * @param[out]  start_handle   服务添加成功后服务起始handle值将被写入到start_handle
 * @note 服务添加的总数应小于@ref omble_init 的参数 max_gatt_serv_num
 * @note 新添加的服务起始handle值从1开始递增
 * @warning 参数att_serv指向的变量必须为全局或static变量
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gatts_add_service(const ob_gatt_serv_t *att_serv, uint16_t *start_handle);

/**@brief 设置服务可见性
 * @details 服务被设置为不可见后当GATT client 搜索服务时，将自动略过被隐藏的service，但其所占用的handle值将被保留
 * @param[in]  serv_hdl       服务起始handle
 * @param[in]  visible        服务是否可见
 * @note 参数 serv_hdl 必须是 @ref ob_gatts_add_service 返回的start_handle值
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gatts_set_service_visibility(uint16_t serv_hdl, uint8_t visible);

/**@brief 发送notification或indication，仅作为GATT server时可调用
 * @details 发送notification后当数据实际发送完成时会收到 @ref OB_GATT_EVT_TX_COMPLETE 事件，但在收到该事件前可以连续调用ob_gatts_send_hvx来缓存需要发送的notification以提高通讯速率\n
 * @details 发送indication后对端设备会回应@ref OB_GATTS_EVT_INDICATE_CFM 事件
 * @param[in]  conn_idx       连接index
 * @param[in]  hvx            数据参数
 * @note 调用ob_gatts_send_hvx连续发送notification时要注意未发出的数据是需要占用内存进行缓存的，因此应根据@ref OB_GATT_EVT_TX_COMPLETE 事件控制被缓存的数据量以防内存被耗尽
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gatts_send_hvx(uint8_t conn_idx, const ob_gatts_hvx_t *hvx);

/**@brief 响应读请求
 * @details 该接口用于响应@ref OB_GATTS_EVT_READ_REQ 所请求的attribute，请求的attribute handle值参考@ref ob_gatts_evt_read_req_t
 * @param[in]  conn_idx       连接index
 * @param[in]  att_state      读状态，参考@ref ob_gatt_error_t ，若不为success，则data参数无用
 * @param[in]  data           数据
 * @param[in]  len            长度，若超出可发送长度则数据内容自动截断
 * @note 数据的可发送长度为当前MTU-1，数据长度超出时将自动截断\n
 * @note 若读取的attribute数据内容超出MTU-1，则GATT Client会再次发起read操作，且读取参数@ref ob_gatts_evt_read_req_t 中的offset为需要读取数据的偏移量
 * @note 若读取操作需要返回错误，则将att_state设置为响应的错误码即可，GATT的错误码参考@ref ob_gatt_error_t
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gatts_read_response(uint8_t conn_idx, uint16_t att_state, const uint8_t *data,
                                uint16_t len);

/**@brief 响应写请求
 * @details 该接口用于响应@ref OB_GATTS_EVT_WRITE_REQ 请求
 * @param[in]  conn_idx       连接index
 * @param[in]  att_state      写状态，参考@ref ob_gatt_error_t
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gatts_write_response(uint8_t conn_idx, uint16_t att_state);

/**@brief 设置gatt server暂停响应client请求
 * @details 设置了暂停状态后，GATT server将缓存client的GATT请求暂时不上报给应用层，直到用户取消该暂停状态
 * @param[in]  conn_idx       连接index
 * @param[in]  pause_state    pending状态:1或0
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gatts_set_pause(uint8_t conn_idx, uint8_t pause_state);

/**@brief 请求MTU交换
 * @details 请求的MTU值为@ref omble_init 的参数 max_att_mtu
 * @param[in]  conn_idx       连接index
 * @note MTU参数更新成功后，若MTU值发生变化，会通过事件@ref OB_GATT_EVT_MTU_EXCHANGED 通知上层
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gattc_mtu_req(uint8_t conn_idx);

/**@brief 服务搜索 - 通过handle范围
 * @details 作为GATT Client在start_handle和end_handle之间搜索GATT Server服务，对方响应后会通过事件@ref OB_GATTC_EVT_FIND_SERV_RSP 通知上层
 * @param[in]  conn_idx       连接index
 * @param[in]  start_handle    handle 范围
 * @param[in]  end_handle    handle 范围
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gattc_find_service_by_handle(uint8_t conn_idx, uint16_t start_handle,
                                         uint16_t end_handle);

/**@brief 服务搜索 - 通过handle范围
 * @details 作为GATT Client在start_handle和end_handle之间搜索指定uuid的GATT Server服务，对方响应后会通过事件@ref OB_GATTC_EVT_FIND_SERV_BY_UUID_RSP 通知上层
 * @param[in]  conn_idx       连接index
 * @param[in]  start_handle   handle 范围
 * @param[in]  end_handle    handle 范围
 * @param[in]  uuid          UUID
 * @param[in]  uuid_len      UUID长度
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gattc_find_service_by_uuid(uint8_t conn_idx, uint16_t start_handle,
                                       uint16_t end_handle, const uint8_t *uuid, uint8_t uuid_len);

/**@brief 特征搜索
 * @details 作为GATT Client在start_handle和end_handle之间搜索Characteristic，对方响应后会通过事件@ref OB_GATTC_EVT_FIND_CHAR_RSP 通知上层
 * @param[in]  conn_idx       连接index
 * @param[in]  start_handle   handle 范围
 * @param[in]  end_handle    handle 范围
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gattc_find_characteristic(uint8_t conn_idx,
                                      uint16_t start_handle, uint16_t end_handle);

/**@brief 描述搜索
 * @details 作为GATT Client在start_handle和end_handle之间搜索Descriptor，对方响应后会通过事件@ref OB_GATTC_EVT_FIND_DESC_RSP 通知上层
 * @param[in]  conn_idx       连接index
 * @param[in]  start_handle   handle 范围
 * @param[in]  end_handle    handle 范围
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gattc_find_descriptor(uint8_t conn_idx, uint16_t start_handle, uint16_t end_handle);

/**@brief 读取请求
 * @details 作为GATT Client读取att_hdl对应的数据，对方响应后会通过事件@ref OB_GATTC_EVT_READ_RSP 通知上层
 * @param[in]  conn_idx       连接index
 * @param[in]  att_hdl        att handle
 * @param[in]  offset         数据偏移
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gattc_read(uint8_t conn_idx, uint16_t att_hdl, uint16_t offset);

/**@brief 读取请求
 * @details 作为GATT Client在start_handle和end_handle之间读取指定UUID的数据，对方响应后会通过事件@ref OB_GATTC_EVT_READ_BY_UUID_RSP 通知上层
 * @param[in]  conn_idx       连接index
 * @param[in]  start_handle   handle 范围
 * @param[in]  end_handle    handle 范围
 * @param[in]  uuid          UUID
 * @param[in]  uuid_len      UUID长度
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gattc_read_by_uuid(uint8_t conn_idx, uint16_t start_handle, uint16_t end_handle, const uint8_t *uuid,
                               uint8_t uuid_len);

/**@brief 写入请求
 * @details 作为GATT Client向att_hdl写入数据，对方响应后会通过事件@ref OB_GATTC_EVT_WRITE_RSP 通知上层 \n
 * @details 若参数type类型为OB_GATTC_WRITE_CMD，则不会产生响应事件@ref OB_GATTC_EVT_WRITE_RSP \n
 * @details 若参数type类型为OB_GATTC_WRITE_CMD，数据发送给对端设备后，会通过@ref OB_GATT_EVT_TX_COMPLETE 事件通知上层，可以连续调用ob_gattc_write来缓存需要写入的数据以提高通讯速率
 * @param[in]  conn_idx       连接index
 * @param[in]  att_hdl        att handle
 * @param[in]  type           write类型，参考@ref ob_gattc_write_type
 * @param[in]  data           数据
 * @param[in]  len            数据长度，超出当前MTU-3则自动截断
 * @note 调用ob_gattc_write连续发送数据时要注意未发出的数据是需要占用内存进行缓存的，因此应根据@ref OB_GATT_EVT_TX_COMPLETE 事件控制被缓存的数据量以防内存被耗尽
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gattc_write(uint8_t conn_idx, uint16_t att_hdl, uint8_t type,
                        const uint8_t *data, int len);

/**@brief indicate响应
 * @details 该接口用于响应@ref OB_GATTC_EVT_HVX_IND 请求(仅当@ref ob_gattc_evt_hvx_ind_t 参数中的type == OB_HANDLE_VALUE_IND 时)
 * @param[in]  conn_idx       连接index
 * @return 执行结果，参考@ref ob_error
 */
uint32_t ob_gattc_indicate_cfm(uint8_t conn_idx);

#endif /* __OMBLE_GATT_H__ */

/// @}
