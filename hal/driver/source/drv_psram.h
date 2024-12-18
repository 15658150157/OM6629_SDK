/* ----------------------------------------------------------------------------
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 * -------------------------------------------------------------------------- */

/**
 * @file     drv_psram.h
 * @brief    PSRAM driver
 * @date     30 September 2024
 * @author   OnMicro SW Team
 *
 * @defgroup PSRAM
 * @ingroup  Peripheral
 * @brief    PSRAM With QPI Driver
 * @details  PSRAM With QPI Driver

 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __DRV_PSRAM_H
#define __DRV_PSRAM_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_driver.h"
#if (RTE_PSRAM)
#include <stdint.h>
#include "om_driver.h"

#ifdef  __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */
#define PSRAM_DELAY_AUTO                                0xFF
#define PSRAM_DELAY_MAX                                 0x0F
#define PSRAM_DELAY_DEFAULT                             0x02
#define PSRAM_AUTO_DLY_RETYR_CNT                        2
/* psram list functions, see ospi driver code for detail */
#define drv_psram_node_setup(om_psram, node, cfg)       drv_ospi_node_setup(om_psram, node, cfg)


/*******************************************************************************
 * TYPEDEFS
 */
typedef enum {
    PSRAM_READ,
    PSRAM_FAST_READ,
    PSRAM_FAST_READ_QUAD,
    PSRAM_WRITE,
    PSRAM_WRITE_QUAD,
    PSRAM_ENTER_QUAD,
    PSRAM_EXIT_QUAD,
    PSRAM_RESET_ENABLE,
    PSRAM_RESET,
    PSRAM_SET_BURST_LEN,
    PSRAM_READ_ID,

    PSRAM_QPI_CMD_MAX
} psram_cmd_t;

typedef struct {
    uint8_t man_id;             /**< manufacturer ID */
    uint8_t kgd;                /**< known good die */
    uint8_t eid[6];             /**< extended ID */
} psram_id_t;

typedef struct {
    uint8_t     clk_div;        /*!< Clock divider */
    uint8_t     delay;          /*!< Delay(2 ns for per step) */
    psram_cmd_t read_cmd;       /*!< Command for reading, see@psram_cmd_t */
    psram_cmd_t write_cmd;      /*!< Command for writing, see@psram_cmd_t */
    uint16_t    page_size;      /*!< Page size */
    uint8_t     page_cross_en;  /*!< Page cross enable */
} psram_config_t;

typedef ospi_list_node_t     psram_list_node_t;
typedef ospi_list_node_cfg_t psram_list_node_cfg_t;


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief Initialize psram
 *
 * @param[in] om_psram       The ospi pointer of psram
 * @param[in] psram_config   The ospi configuration
 * @return                   The error infomation
 *
 *******************************************************************************
 */
extern om_error_t drv_psram_init(OM_OSPI_Type *om_psram, const psram_config_t *psram_config);

/**
 *******************************************************************************
 * @brief Read with qpi
 *
 * @param[in] om_psram       The psram number
 * @param[in] addr           The address of psram
 * @param[in] data           The data pointer to store read data
 * @param[in] data_len       The data length to read
 * @return                   The error infomation
 *
 *******************************************************************************
 */
extern om_error_t drv_psram_read(OM_OSPI_Type *om_psram,
                                 uint32_t addr,
                                 uint8_t *data,
                                 uint32_t data_len);

/**
 *******************************************************************************
 * @brief Read with qpi and interrupt enabled
 *
 * @param[in] om_psram       The psram number
 * @param[in] addr           The address of psram
 * @param[in] data           The data pointer to store read data
 * @param[in] data_len       The data length to read
 * @return                   The error infomation
 *
 *******************************************************************************
 */
extern om_error_t drv_psram_read_int(OM_OSPI_Type *om_psram,
                                     uint32_t addr,
                                     uint8_t *data,
                                     uint32_t data_len);

/**
 *******************************************************************************
 * @brief Write with qpi
 *
 * @param[in] om_psram       The psram number
 * @param[in] addr           The address of psram
 * @param[in] data           The data pointer to store write data
 * @param[in] data_len       The data length to read
 * @return                   The error infomation
 *
 *******************************************************************************
 */
extern om_error_t drv_psram_write(OM_OSPI_Type *om_psram,
                                  uint32_t addr,
                                  uint8_t *data,
                                  uint32_t data_len);

