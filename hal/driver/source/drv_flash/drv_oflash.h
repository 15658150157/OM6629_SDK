/* ----------------------------------------------------------------------------
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 * -------------------------------------------------------------------------- */

/**
 * @defgroup FLASH FLASH
 * @ingroup  HAL_Driver
 * @brief    FLASH Driver
 * @details  FLASH Driver
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __DRV_OFLASH_H
#define __DRV_OFLASH_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_driver.h"
#if (RTE_FLASH1)
#include <stdint.h>
#include "om_driver.h"
#include "drv_flash_common.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * MACROS
 */
#define drv_oflash_node_setup(flash, node, cfg)  drv_ospi_node_setup(flash, node, cfg)


/*******************************************************************************
 * TYPEDEFS
 */
typedef ospi_list_node_t     flash_list_node_t;
typedef ospi_list_node_cfg_t flash_list_node_cfg_t;


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief Initialize external FLASH controller with specified configuration struct
 *
 * @param om_flash  The FLASH controller device address
 * @param config    The configuration struct pointer, see@flash_config_t
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_init(OM_OSPI_Type *om_flash, const flash_config_t *config);

/**
 *******************************************************************************
 * @brief external FLASH read id
 * @param om_flash  The external FLASH controller device address
 * @param id        The ID struct pointer, see@flash_id_t
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_read_id(OM_OSPI_Type *om_flash, flash_id_t *id);

/**
 *******************************************************************************
 * @brief Get external FLASH id
 *
 * @param om_flash  The FLASH controller device address
 * @param id        The ID struct pointer, see@flash_id_t
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_id_get(OM_OSPI_Type *om_flash, flash_id_t *id);

/**
 *******************************************************************************
 * @brief Set new command for reading
 *
 * @param om_flash  The external FLASH controller device address
 * @param read_cmd  The new read command, see@flash_read_t
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_read_cmd_set(OM_OSPI_Type *om_flash, flash_read_t read_cmd);

/**
 *******************************************************************************
 * @brief Set new command for writing
 *
 * @param om_flash  The external FLASH controller device address
 * @param read_cmd  The new write command, see@flash_write_t
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_write_cmd_set(OM_OSPI_Type *om_flash, flash_write_t write_cmd);

/**
 *******************************************************************************
 * @brief external FLASH read, wait for operation done
 *
 * @param om_flash  The external FLASH controller device address
 * @param addr      The address of FLASH
 * @param data      The reading data buffer pointer in RAM
 * @param data_len  The reading data length
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_read(OM_OSPI_Type *om_flash, uint32_t addr, uint8_t *data, uint32_t data_len);

/**
 *******************************************************************************
 * @brief external FLASH read with interrupt enabled
 *
 * @param om_flash  The external FLASH controller device address
 * @param addr      The address of FLASH
 * @param data      The reading data buffer pointer in RAM
 * @param data_len  The reading data length
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_read_int(OM_OSPI_Type *om_flash, uint32_t addr, uint8_t *data, uint32_t data_len);

/**
 *******************************************************************************
 * @brief external FLASH write, wait for operation done
 *
 * @param om_flash  The external FLASH controller device address
 * @param addr      The address of FLASH
 * @param data      The writing data buffer pointer in RAM
 *                  Attention: the type must be volatile to avoid compiler optimization
 *                              when Link Time Optimization is enabled,
 *                              if the data is from heap, it should be sure to be volatile,
 *                              for example: volatile uint8_t *data = om_mem_malloc(X, X);
 * @param data_len  The writing data length
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_write(OM_OSPI_Type *om_flash,
                                   uint32_t addr,
                                   volatile uint8_t *data,
                                   uint32_t data_len);

/**
 *******************************************************************************
 * @brief external FLASH write with interrupt enabled
 *          1. drv_oflash_write_int_start
 *          2. drv_oflash_write_int_status_get
 *          3. if is_wip is 1, go to step 2, else go to step 4
 *          4. drv_oflash_write_int_continue
 *          5. go to step 2
 *          6. loop step 2 - 5,
 *             until interrupt callback DRV_EVENT_COMMON_WRITE_COMPLETED event is triggered
 *
 * @param om_flash  The external FLASH controller device address
 * @param addr      The address of FLASH
 * @param data      The writing data buffer pointer in RAM
 *                  Attention: the type must be volatile to avoid compiler optimization
 *                              when Link Time Optimization is enabled,
 *                              if the data is from heap, it should be sure to be volatile
 * @param data_len  The writing data length
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_write_int_start(OM_OSPI_Type *om_flash,
                                             uint32_t addr,
                                             volatile uint8_t *data,
                                             uint32_t data_len);

/**
 *******************************************************************************
 * @brief FLASH is write in progress, it should be called after drv_flash_write_int_start
 *
 * @param om_flash  The FLASH controller device address
 * @param is_wip    The Flash is in write progress status or not, 1 means yes, 0 means no
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_write_int_status_get(OM_OSPI_Type *om_flash, uint8_t *is_wip);

/**
 *******************************************************************************
 * @brief FLASH write with interrupt continue, it should be called when flash wip cleared(value:0),
 *          the wip statue is queried by drv_oflash_write_int_status_get, and it will finished when the
 *          write interrupt is occured.
 *
 * @param om_flash  The external FLASH controller device address
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_write_int_continue(OM_OSPI_Type *om_flash);

/**
 *******************************************************************************
 * @brief external FLASH erase
 *
 * @param om_flash      The external FLASH controller device address
 * @param addr          The address of FLASH
 * @param erase_type    The erase size, see@flash_erase_t
 *
 * @return              Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_erase(OM_OSPI_Type *om_flash, uint32_t addr, flash_erase_t erase_type);

/**
 *******************************************************************************
 * @brief external FLASH read status 1 register
 *
 * @param om_flash  The external FLASH controller device address
 * @param status    Status 1 register read buffer pointer
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_read_status_reg1(OM_OSPI_Type *om_flash, uint8_t *status);

/**
 *******************************************************************************
 * @brief external FLASH read status 2 register
 *
 * @param om_flash  The external FLASH controller device address
 * @param status    Status 2 register read buffer pointer
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_read_status_reg2(OM_OSPI_Type *om_flash, uint8_t *status);

/**
 *******************************************************************************
 * @brief external FLASH read config register
 *
 * @param om_flash  The external FLASH controller device address
 * @param config    Config register read buffer pointer
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_read_config_reg(OM_OSPI_Type *om_flash, uint8_t *config);

/**
 *******************************************************************************
 * @brief external FLASH write status 1&2 register, check ID for writing separately or together
 *
 * @param om_flash  The external FLASH controller device address
 * @param status    Status register read buffer pointer
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_write_status(OM_OSPI_Type *om_flash, uint8_t status[2]);

/**
 *******************************************************************************
 * @brief external FLASH modify status 1&2 register bits
 *
 * @param om_flash  The external FLASH controller device address
 * @param status    Status register read buffer pointer
 * @param mask      Status register mask
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_modifiy_status_bits(OM_OSPI_Type *om_flash, uint8_t status[2], uint8_t mask[2]);

/**
 *******************************************************************************
 * @brief external FLASH write status 1 register
 *
 * @param om_flash  The external FLASH controller device address
 * @param status    Status register read buffer pointer
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_write_status_reg1(OM_OSPI_Type *om_flash, uint8_t *status);

/**
 *******************************************************************************
 * @brief external FLASH write status 2 register
 *
 * @param om_flash  The external FLASH controller device address
 * @param status    Status register read buffer pointer
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_write_status_reg2(OM_OSPI_Type *om_flash, uint8_t *status);

/**
 *******************************************************************************
 * @brief external FLASH write config register
 *
 * @param om_flash  The external FLASH controller device address
 * @param config    Config register read buffer pointer
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_write_config_reg(OM_OSPI_Type *om_flash, uint8_t *config);

/**
 *******************************************************************************
 * @brief external FLASH set write protect region, the protected region cannot be erased or written
 *
 *      Attention:
 *      This function support the following flash:
 *      GD25WQ16E, GD25WQ80E, GD25WQ40E,
 *      P25Q40SU, P25Q80SU, P25Q16SU,
 *      GT25Q40D, GT25Q80A
 *      Other flash without testing,
 *      if not supported, please use drv_oflash_modifiy_status_bits to set the status register
 *
 * @param om_flash  The external FLASH controller device address
 * @param protect   Write protect region, see@flash_protect_t
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
*/
extern om_error_t drv_oflash_write_protect_set(OM_OSPI_Type *om_flash, flash_protect_t protect);