/**
 *******************************************************************************
 * @brief Write with qpi with interrupt enabled
 *
 * @param[in] om_psram       The psram number
 * @param[in] addr           The address of psram
 * @param[in] data           The data pointer to store write data
 * @param[in] data_len       The data length to read
 * @return                   The error infomation
 *
 *******************************************************************************
 */
extern om_error_t drv_psram_write_int(OM_OSPI_Type *om_psram,
                                      uint32_t addr,
                                      uint8_t *data,
                                      uint32_t data_len);

/**
 *******************************************************************************
 * @brief Read ID in spi status
 *
 * @param[in] om_psram       The psram number
 * @param[in] id[2]          The array to store id
 * @return                   The error infomation
 *
 *******************************************************************************
 */
extern om_error_t drv_psram_read_id(OM_OSPI_Type *om_psram, psram_id_t *id);

/**
 * @brief Set new command for reading
 *
 * @param om_psram          The psram controller device address
 * @param read_cmd          The new read command, see@psram_cmd_t
 *
 * @return                  Error code, see@om_error_t
 */
extern om_error_t drv_psram_read_cmd_set(OM_OSPI_Type *om_psram, psram_cmd_t read_cmd);

/**
 * @brief Set new command for writing
 *
 * @param om_psram          The psram controller device address
 * @param read_cmd          The new write command, see@psram_cmd_t
 *
 * @return                  Error code, see@om_error_t
 */
extern om_error_t drv_psram_write_cmd_set(OM_OSPI_Type *om_psram, psram_cmd_t write_cmd);

/**
 *******************************************************************************
 * @brief Enter/Exit quad mode
 *
 * @param[in] om_psram      The psram number
 * @param[in] enable        The flag to enter or exit quad mode
 * @return The error infomation
 *
 *******************************************************************************
 */
extern om_error_t drv_psram_quad_mode_enable(OM_OSPI_Type *om_psram, uint8_t enable);

/**
 *******************************************************************************
 * @brief Reset psram
 *
 * @param[in] om_psram      The psram number
 * @return                  The error infomation
 *
 *******************************************************************************
 */
extern om_error_t drv_psram_reset(OM_OSPI_Type *om_psram);

/**
 *******************************************************************************
 * @brief Toggle burst length between 32 and page length
 *
 * @param[in] om_psram      The psram number
 * @return                  The error infomation
 *
 *******************************************************************************
 */
extern om_error_t drv_psram_set_burst_len(OM_OSPI_Type *om_psram);

/**
 *******************************************************************************
 * @brief Start list transfer
 *
 * @param[in] om_psram      Pointer to psram
 * @param[in] list_head     The first list node pointer
 * @return The error infomation
 *
 *******************************************************************************
 */
extern om_error_t drv_psram_list_start(OM_OSPI_Type *om_psram, psram_list_node_t *list_head);

/**
 *******************************************************************************
 * @brief psram qpi page size set
 *
 * @param[in] om_psram     Pointer to psram
 * @param[in] page_size    Psram page size
 *
 *******************************************************************************
 */
__STATIC_INLINE void drv_psram_set_page_size(OM_OSPI_Type *om_psram, const uint32_t page_size)
{
    register_set(&om_psram->MEM_TYPE0, MASK_1REG(OSPI_MEM_TYPE_PAGE_SIZE, page_size - 1));
}

#if (RTE_PSRAM_REGISTER_CALLBACK)
/**
 *******************************************************************************
 * @brief Register callback of psram
 *
 * @param[in] om_ospi      Pointer to ospi
 * @param[in] event_cb     The psram callback
 *
 * @return                 None
 *
 *******************************************************************************
 */
extern void drv_psram_register_isr_callback(OM_OSPI_Type *om_psram, drv_isr_callback_t event_cb);

/**
 *******************************************************************************
 * @brief The interrupt callback for psram controller driver. It is a weak function. User should define
 *        their own callback in user file, other than modify it in the psram driver.
 *
 * @param om_psram         The psram controller device address
 * @param event            The driver event
 *                             - DRV_EVENT_COMMON_WRITE_COMPLETED
 *                             - DRV_EVENT_COMMON_READ_COMPLETED
 *                             - DRV_EVENT_COMMON_ERROR
 *
 * @return                 None
 *******************************************************************************
 */
extern void drv_psram_isr_callback(OM_OSPI_Type *om_psram, drv_event_t event);
#endif

#ifdef  __cplusplus
}
#endif

#endif  /* (RTE_PSRAM) */
#endif  /* __DRV_PSRAM_H */


/** @} */