/**
 *******************************************************************************
 * @brief external FLASH enable WP and HOLD pin as data pin for quad read and write
 *
 * @param om_flash  The external FLASH controller device address
 * @param enable    Enable or disable
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_quad_enable(OM_OSPI_Type *om_flash, bool enable);

/**
 *******************************************************************************
 * @brief external FLASH set read parameter qpi mode
 *
 * @param om_flash  The external FLASH controller device address
 * @param param     Read parameter
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_read_param_set(OM_OSPI_Type *om_flash, uint8_t param);

/**
 *******************************************************************************
 * @brief external FLASH enable 4-Byte address mode
 *
 * @param om_flash  The external FLASH controller device address
 * @param enable    Enable or disable
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_4byte_addr_enable(OM_OSPI_Type *om_flash, uint8_t enable);

/**
 *******************************************************************************
 * @brief external FLASH reset
 *
 * @param om_flash  The external FLASH controller device address
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_reset(OM_OSPI_Type *om_flash);

/**
 *******************************************************************************
 * @brief external FLASH encrypt enable
 *
 * @param om_flash  The external FLASH controller device address
 * @param enable    Enable or disable
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_encrypt_enable(OM_OSPI_Type *om_flash, uint8_t enable);

/**
 *******************************************************************************
 * @brief external FLASH list start
 *
 * @param om_flash  The external FLASH controller device address
 * @param list_head list head node pointer
 * @return          Error code, see@om_error_t
 *******************************************************************************
 */
extern om_error_t drv_oflash_list_start(OM_OSPI_Type *om_flash, flash_list_node_t *list_head);

#if (RTE_FLASH1_REGISTER_CALLBACK)
/**
 *******************************************************************************
 * @brief external FLASH controller register callback
 *
 * @param om_flash  The external FLASH controller device address
 * @param isr_cb    Callback
 *
 * @return          Error code, see@om_error_t
 *******************************************************************************
 **/
extern om_error_t drv_oflash_register_isr_callback(OM_OSPI_Type *om_flash, drv_isr_callback_t isr_cb);

/**
 *******************************************************************************
 * @brief The interrupt callback for external FLASH controller driver. It is a weak function. User should define
 *        their own callback in user file, other than modify it in the FLASH driver.
 *
 * @param om_flash  The external FLASH controller device address
 * @param event     The driver event
 *                  - DRV_EVENT_COMMON_WRITE_COMPLETED
 *                  - DRV_EVENT_COMMON_READ_COMPLETED
 *                  - DRV_EVENT_COMMON_ERROR
 *
 * @return          None
 *******************************************************************************
 */
extern void drv_oflash_isr_callback(OM_OSPI_Type *om_flash, drv_event_t event);
#endif /* RTE_FLASH1_REGISTER_CALLBACK */

#ifdef __cplusplus
}
#endif

#endif  /* (RTE_FLASH1) */
#endif  /* __DRV_OFLASH_H */


/** @} */
