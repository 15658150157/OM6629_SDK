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
 * @brief    FLASH driver
 * @details  FLASH driver
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
#if (RTE_FLASH1)
#include "om_device.h"
#include "om_driver.h"
#include <stddef.h>


/*******************************************************************************
 * MACROS
 */
#if RTE_FLASH1_XIP
#define __OF_RAM_CODE   __RAM_CODES("DRV_OFLASH")
#else
#define __OF_RAM_CODE
#endif

#define M_SPI           0 /* Do not modify, must be 0 */
#define M_QPI           1 /* Do not modify, must be 1 */
#define M_3BA           0 /* Do not modify, must be 0 */
#define M_4BA           1 /* Do not modify, must be 1 */
#define XBUS(xpi)       ((xpi == M_QPI) ? BUS_4BIT : BUS_1BIT)
#define FCFG            OSPI_FRAME_CONFIG


#define CMD_FRAME_SET(pframe, cfg)                                  \
    do {                                                            \
        *(flash_frame_t *)(pframe) = (flash_frame_t)cfg;            \
    } while (0)

/*
 * This is a list of commands used in flash operations.
 * parameter1 is used for SPI mode or QPI mode.
 * parameter2 is used for 3 bytes address or 4 bytes address.
 * parameter3 is used for dummy cycles, only for QPI read commands.
 * Parameters instruction:
 *   NA: Not applicable, it is not effective.
 *   xpi: if this parameter is M_QPI, it means the command is used in QPI mode,
 *       and if this parameter is M_SPI, it means the command is used in SPI mode,
 *       do not use any other value.
 *   xba: the address type of command, if this parameter is M_3BA, it means the address is 3 bytes,
 *       if this parameter is M_4BA, it means the address is 4 bytes.
 *   xdmy: the dummy cycles of command, only for QPI read commands.
 */
// Read cmd for SPI mode
#define FLASH_READ_CFG(xba)                 {0x03U, FCFG(BUS_1BIT, 8, BUS_1BIT, (24 + 8 * xba), 0, BUS_1BIT)}
#define FLASH_FAST_READ_CFG(xba)            {0x0BU, FCFG(BUS_1BIT, 8, BUS_1BIT, (24 + 8 * xba), 8, BUS_1BIT)}
#define FLASH_FAST_READ_DO_CFG(xba)         {0x3BU, FCFG(BUS_1BIT, 8, BUS_1BIT, (24 + 8 * xba), 8, BUS_2BIT)}
#define FLASH_FAST_READ_DIO_CFG(xba)        {0xBBU, FCFG(BUS_1BIT, 8, BUS_2BIT, (32 + 8 * xba), 0, BUS_2BIT)}
#define FLASH_FAST_READ_QO_CFG(xba)         {0x6BU, FCFG(BUS_1BIT, 8, BUS_1BIT, (24 + 8 * xba), 8, BUS_4BIT)}
#define FLASH_FAST_READ_QIO_CFG(xba)        {0xEBU, FCFG(BUS_1BIT, 8, BUS_4BIT, (32 + 8 * xba), 4, BUS_4BIT)}
// Read cmd for QPI mode
#define FLASH_FAST_READ_QPI_XDMY_CFG(xba, xdmy)     \
                                            {0x0BU, FCFG(BUS_4BIT, 8, BUS_4BIT, (24 + 8 * xba), xdmy, BUS_4BIT)}
#define FLASH_FAST_READ_QIO_QPI_XDMY_CFG(xba, xdmy) \
                                            {0xEBU, FCFG(BUS_4BIT, 8, BUS_4BIT, (24 + 8 * xba), xdmy, BUS_4BIT)}
// Write cmd for SPI & QPI mode
#define FLASH_PAGE_PROG_CFG(xpi, xba)       {0x02U, FCFG(XBUS(xpi), 8, XBUS(xpi), (24 + 8 * xba), 0, XBUS(xpi))}
// Write cmd for SPI mode only
#define FLASH_PAGE_PROG_QI_CFG(xba)         {0x32U, FCFG(BUS_1BIT, 8, BUS_1BIT, (24 + 8 * xba), 0, BUS_4BIT)}
// Erase
#define FLASH_SEC_ERASE_4K_CFG(xpi, xba)    {0x20U, FCFG(XBUS(xpi), 8, XBUS(xpi), (24 + 8 * xba), 0, XBUS(xpi))}
#define FLASH_BLK_ERASE_32K_CFG(xpi, xba)   {0x52U, FCFG(XBUS(xpi), 8, XBUS(xpi), (24 + 8 * xba), 0, XBUS(xpi))}
#define FLASH_BLK_ERASE_64K_CFG(xpi, xba)   {0xD8U, FCFG(XBUS(xpi), 8, XBUS(xpi), (24 + 8 * xba), 0, XBUS(xpi))}
#define FLASH_CHIP_ERASE_CFG(xpi, NA)       {0x60U, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}
// Read ID
#define FLASH_READ_ID_CFG(xpi)              {0x9FU, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}
// Read UID
#define FLASH_READ_UID_CFG(xpi, xba)        {0x4BU, FCFG(XBUS(xpi), 8, XBUS(xpi), (24 + 8 * xba), 8, XBUS(xpi))}
// Read & Write Reg
#define FLASH_READ_STA_REG1_CFG(xpi)        {0x05U, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}
#define FLASH_WRITE_STA_REG1_CFG(xpi)       {0x01U, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}
#define FLASH_READ_STA_REG2_CFG(xpi)        {0x35U, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}
#define FLASH_WRITE_STA_REG2_CFG(xpi)       {0x31U, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}
#define FLASH_READ_CFG_REG_CFG(xpi)         {0x15U, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}
#define FLASH_WRITE_CFG_REG_CFG(xpi)        {0x11U, FCFG(XBUS(xpi), 8, XBUS(xpi), 8, 0, XBUS(xpi))}
// Write enable/disable
#define FLASH_WRIET_ENABLE_VSR_CFG(xpi)     {0x50U, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}
#define FLASH_WRITE_ENABLE_CFG(xpi)         {0x06U, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}
#define FLASH_WRITE_DISABLE_CFG(xpi)        {0x04U, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}
// Reset
#define FLASH_RESET_ENABLE_CFG(xpi)         {0x66U, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}
#define FLASH_RESET_CFG(xpi)                {0x99U, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}
// Deep power down
#define FLASH_DEEP_PWR_DWN_CFG(xpi)         {0xB9U, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}
#define FLASH_RELEASE_PWR_DWN_CFG(xpi)      {0xABU, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}
// Suspend/Resume
#define FLASH_SUSPEND_CFG(xpi)              {0x75U, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}
#define FLASH_RESUME_CFG(xpi)               {0x7AU, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}
// QPI enable/disable
#define FLASH_QPI_ENABLE_CFG(NA)            {0x38U, FCFG(BUS_1BIT, 8, BUS_1BIT, 0,  0, BUS_1BIT)}
#define FLASH_QPI_DISABLE_CFG(NA)           {0xFFU, FCFG(BUS_4BIT, 8, BUS_4BIT, 0,  0, BUS_4BIT)}
// Set read param
#define FLASH_SET_READ_PARAM_CFG(NA)        {0xC0U, FCFG(BUS_4BIT, 8, BUS_4BIT, 0,  0, BUS_4BIT)}
// 4-Byte address enable/disable
#define FLASH_4BADR_ENABLE_CFG(xpi)         {0xB7U, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}
#define FLASH_4BADR_DISABLE_CFG(xpi)        {0xE9U, FCFG(XBUS(xpi), 8, XBUS(xpi), 0, 0, XBUS(xpi))}


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    drv_isr_callback_t isr_cb;              /*!< Interrupt callback */
    uint32_t addr;                          /*!< Flash address to write */
    uint8_t *data;                          /*!< Data buffer to write */
    uint32_t data_len;                      /*!< Total length of data to write */
    uint32_t data_cnt;                      /*!< Length of data written */
    flash_read_t read_cmd;                  /*!< Flash command used in reading data, see@flash_read_t */
    flash_write_t write_cmd;                /*!< Flash command used in writing data, see@flash_write_t */
    uint8_t xpi_mode;                       /*!< Indicate flash is in QPI or SPI mode */
    uint8_t xba_mode;                       /*!< Indicate flash is in 4Byte or 3Byte address mode */
    flash_trans_state_t write_int_s;        /*!< Write state, used for write interrupt */
    flash_state_t state;                    /*!< Indicate flash is state on reading or writing */
    flash_id_t id;                          /*!< Flash id, see@flash_id_t */
    flash_delay_info_t delay_info;          /*!< Delay information */
} oflash_env_t;


/*******************************************************************************
 * CONST & VARIABLES
 */
// flash models that not supported modify status register 1 and 2 by one command(0x01)
static const flash_id_t no_dual_reg_modify_flash[] = {
    {{0xC8, 0x40, 0x17}},     /* GD25Q64 */
};

static oflash_env_t flash1_env = {
    .isr_cb = NULL,
    .addr = 0,
    .data = NULL,
    .data_len = 0,
    .data_cnt = 0,
    .id.id = 0,
    .state = FLASH_STATE_UNINIT,
};


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
flash_frame_t flash_read_frame_get(flash_read_t read_cmd)
{
    uint8_t xba;
    flash_frame_t frame;

    OM_CRITICAL_BEGIN();
    xba = flash1_env.xba_mode;
    switch(read_cmd) {
        case FLASH_FAST_READ:
            CMD_FRAME_SET(&frame, FLASH_FAST_READ_CFG(xba));
            break;
        case FLASH_FAST_READ_DO:
            CMD_FRAME_SET(&frame, FLASH_FAST_READ_DO_CFG(xba));
            break;
        case FLASH_FAST_READ_DIO:
            CMD_FRAME_SET(&frame, FLASH_FAST_READ_DIO_CFG(xba));
            break;
        case FLASH_FAST_READ_QO:
            CMD_FRAME_SET(&frame, FLASH_FAST_READ_QO_CFG(xba));
            break;
        case FLASH_FAST_READ_QIO:
            CMD_FRAME_SET(&frame, FLASH_FAST_READ_QIO_CFG(xba));
            break;
        case FLASH_FAST_READ_QPI_4_DUMMY:
        case FLASH_FAST_READ_QPI_6_DUMMY:
        case FLASH_FAST_READ_QPI_8_DUMMY:
        case FLASH_FAST_READ_QPI_10_DUMMY:
            CMD_FRAME_SET(&frame, FLASH_FAST_READ_QPI_XDMY_CFG(xba,
                        (4 + (read_cmd - FLASH_FAST_READ_QPI_4_DUMMY) * 2)));
            break;
        case FLASH_FAST_READ_QIO_QPI_4_DUMMY:
        case FLASH_FAST_READ_QIO_QPI_6_DUMMY:
        case FLASH_FAST_READ_QIO_QPI_8_DUMMY:
        case FLASH_FAST_READ_QIO_QPI_10_DUMMY:
            CMD_FRAME_SET(&frame, FLASH_FAST_READ_QIO_QPI_XDMY_CFG(xba,
                        (4 + (read_cmd - FLASH_FAST_READ_QIO_QPI_4_DUMMY) * 2)));
            break;
        case FLASH_READ:
        default:
            CMD_FRAME_SET(&frame, FLASH_READ_CFG(xba));
            break;
    }
    OM_CRITICAL_END();
    return frame;
}

flash_frame_t flash_write_frame_get(flash_write_t write_cmd)
{
    uint8_t xba, xpi;
    flash_frame_t frame;

    OM_CRITICAL_BEGIN();
    xba = flash1_env.xba_mode;
    xpi = flash1_env.xpi_mode;
    switch (write_cmd) {
        case FLASH_PAGE_PROGRAM_QI:
            CMD_FRAME_SET(&frame, FLASH_PAGE_PROG_QI_CFG(xba)); // only spi mode
            break;
        case FLASH_PAGE_PROGRAM:
        case FLASH_PAGE_PROGRAM_QPI:
        default:
            CMD_FRAME_SET(&frame, FLASH_PAGE_PROG_CFG(xpi, xba));
            break;
    }
    OM_CRITICAL_END();
    return frame;
}

__OF_RAM_CODE static om_error_t oflash_read_reg(flash_frame_t *frame, uint8_t *data, uint8_t data_len)
{
    om_error_t error;
    uint32_t cmd[2];
    uint32_t read_cfg[2];
    uint32_t sec_cfg;

    cmd[0] = frame->cmd << 24U;
    cmd[1] = 0;
    OM_CRITICAL_BEGIN();
    // Store frame config
    drv_ospi_sec_cfg_get(OM_OSPI1, &sec_cfg);
    drv_ospi_read_cfg_get(OM_OSPI1, read_cfg);
    // Set read reg frame config
    drv_ospi_sec_cfg_set(OM_OSPI1, 0);
    drv_ospi_read_cfg_set(OM_OSPI1, frame->frame_cfg);
    // Start read
    error = drv_ospi_read(OM_OSPI1, cmd, data, data_len, FLASH_TIMEOUT_MS_DEFAULT);
    // Restore frame config
    drv_ospi_sec_cfg_set(OM_OSPI1, sec_cfg);
    drv_ospi_read_cfg_set(OM_OSPI1, read_cfg);
    OM_CRITICAL_END();

    return error;
}

__OF_RAM_CODE static om_error_t oflash_write_enable(OM_OSPI_Type *om_flash)
{
    flash_frame_t frame;

    CMD_FRAME_SET(&frame, FLASH_WRITE_ENABLE_CFG(flash1_env.xpi_mode));
    return oflash_read_reg(&frame, NULL, 0);
}

__OF_RAM_CODE static om_error_t oflash_write_enable_vsr(OM_OSPI_Type *om_flash)
{
    flash_frame_t frame;

    CMD_FRAME_SET(&frame, FLASH_WRIET_ENABLE_VSR_CFG(flash1_env.xpi_mode));
    return oflash_read_reg(&frame, NULL, 0);
}

__OF_RAM_CODE static om_error_t oflash_poll_wip(flash_frame_t *wip_frame, uint32_t timeout_ms)
{
    uint8_t status = 0xFF;
    om_error_t error;
    uint32_t start_cycle;

    start_cycle = drv_dwt_get_cycle();
    do {
        error = oflash_read_reg(wip_frame, &status, 1);
        if (drv_dwt_get_cycle() - start_cycle > DRV_DWT_MS_2_CYCLES_CEIL(timeout_ms)) {
            return OM_ERROR_TIMEOUT;
        }
    } while ((error != OM_ERROR_OK) || (status & FLASH_STATUS_1_WIP_MASK));
    return error;
}

__OF_RAM_CODE static om_error_t oflash_write_reg(flash_frame_t *write_reg_frame,
                                                 flash_frame_t *wip_frame,
                                                 uint8_t *data,
                                                 uint8_t data_len,
                                                 flash_reg_write_en_t wr_en_type)
{
    om_error_t error;
    oflash_env_t *env = &flash1_env;
    uint32_t cmd[2];
    uint32_t read_cfg[2], frame_cfg[2];
    uint32_t sec_cfg;

    if (data_len >= 4) {
        return OM_ERROR_PARAMETER;
    }
    cmd[0] = write_reg_frame->cmd << 24U;
    // the data[0] is transfered first
    cmd[1] = 0;
    for (uint8_t i = 0; i < data_len; i++) {
        cmd[1] |= (data[i] << (8 * ((env->xba_mode ? 3 : 2) - i)));
    }
    OM_CRITICAL_BEGIN();
    // Store frame config
    drv_ospi_sec_cfg_get(OM_OSPI1, &sec_cfg);
    drv_ospi_read_cfg_get(OM_OSPI1, read_cfg);
    // Set read reg frame config, the data length should switch to bits
    frame_cfg[0] = write_reg_frame->frame_cfg[0] | ((data_len << 3) << OSPI_SW_CFG0_P1_BIT_CNT_POS);
    frame_cfg[1] = write_reg_frame->frame_cfg[1];
    drv_ospi_sec_cfg_set(OM_OSPI1, 0);
    drv_ospi_read_cfg_set(OM_OSPI1, frame_cfg);
    // Set write enable
    if (wr_en_type == SR_WRITE_EN_PERMANENT) {
        oflash_write_enable(OM_OSPI1);
    } else if (wr_en_type == SR_WRITE_EN_VOLATILE) {
        oflash_write_enable_vsr(OM_OSPI1);
    }
    // Start Read
    error = drv_ospi_read(OM_OSPI1, cmd, NULL, 0, FLASH_TIMEOUT_MS_DEFAULT);
    // Restore frame config
    drv_ospi_sec_cfg_set(OM_OSPI1, sec_cfg);
    drv_ospi_read_cfg_set(OM_OSPI1, read_cfg);
    OM_CRITICAL_END();
    // wait done
    return error == OM_ERROR_OK ? oflash_poll_wip(wip_frame, FLASH_TIMEOUT_MS_DEFAULT) : error;
}

__OF_RAM_CODE static om_error_t oflash_read_frame_set(OM_OSPI_Type *om_flash,
                                                      flash_frame_t *qpi_frame,
                                                      flash_frame_t *read_frame)
{
    om_error_t error = OM_ERROR_OK;

    // if qpi frame is not null, set qpi mode
    if (qpi_frame->cmd) {
        if ((error = oflash_read_reg(qpi_frame, NULL, 0)) != OM_ERROR_OK) {
            return error;
        }
    }
    // Update read frame config
    drv_ospi_read_frame_set(om_flash, (const ospi_frame_t *)read_frame);
    return error;
}

#if (RTE_FLASH1_XIP)
__OF_RAM_CODE static om_error_t oflash_suspend(flash_frame_t *suspend_frame)
{
    om_error_t error;

    if ((error = oflash_read_reg(suspend_frame, NULL, 0)) != OM_ERROR_OK) {
        return error;
    }
    // CS# High To Next Command After Suspend
    switch (flash1_env.id.man_id) {
        case FLASH_MID_PUYA:        // (0x856014) tPSL = 30us
        case FLASH_MID_GIGADEVICE:  // (0xc86514) tSUS = 40us
            DRV_DELAY_US(50);
            break;
        default:
            DRV_DELAY_US(50);
            break;
    }
    return error;
}

__OF_RAM_CODE static om_error_t oflash_resume(flash_frame_t *resume_frame)
{
    om_error_t error;

    if ((error = oflash_read_reg(resume_frame, NULL, 0)) != OM_ERROR_OK) {
        return error;
    }
    // Latency between Program/Erase Resume and next Suspend
    switch (flash1_env.id.man_id) {
        case FLASH_MID_PUYA:        // (0x856014) tPRS = 20us
            DRV_DELAY_US(20 * 2);
            break;
        case FLASH_MID_GIGADEVICE:  // (0xc86514) tRS = 100us
            DRV_DELAY_US(110);
            break;
        default:
            DRV_DELAY_US(110);
            break;
    }
    return error;
}

__OF_RAM_CODE static om_error_t oflash_poll_wip_with_suspend(flash_frame_t *wip_frame,
                                                             flash_frame_t *suspend_frame,
                                                             flash_frame_t *resume_frame,
                                                             uint32_t irq_save,
                                                             uint32_t timeout_ms)
{
    om_error_t error, ret;
    uint8_t status = 0;
    uint32_t start_cycle;

    start_cycle = drv_dwt_get_cycle();
    while ((error = oflash_read_reg(wip_frame, &status, 1)) == OM_ERROR_OK &&
           status & FLASH_STATUS_1_WIP_MASK) {
        // Delay 100us
        DRV_WAIT_US_UNTIL_TO(!(drv_irq_is_any_ext_pending() && !irq_save), 100, ret);
        // is pending
        if (ret == OM_ERROR_OK) {
            OM_ASSERT(drv_irq_is_any_ext_pending());
            // suspend
            oflash_suspend(suspend_frame);
            // like call __enable_irq()
            OM_CRITICAL_END_EX(irq_save);
            // ENTER irq

            // like call __disable_irq()
            OM_CRITICAL_BEGIN_EX(irq_save);
            // resume
            oflash_resume(resume_frame);
        }
        if (drv_dwt_get_cycle() - start_cycle > DRV_DWT_MS_2_CYCLES_CEIL(timeout_ms)) {
            error = OM_ERROR_TIMEOUT;
            break;
        }
    }
    return error;
}

__OF_RAM_CODE static om_error_t oflash_write_with_suspend(OM_OSPI_Type *om_flash,
                                                          uint32_t addr,
                                                          uint8_t *data,
                                                          uint32_t data_len,
                                                          flash_frame_t *write_frame,
                                                          flash_frame_t *wip_frame,
                                                          flash_frame_t *suspend_frame,
                                                          flash_frame_t *resume_frame,
                                                          uint32_t timeout_ms)
{
    uint32_t cmd[2];
    om_error_t error;
    uint32_t irq_save;

    cmd[0] = write_frame->cmd << 24U;
    cmd[1] = addr;

    OM_CRITICAL_BEGIN_EX(irq_save);
    // Start Write
    error = drv_ospi_write(om_flash, cmd, data, data_len,
                timeout_ms < FLASH_TIMEOUT_MS_DEFAULT ? timeout_ms : FLASH_TIMEOUT_MS_DEFAULT);
    if (error != OM_ERROR_OK) {
        OM_CRITICAL_END_EX(irq_save);
        return error;
    }
    error = oflash_poll_wip_with_suspend(wip_frame, suspend_frame, resume_frame, irq_save,
                timeout_ms < FLASH_TIMEOUT_MS_DEFAULT ? timeout_ms : FLASH_TIMEOUT_MS_DEFAULT);
    OM_CRITICAL_END_EX(irq_save);
    return error;
}

__OF_RAM_CODE static om_error_t oflash_erase_with_suspend(OM_OSPI_Type *om_flash,
                                                          uint32_t addr,
                                                          flash_frame_t *erase_frame,
                                                          flash_frame_t *wip_frame,
                                                          flash_frame_t *suspend_frame,
                                                          flash_frame_t *resume_frame,
                                                          uint32_t timeout_ms)
{
    om_error_t error;
    uint32_t cmd[2];
    uint32_t read_cfg[2];
    uint32_t sec_cfg;
    uint32_t irq_save;

    cmd[0] = erase_frame->cmd << 24U;
    cmd[1] = addr;

    OM_CRITICAL_BEGIN_EX(irq_save);
    // Store frame config
    drv_ospi_sec_cfg_get(om_flash, &sec_cfg);
    drv_ospi_read_cfg_get(om_flash, read_cfg);
    // Set read reg frame config
    drv_ospi_sec_cfg_set(om_flash, 0);
    drv_ospi_read_cfg_set(om_flash, erase_frame->frame_cfg);
    // Start read
    error = drv_ospi_read(om_flash, cmd, NULL, 0,
                timeout_ms < FLASH_TIMEOUT_MS_DEFAULT ? timeout_ms : FLASH_TIMEOUT_MS_DEFAULT);
    // Restore frame config
    drv_ospi_sec_cfg_set(om_flash, sec_cfg);
    drv_ospi_read_cfg_set(om_flash, read_cfg);

    error = oflash_poll_wip_with_suspend(wip_frame, suspend_frame, resume_frame, irq_save,
                timeout_ms < FLASH_TIMEOUT_MS_DEFAULT ? timeout_ms : FLASH_TIMEOUT_MS_DEFAULT);
    OM_CRITICAL_END_EX(irq_save);
    return error;
}

#else

__OF_RAM_CODE static om_error_t oflash_write(OM_OSPI_Type *om_flash,
                                             uint32_t addr,
                                             uint8_t *data,
                                             uint32_t data_len,
                                             flash_frame_t *write_frame,
                                             flash_frame_t *wip_frame,
                                             uint32_t timeout_ms)
{
    uint32_t cmd[2];
    om_error_t error;

    cmd[0] = write_frame->cmd << 24U;
    cmd[1] = addr;
    // Start Write
    error = drv_ospi_write(om_flash, cmd, data, data_len,
                timeout_ms < FLASH_TIMEOUT_MS_DEFAULT ? timeout_ms : FLASH_TIMEOUT_MS_DEFAULT);
    if (error != OM_ERROR_OK) {
        return error;
    }
    return oflash_poll_wip(wip_frame, timeout_ms);
}

__OF_RAM_CODE static om_error_t oflash_erase(OM_OSPI_Type *om_flash,
                                             uint32_t addr,
                                             flash_frame_t *erase_frame,
                                             flash_frame_t *wip_frame,
                                             uint32_t timeout_ms)
{
    om_error_t error;
    uint32_t cmd[2];
    uint32_t read_cfg[2];
    uint32_t sec_cfg;

    cmd[0] = erase_frame->cmd << 24U;
    cmd[1] = addr;

    // Store frame config
    drv_ospi_sec_cfg_get(om_flash, &sec_cfg);
    drv_ospi_read_cfg_get(om_flash, read_cfg);
    // Set read reg frame config
    drv_ospi_sec_cfg_set(om_flash, 0);
    drv_ospi_read_cfg_set(om_flash, erase_frame->frame_cfg);
    // Start read
    error = drv_ospi_read(om_flash, cmd, NULL, 0,
                timeout_ms < FLASH_TIMEOUT_MS_DEFAULT ? timeout_ms : FLASH_TIMEOUT_MS_DEFAULT);
    // Restore frame config
    drv_ospi_sec_cfg_set(om_flash, sec_cfg);
    drv_ospi_read_cfg_set(om_flash, read_cfg);

    if (error != OM_ERROR_OK) {
        return error;
    }
    return oflash_poll_wip(wip_frame, timeout_ms);
}
#endif /* RTE_FLASH1_XIP */

__OF_RAM_CODE static om_error_t oflash_read_id(flash_frame_t *id_frame, flash_id_t *id)
{
    om_error_t error;
    flash_id_t id_read = {.id = 0};

    if ((error = oflash_read_reg(id_frame, (uint8_t*)&id_read, 3)) != OM_ERROR_OK) {
        return error;
    }
    if ((id_read.id & 0x00FFFFFF) == 0x00FFFFFF || id_read.id == 0x0) {
        return OM_ERROR_FAIL;
    }
    *id = id_read;
    return OM_ERROR_OK;
}

__OF_RAM_CODE static om_error_t oflash_auto_delay_init(flash_frame_t *id_frame,
                                                       ospi_config_t *config)
{
    flash_id_t id1, id2;
    int32_t delayi, delay1 = -1, delay2 = FLASH_DELAY_MAX;
    ospi_config_t ospicfg = *config;
    uint8_t retry_cnt = 0;
    om_error_t error = OM_ERROR_FAIL;

    if (drv_rcc_clock_get(RCC_CLK_OSPI1) == 0) {
        DRV_RCC_CLOCK_ENABLE(RCC_CLK_OSPI1, 1);
    }
    OM_CRITICAL_BEGIN();
    // set flash low frequency so it can work
    ospicfg.clk_div = drv_rcc_clock_get(RCC_CLK_OSPI1) / FLASH_FREQ_HZ_DEFAULT;
    ospicfg.sample_cfg.sdr_async.sdr_async_dly = FLASH_DELAY_DEFAULT;
    drv_ospi_init(OM_OSPI1, &ospicfg);
    // read id twice, save the true id to id1
    while (1) {
        if ((oflash_read_id(id_frame, &id1) == OM_ERROR_OK) && (oflash_read_id(id_frame, &id2) == OM_ERROR_OK) && (id1.id == id2.id)) {
            break;
        }

        retry_cnt++;
        if (retry_cnt >= FLASH_AUTO_DLY_RETYR_CNT) {
            goto EXIT;
        }
    };

    // poll, delayi from 0 to DRV_SF_DELAY_MAX
    for (delayi = 0; delayi <= FLASH_DELAY_MAX; ++delayi) {
        ospicfg.clk_div = config->clk_div;
        ospicfg.sample_cfg.sdr_async.sdr_async_dly = delayi;
        drv_ospi_init(OM_OSPI1, &ospicfg);
        if ((oflash_read_id(id_frame, &id2) == OM_ERROR_OK) && (id1.id == id2.id)) {
            if (delay1 == -1) {
                delay1 = delayi;
            }
            delay2 = delayi;
        } else {
            if (delay1 != -1) {
                break;
            }
        }
    }
    if (delay1 == -1) {
        // faild, then set flash low frequency so it can work
        ospicfg.clk_div = drv_rcc_clock_get(RCC_CLK_OSPI1) / FLASH_FREQ_HZ_DEFAULT;
        ospicfg.sample_cfg.sdr_async.sdr_async_dly = FLASH_DELAY_DEFAULT;
    } else {
        ospicfg.sample_cfg.sdr_async.sdr_async_dly = (delay1 + delay2) / 2;
        error = OM_ERROR_OK;
    }
    drv_ospi_init(OM_OSPI1, &ospicfg);
    OM_CRITICAL_END();

EXIT:
    flash1_env.delay_info.auto_delay = ospicfg.sample_cfg.sdr_async.sdr_async_dly;
    flash1_env.delay_info.valid_delay_max = delay2;
    flash1_env.delay_info.valid_delay_min = delay1;
    flash1_env.delay_info.auto_delay_en = 1;
    return error;
}

static om_error_t oflash_write_status(OM_OSPI_Type *om_flash, uint8_t status[2], uint8_t is_volatile)
{
    flash_frame_t reg_frame, wip_frame;
    oflash_env_t *env = &flash1_env;
    om_error_t error;

    CMD_FRAME_SET(&reg_frame, FLASH_WRITE_STA_REG1_CFG(env->xpi_mode));
    CMD_FRAME_SET(&wip_frame, FLASH_READ_STA_REG1_CFG(env->xpi_mode));
    #if (RTE_FLASH1_XIP)
    OM_CRITICAL_BEGIN();
    #endif
    error = oflash_write_reg(&reg_frame, &wip_frame, status, 2,
                is_volatile ? SR_WRITE_EN_VOLATILE : SR_WRITE_EN_PERMANENT);
    #if (RTE_FLASH1_XIP)
    OM_CRITICAL_END();
    #endif
    return error;
}

static om_error_t oflash_write_status_reg1(OM_OSPI_Type *om_flash, uint8_t *status, uint8_t is_volatile)
{
    flash_frame_t reg_frame, wip_frame;

    CMD_FRAME_SET(&reg_frame, FLASH_WRITE_STA_REG1_CFG(flash1_env.xpi_mode));
    CMD_FRAME_SET(&wip_frame, FLASH_READ_STA_REG1_CFG(flash1_env.xpi_mode));

    #if (RTE_FLASH1_XIP)
    OM_CRITICAL_BEGIN();
    #endif
    if (oflash_write_reg(&reg_frame, &wip_frame, status, 1,
            is_volatile ? SR_WRITE_EN_VOLATILE : SR_WRITE_EN_PERMANENT) != OM_ERROR_OK) {
        return OM_ERROR_FAIL;
    }
    #if (RTE_FLASH1_XIP)
    OM_CRITICAL_END();
    #endif
    return OM_ERROR_OK;
}


static om_error_t oflash_write_status_reg2(OM_OSPI_Type *om_flash, uint8_t *status, uint8_t is_volatile)
{
    flash_frame_t reg_frame, wip_frame;
    uint8_t rd_status;

    CMD_FRAME_SET(&reg_frame, FLASH_WRITE_STA_REG2_CFG(flash1_env.xpi_mode));
    CMD_FRAME_SET(&wip_frame, FLASH_READ_STA_REG1_CFG(flash1_env.xpi_mode));

    #if (RTE_FLASH1_XIP)
    OM_CRITICAL_BEGIN();
    #endif
    if (oflash_write_reg(&reg_frame, &wip_frame, status, 1,
            is_volatile ? SR_WRITE_EN_VOLATILE : SR_WRITE_EN_PERMANENT) != OM_ERROR_OK) {
        return OM_ERROR_FAIL;
    }
    #if (RTE_FLASH1_XIP)
    OM_CRITICAL_END();
    #endif
    // read back to verify
    if (drv_oflash_read_status_reg2(om_flash, &rd_status) != OM_ERROR_OK) {
        return OM_ERROR_FAIL;
    }
    if (rd_status != *status) {
        return OM_ERROR_VERIFY;
    }
    return OM_ERROR_OK;
}

static om_error_t oflash_write_config_reg(OM_OSPI_Type *om_flash, uint8_t *config, uint8_t is_volatile)
{
    flash_frame_t reg_frame, wip_frame;
    CMD_FRAME_SET(&reg_frame, FLASH_WRITE_CFG_REG_CFG(flash1_env.xpi_mode));
    CMD_FRAME_SET(&wip_frame, FLASH_READ_STA_REG1_CFG(flash1_env.xpi_mode));

    #if (RTE_FLASH1_XIP)
    OM_CRITICAL_BEGIN();
    #endif
    if (oflash_write_reg(&reg_frame, &wip_frame, config, 1,
            is_volatile ? SR_WRITE_EN_VOLATILE : SR_WRITE_EN_PERMANENT) != OM_ERROR_OK) {
        return OM_ERROR_FAIL;
    }
    #if (RTE_FLASH1_XIP)
    OM_CRITICAL_END();
    #endif
    return OM_ERROR_OK;
}

static om_error_t oflash_modifiy_status_bits(OM_OSPI_Type *om_flash, uint8_t status[2], uint8_t mask[2], uint8_t is_volatile)
{
    uint8_t s1[2];
    uint8_t s2[2];
    uint8_t need_write[2] = {0, 0};
    uint8_t modify_by_one_cmd = 1;
    om_error_t error = OM_ERROR_OK;

    // Read status reg1 and modify
    if ((error = drv_oflash_read_status_reg1(om_flash, &s1[0]) ) != OM_ERROR_OK) {
        goto EXIT;
    }
    if ((error = drv_oflash_read_status_reg1(om_flash, &s2[0])) != OM_ERROR_OK) {
        goto EXIT;
    }
    if (s1[0] != s2[0]) {
        return OM_ERROR_VERIFY;
    }
    if ((s1[0] & mask[0]) != (status[0] & mask[0])) {
        s1[0] &= ~mask[0];
        s1[0] |= (status[0] & mask[0]);
        need_write[0] = 1;
    }
    // Read status reg2 and modify
    if ((error = drv_oflash_read_status_reg2(om_flash, &s1[1])) != OM_ERROR_OK) {
        goto EXIT;
    }
    if ((error = drv_oflash_read_status_reg2(om_flash, &s2[1])) != OM_ERROR_OK) {
        goto EXIT;
    }
    if (s1[1] != s2[1]) {
        return OM_ERROR_VERIFY;
    }
    if ((s1[1] & mask[1]) != (status[1] & mask[1])) {
        s1[1] &= ~mask[1];
        s1[1] |= (status[1] & mask[1]);
        need_write[1] = 1;
    }
    // check if the current flash support dual register modify by one command
    for (uint16_t i = 0; i < sizeof(no_dual_reg_modify_flash) / sizeof(no_dual_reg_modify_flash[0]); i++) {
        if (flash1_env.id.id == no_dual_reg_modify_flash[i].id) {
            modify_by_one_cmd = 0;
            break;
        }
    }
    if (modify_by_one_cmd) {
        // write status reg 1 and reg 2 by one command
        if (need_write[0] || need_write[1]) {
            error = oflash_write_status(om_flash, s1, is_volatile);
        }
    } else {
        // write status reg 1, then write status reg 2
        if (need_write[0] && ((error = oflash_write_status_reg1(om_flash, &s1[0], is_volatile)) != OM_ERROR_OK)) {
            goto EXIT;
        }
        if (need_write[1] && ((error = oflash_write_status_reg2(om_flash, &s1[1], is_volatile)) != OM_ERROR_OK)) {
            goto EXIT;
        }
    }

EXIT:
    return error;
}


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
om_error_t drv_oflash_init(OM_OSPI_Type *om_flash, const flash_config_t *config)
{
    om_error_t error = OM_ERROR_OK;
    oflash_env_t *env = &flash1_env;
    flash_frame_t read_frame = flash_read_frame_get(config->read_cmd);
    flash_frame_t write_frame = flash_write_frame_get(config->write_cmd);
    flash_config_t flash_cfg;

    if (drv_rcc_clock_get(RCC_CLK_OSPI1) == 0) {
        DRV_RCC_CLOCK_ENABLE(RCC_CLK_OSPI1, 1);
    }
    if (!config) {
        // If config is NULL, set default config, freq is 8MHz, delay is 2, bus width is 2
        flash_cfg.clk_div = drv_rcc_clock_get(RCC_CLK_OSPI1) / FLASH_FREQ_HZ_DEFAULT;
        flash_cfg.delay = FLASH_DELAY_DEFAULT;
        flash_cfg.spi_mode = FLASH_SPI_MODE_0;
        flash_cfg.read_cmd = FLASH_FAST_READ_DO;
        flash_cfg.write_cmd = FLASH_PAGE_PROGRAM;
    } else {
        flash_cfg = *config;
    }
    ospi_config_t ospi_config = {
        .cs_config          = NULL,
        .read_frame_cfg     = {
            read_frame.frame_cfg[0],
            read_frame.frame_cfg[1],
        },
        .write_frame_cfg    = {
            write_frame.frame_cfg[0],
            write_frame.frame_cfg[1],
        },
        .read_opcode        = read_frame.cmd,
        .write_opcode       = write_frame.cmd,
        .mode               = (ospi_mode_t)flash_cfg.spi_mode,
        .page_cross_en      = 0,
        .page_size          = FLASH_PAGE_SIZE,
        .clk_div            = flash_cfg.clk_div,
        .sdr_async_en       = 1,
        .sample_cfg.sdr_async.sdr_async_dly
                            = flash_cfg.delay,
        .rw_data_width      = 0,
        .opcode_bypass_en   = 0,
        .is_normal_protocol = 1,
        .is_4bytes_addr     = 0,
        .encrypt_en         = 0,
        .decrypt_en         = 0,
    };

    // 1. Flash only support SPI mode 0/3.
    // 2. In SPI mode 3, clock division must be at least 2
    if (flash_cfg.spi_mode == FLASH_SPI_MODE_1 ||
        flash_cfg.spi_mode == FLASH_SPI_MODE_2 ||
        (flash_cfg.spi_mode == FLASH_SPI_MODE_3 && flash_cfg.clk_div < 2)) {
            return OM_ERROR_PARAMETER;
    }
    // auto delay
    if (config->delay == FLASH_DELAY_AUTO) {
        flash_frame_t id_frame;
        CMD_FRAME_SET(&id_frame, FLASH_READ_ID_CFG(flash1_env.xpi_mode));
        if(oflash_auto_delay_init(&id_frame, &ospi_config) != OM_ERROR_OK) {
            return OM_ERROR_FAIL;
        }
    } else {
        drv_ospi_init(om_flash, &ospi_config);
    }
    // Store read and write command
    if ((error = drv_oflash_read_cmd_set(om_flash, flash_cfg.read_cmd)) != OM_ERROR_OK) {
        goto INIT_EXIT;
    }
    if ((error = drv_oflash_write_cmd_set(om_flash, flash_cfg.write_cmd)) != OM_ERROR_OK) {
        goto INIT_EXIT;
    }
    // Read FLASH id
    if ((error = drv_oflash_read_id(om_flash, &env->id)) != OM_ERROR_OK) {
        goto INIT_EXIT;
    }
    #if (RTE_FLASH1_REGISTER_CALLBACK)
    drv_ospi_register_isr_callback(om_flash, (drv_isr_callback_t)drv_oflash_isr_callback);
    #endif
    env->state = FLASH_STATE_INIT;
    return OM_ERROR_OK;

INIT_EXIT:
    // set state uninit
    env->state = FLASH_STATE_UNINIT;
    return error;
}

om_error_t drv_oflash_read_uid(OM_OSPI_Type *om_flash, uint8_t *uid, uint32_t len)
{
    oflash_env_t *env = &flash1_env;
    uint32_t cmd[2];
    flash_frame_t frame;
    om_error_t error;

    if (env->state != FLASH_STATE_INIT) {
        return OM_ERROR_BUSY;
    }

    CMD_FRAME_SET(&frame, FLASH_READ_UID_CFG(env->xpi_mode, env->xba_mode));

    cmd[0] = frame.cmd << 24;
    cmd[1] = 0;

    env->state = FLASH_STATE_READING;
    #if (RTE_FLASH1_XIP)
    OM_CRITICAL_BEGIN();
    #endif
    error = drv_ospi_read(om_flash, cmd, uid, len, FLASH_TIMEOUT_MS_DEFAULT);
    #if (RTE_FLASH1_XIP)
    OM_CRITICAL_END();
    #endif
    env->state = FLASH_STATE_INIT;

    return error;
}

om_error_t drv_oflash_id_get(OM_OSPI_Type *om_flash, flash_id_t *id)
{
    if ((flash1_env.id.id & 0x00FFFFFF) == 0x00FFFFFF || flash1_env.id.id == 0x0) {
        return OM_ERROR_FAIL;
    }
    *id = flash1_env.id;
    return OM_ERROR_OK;
}

om_error_t drv_oflash_read_cmd_set(OM_OSPI_Type *om_flash, flash_read_t read_cmd)
{
    oflash_env_t *env = &flash1_env;
    om_error_t error;
    flash_frame_t qpi_frame = {0};
    flash_frame_t read_frame = flash_read_frame_get(read_cmd);
    uint8_t xpi_mode = env->xpi_mode;

    // Update quad mode
    if (read_cmd >= FLASH_FAST_READ_QO) {
        if ((error = drv_oflash_quad_enable(om_flash, 1)) != OM_ERROR_OK) {
            return error;
        }
    }
    // Update qpi frame
    if (read_cmd >= FLASH_FAST_READ_QPI_4_DUMMY && env->xpi_mode != M_QPI) {
        CMD_FRAME_SET(&qpi_frame, FLASH_QPI_ENABLE_CFG(0));
        xpi_mode = M_QPI;
    } else if (read_cmd < FLASH_FAST_READ_QPI_4_DUMMY && env->xpi_mode != M_SPI) {
        CMD_FRAME_SET(&qpi_frame, FLASH_QPI_DISABLE_CFG(0));
        xpi_mode = M_SPI;
    }
    // Set read frame and qpi mode
    OM_CRITICAL_BEGIN();
    error = oflash_read_frame_set(om_flash, &qpi_frame, &read_frame);
    if (error == OM_ERROR_OK) {
        // Update env
        env->read_cmd = read_cmd;
        env->xpi_mode = xpi_mode;
    }
    OM_CRITICAL_END();
    return error;
}

om_error_t drv_oflash_write_cmd_set(OM_OSPI_Type *om_flash, flash_write_t write_cmd)
{
    oflash_env_t *env = &flash1_env;
    om_error_t error = OM_ERROR_OK;
    flash_frame_t write_frame = flash_write_frame_get(write_cmd);

    // Update quad mode
    if (write_cmd >= FLASH_PAGE_PROGRAM_QI) {
        if ((error = drv_oflash_quad_enable(om_flash, 1)) != OM_ERROR_OK) {
            return error;
        }
    }
    // NOTE: QPI/SPI mode should be updated when set read frame

    // Update write frame config
    drv_ospi_write_frame_set(om_flash, (const ospi_frame_t *)&write_frame);
    // Update write frame of env
    env->write_cmd = write_cmd;
    return error;
}

om_error_t drv_oflash_read(OM_OSPI_Type *om_flash, uint32_t addr, uint8_t *data, uint32_t data_len, uint32_t timeout_ms)
{
    oflash_env_t *env = &flash1_env;
    uint32_t cmd[2];
    flash_frame_t read_frame = flash_read_frame_get(env->read_cmd);
    om_error_t error;

    if (env->state != FLASH_STATE_INIT) {
        return OM_ERROR_BUSY;
    }

    cmd[0] = read_frame.cmd << 24;
    cmd[1] = addr;

    env->state = FLASH_STATE_READING;
    #if (RTE_FLASH1_XIP)
    OM_CRITICAL_BEGIN();
    #endif
    error = drv_ospi_read(om_flash, cmd, data, data_len,
                timeout_ms < FLASH_TIMEOUT_MS_DEFAULT ? timeout_ms : FLASH_TIMEOUT_MS_DEFAULT);
    #if (RTE_FLASH1_XIP)
    OM_CRITICAL_END();
    #endif
    env->state = FLASH_STATE_INIT;

    return error;
}

om_error_t drv_oflash_read_int(OM_OSPI_Type *om_flash,
                               uint32_t addr,
                               uint8_t *data,
                               uint32_t data_len)
{
    oflash_env_t *env = &flash1_env;
    uint32_t cmd[2];
    flash_frame_t read_frame = flash_read_frame_get(env->read_cmd);

    // Do not use read/write with interrupt interfaces for internal flash
    #if (RTE_FLASH1_XIP)
    return OM_ERROR_PERMISSION;
    #endif

    if (env->state != FLASH_STATE_INIT) {
        return OM_ERROR_BUSY;
    }
    cmd[0] = read_frame.cmd << 24;
    cmd[1] = addr;
    env->state = FLASH_STATE_READING;
    env->addr = addr;
    env->data = data;
    env->data_len = data_len;
    drv_ospi_read_int(om_flash, cmd, data, data_len);
    return OM_ERROR_OK;
}

om_error_t drv_oflash_write(OM_OSPI_Type *om_flash,
                            uint32_t addr,
                            volatile uint8_t *data,
                            uint32_t data_len,
                            uint32_t timeout_ms)
{
    oflash_env_t *env = &flash1_env;
    uint32_t data_remain = data_len;
    uint32_t addr_write = addr;
    uint8_t* data_write = (uint8_t *)data;
    om_error_t error = OM_ERROR_OK;
    flash_frame_t write_frame = flash_write_frame_get(flash1_env.write_cmd);
    flash_frame_t wip_frame;
    uint32_t start_cycle;

    CMD_FRAME_SET(&wip_frame, FLASH_READ_STA_REG1_CFG(env->xpi_mode));
    if (env->state != FLASH_STATE_INIT) {
        return OM_ERROR_BUSY;
    }
    env->state = FLASH_STATE_WRITING;
    start_cycle = drv_dwt_get_cycle();
    while (data_remain) {
        error = oflash_write_enable(om_flash);
        if (error == OM_ERROR_OK) {
            // Write to the next Page boundary
            uint32_t write_len = FLASH_PAGE_SIZE - (addr_write & (FLASH_PAGE_SIZE - 1));
            write_len = (data_remain >= write_len) ? write_len : data_remain;
            #if (RTE_FLASH1_XIP)
            flash_frame_t suspend_frame, resume_frame;
            CMD_FRAME_SET(&suspend_frame, FLASH_SUSPEND_CFG(env->xpi_mode));
            CMD_FRAME_SET(&resume_frame, FLASH_RESUME_CFG(env->xpi_mode));
            error = oflash_write_with_suspend(om_flash, addr_write, data_write, write_len,
                            &write_frame, &wip_frame, &suspend_frame, &resume_frame,
                            timeout_ms < FLASH_TIMEOUT_MS_DEFAULT ? timeout_ms : FLASH_TIMEOUT_MS_DEFAULT);
            #else
            error = oflash_write(om_flash, addr_write, data_write,
                        write_len, &write_frame, &wip_frame,
                        timeout_ms < FLASH_TIMEOUT_MS_DEFAULT ? timeout_ms : FLASH_TIMEOUT_MS_DEFAULT);
            #endif
            data_remain -= write_len;
            addr_write += write_len;
            data_write += write_len;
            if (drv_dwt_get_cycle() - start_cycle > DRV_DWT_MS_2_CYCLES_CEIL(timeout_ms)) {
                error = OM_ERROR_TIMEOUT;
                break;
            }
        } else {
            break;
        }
    }
    env->state = FLASH_STATE_INIT;
    return error;
}

om_error_t drv_oflash_write_int_start(OM_OSPI_Type *om_flash,
                                      uint32_t addr,
                                      volatile uint8_t *data,
                                      uint32_t data_len)
{
    oflash_env_t *env = &flash1_env;
    om_error_t error = OM_ERROR_OK;
    uint32_t write_len;
    uint32_t cmd[2];
    flash_frame_t write_frame = flash_write_frame_get(env->write_cmd);

    #if (RTE_FLASH1_XIP)
    return OM_ERROR_PERMISSION;
    #endif

    if (env->state != FLASH_STATE_INIT) {
        return OM_ERROR_BUSY;
    }
    env->state = FLASH_STATE_WRITING;
    error = oflash_write_enable(om_flash);
    if (error == OM_ERROR_OK) {
        // Write to the next Page boundary
        write_len = FLASH_PAGE_SIZE - (addr & (FLASH_PAGE_SIZE - 1));
        write_len = (data_len >= write_len) ? write_len : data_len;
        cmd[0] = write_frame.cmd << 24;
        cmd[1] = addr;
        env->write_int_s = FLASH_TRANS_BUSY;
        env->addr = addr + write_len;
        env->data = (uint8_t *)data + write_len;
        env->data_cnt = write_len;
        env->data_len = data_len;
        drv_ospi_write_int(om_flash, cmd, data, write_len);
    }
    return error;
}

om_error_t drv_oflash_write_int_status_get(OM_OSPI_Type *om_flash, uint8_t *is_wip)
{
    flash_frame_t wip_frame;
    uint8_t status;
    om_error_t error;

    // Waiting for the command done interrupt
    if (flash1_env.write_int_s == FLASH_TRANS_BUSY) {
        *is_wip = 1;
        return OM_ERROR_OK;
    }
    // Check ospi busy status again
    if (drv_ospi_is_busy(om_flash)) {
        return OM_ERROR_BUSY;
    }
    // Send command to check wip status
    CMD_FRAME_SET(&wip_frame, FLASH_READ_STA_REG1_CFG(flash1_env.xpi_mode));
    if ((error = oflash_read_reg(&wip_frame, &status, 1)) != OM_ERROR_OK) {
        return error;
    }
    *is_wip = (status & FLASH_STATUS_1_WIP_MASK) ? 1 : 0;
    return OM_ERROR_OK;
}

om_error_t drv_oflash_write_int_continue(OM_OSPI_Type *om_flash)
{
    oflash_env_t *env = &flash1_env;
    om_error_t error;

    if ((env->state == FLASH_STATE_WRITING) && (env->data_cnt < env->data_len)) {
        if ((error = oflash_write_enable(om_flash)) == OM_ERROR_OK) {
            // Write to the next Page boundary
            uint32_t data_remain = env->data_len - env->data_cnt;
            uint32_t write_len = (data_remain >= FLASH_PAGE_SIZE) ? FLASH_PAGE_SIZE : data_remain;
            flash_frame_t write_frame = flash_write_frame_get(env->write_cmd);
            uint32_t cmd[2] = {write_frame.cmd << 24, env->addr};
            env->write_int_s = FLASH_TRANS_BUSY;
            if ((error = drv_ospi_write_int(om_flash, cmd, env->data, write_len)) != OM_ERROR_OK) {
                return error;
            }
            env->addr += write_len;
            env->data += write_len;
            env->data_cnt += write_len;
            return OM_ERROR_OK;
        } else {
            return error;
        }
    }
    return OM_ERROR_STATUS;
}

om_error_t drv_oflash_erase(OM_OSPI_Type *om_flash,
                            uint32_t addr,
                            flash_erase_t erase_type,
                            uint32_t timeout_ms)
{
    oflash_env_t *env = &flash1_env;
    om_error_t error = OM_ERROR_OK;
    flash_frame_t erase_frame;
    flash_frame_t wip_frame;
    uint8_t xba, xpi;

    if (env->state != FLASH_STATE_INIT) {
        return OM_ERROR_BUSY;
    }
    OM_CRITICAL_BEGIN();
    xba = flash1_env.xba_mode;
    xpi = flash1_env.xpi_mode;
    switch (erase_type) {
        case FLASH_ERASE_4K:
            CMD_FRAME_SET(&erase_frame, FLASH_SEC_ERASE_4K_CFG(xpi, xba));
            break;
        case FLASH_ERASE_32K:
            CMD_FRAME_SET(&erase_frame, FLASH_BLK_ERASE_32K_CFG(xpi, xba));
            break;
        case FLASH_ERASE_64K:
            CMD_FRAME_SET(&erase_frame, FLASH_BLK_ERASE_64K_CFG(xpi, xba));
            break;
        case FLASH_ERASE_CHIP:
            CMD_FRAME_SET(&erase_frame, FLASH_CHIP_ERASE_CFG(xpi, xba));
            break;
        default:
            return OM_ERROR_RESOURCES;
    }
    CMD_FRAME_SET(&wip_frame, FLASH_READ_STA_REG1_CFG(xpi));
    OM_CRITICAL_END();
    env->state = FLASH_STATE_ERASING;
    error = oflash_write_enable(om_flash);
    if (error == OM_ERROR_OK) {
        #if (RTE_FLASH1_XIP)
            flash_frame_t suspend_frame, resume_frame;
            CMD_FRAME_SET(&suspend_frame, FLASH_SUSPEND_CFG(env->xpi_mode));
            CMD_FRAME_SET(&resume_frame, FLASH_RESUME_CFG(env->xpi_mode));
            error = oflash_erase_with_suspend(om_flash, addr,
                            &erase_frame, &wip_frame, &suspend_frame, &resume_frame, timeout_ms);
            #else
            error = oflash_erase(om_flash, addr, &erase_frame, &wip_frame, timeout_ms);
            #endif
    }
    env->state = FLASH_STATE_INIT;
    return error;
}

om_error_t drv_oflash_read_id(OM_OSPI_Type *om_flash, flash_id_t *id)
{
    flash_frame_t id_frame;
    om_error_t error;

    CMD_FRAME_SET(&id_frame, FLASH_READ_ID_CFG(flash1_env.xpi_mode));
    if ((error = oflash_read_id(&id_frame, id)) != OM_ERROR_OK) {
        return error;
    }
    return OM_ERROR_OK;
}

om_error_t drv_oflash_read_status_reg1(OM_OSPI_Type *om_flash, uint8_t *status)
{
    flash_frame_t reg_frame;

    CMD_FRAME_SET(&reg_frame, FLASH_READ_STA_REG1_CFG(flash1_env.xpi_mode));
    return oflash_read_reg(&reg_frame, status, 1);
}

om_error_t drv_oflash_read_status_reg2(OM_OSPI_Type *om_flash, uint8_t *status)
{
    flash_frame_t reg_frame;

    CMD_FRAME_SET(&reg_frame, FLASH_READ_STA_REG2_CFG(flash1_env.xpi_mode));
    return oflash_read_reg(&reg_frame, status, 1);
}

om_error_t drv_oflash_read_config_reg(OM_OSPI_Type *om_flash, uint8_t *config)
{
    flash_frame_t reg_frame;

    CMD_FRAME_SET(&reg_frame, FLASH_READ_CFG_REG_CFG(flash1_env.xpi_mode));
    return oflash_read_reg(&reg_frame, config, 1);
}

om_error_t drv_oflash_write_status(OM_OSPI_Type *om_flash, uint8_t status[2])
{
    return oflash_write_status(om_flash, status, 0);
}

om_error_t drv_oflash_write_status_volatile(OM_OSPI_Type *om_flash, uint8_t status[2])
{
    return oflash_write_status(om_flash, status, 1);
}

om_error_t drv_oflash_write_status_reg1(OM_OSPI_Type *om_flash, uint8_t *status)
{
    return oflash_write_status_reg1(om_flash, status, 0);
}

om_error_t drv_oflash_write_status_reg1_volatile(OM_OSPI_Type *om_flash, uint8_t *status)
{
    return oflash_write_status_reg1(om_flash, status, 1);
}

om_error_t drv_oflash_write_status_reg2(OM_OSPI_Type *om_flash, uint8_t *status)
{
    return oflash_write_status_reg2(om_flash, status, 0);
}

om_error_t drv_oflash_write_status_reg2_volatile(OM_OSPI_Type *om_flash, uint8_t *status)
{
    return oflash_write_status_reg2(om_flash, status, 1);
}

om_error_t drv_oflash_write_config_reg(OM_OSPI_Type *om_flash, uint8_t *config)
{
    return oflash_write_config_reg(om_flash, config, 0);
}

om_error_t drv_oflash_write_config_reg_volatile(OM_OSPI_Type *om_flash, uint8_t *config)
{
    return oflash_write_config_reg(om_flash, config, 1);
}

om_error_t drv_oflash_modifiy_status_bits(OM_OSPI_Type *om_flash, uint8_t status[2], uint8_t mask[2])
{
    return oflash_modifiy_status_bits(om_flash, status, mask, 0);
}

om_error_t drv_oflash_modifiy_status_bits_volatile(OM_OSPI_Type *om_flash, uint8_t status[2], uint8_t mask[2])
{
    return oflash_modifiy_status_bits(om_flash, status, mask, 1);
}

om_error_t drv_oflash_write_protect_set(OM_OSPI_Type *om_flash, flash_protect_t protect)
{
    // Attention:
    // This function support the following flash:
    // GD25WQ16E, GD25WQ80E, GD25WQ40E,
    // P25Q40SU, P25Q80SU, P25Q16SU,
    // GT25Q40D, GT25Q80A
    // Other flash without testing,
    // if not supported, please use drv_oflash_modifiy_status_bits to set the status register

    uint8_t mask[2] = {FLASH_STATUS_1_BP_MASK, FLASH_STATUS_2_CMP_MASK};
    uint8_t status[2] = {protect, FLASH_STATUS_2_CMP_MASK};

    return drv_oflash_modifiy_status_bits(om_flash, status, mask);
}

om_error_t drv_oflash_write_protect_set_volatile(OM_OSPI_Type *om_flash, flash_protect_t protect)
{
    uint8_t mask[2] = {FLASH_STATUS_1_BP_MASK, FLASH_STATUS_2_CMP_MASK};
    uint8_t status[2] = {protect, FLASH_STATUS_2_CMP_MASK};

    return drv_oflash_modifiy_status_bits_volatile(om_flash, status, mask);
}

om_error_t drv_oflash_quad_enable(OM_OSPI_Type *om_flash, bool enable)
{
    uint8_t mask[2] = {0, FLASH_STATUS_2_QE_MASK};
    uint8_t status[2] = {0, 0};

    if (enable) {
        status[1] = FLASH_STATUS_2_QE_MASK;
    }

    return drv_oflash_modifiy_status_bits(om_flash, status, mask);
}

om_error_t drv_oflash_quad_enable_volatile(OM_OSPI_Type *om_flash, bool enable)
{
    uint8_t mask[2] = {0, FLASH_STATUS_2_QE_MASK};
    uint8_t status[2] = {0, 0};

    if (enable) {
        status[1] = FLASH_STATUS_2_QE_MASK;
    }

    return drv_oflash_modifiy_status_bits_volatile(om_flash, status, mask);
}

om_error_t drv_oflash_read_param_set(OM_OSPI_Type *om_flash, uint8_t param)
{
    oflash_env_t *env = &flash1_env;
    flash_frame_t set_param_frame, wip_frame;

    CMD_FRAME_SET(&set_param_frame, FLASH_SET_READ_PARAM_CFG(0));
    CMD_FRAME_SET(&wip_frame, FLASH_READ_STA_REG1_CFG(env->xpi_mode));

    // QPI mode only
    if (env->xpi_mode != M_QPI) {
        return OM_ERROR_PERMISSION;
    }
    return oflash_write_reg(&set_param_frame, &wip_frame, &param, 1, SR_WRITE_EN_NONE);
}

om_error_t drv_oflash_4byte_addr_enable(OM_OSPI_Type *om_flash, uint8_t enable)
{
    oflash_env_t *env = &flash1_env;
    om_error_t error;
    flash_frame_t rw_frame;
    flash_frame_t addr_frame;

    if (enable) {
        CMD_FRAME_SET(&addr_frame, FLASH_4BADR_ENABLE_CFG(env->xpi_mode));
    } else {
        CMD_FRAME_SET(&addr_frame, FLASH_4BADR_DISABLE_CFG(env->xpi_mode));
    }
    OM_CRITICAL_BEGIN();
    error = oflash_read_reg(&addr_frame, NULL, 0);
    if (error == OM_ERROR_OK) {
        env->xba_mode = enable ? M_4BA : M_3BA;
        drv_ospi_set_addr_4byte(om_flash, env->xba_mode);
        rw_frame = flash_read_frame_get(env->read_cmd);
        drv_ospi_read_frame_set(om_flash, (const ospi_frame_t *)&rw_frame);
        rw_frame = flash_write_frame_get(env->write_cmd);
        drv_ospi_write_frame_set(om_flash, (const ospi_frame_t *)&rw_frame);
    }
    OM_CRITICAL_END();
    return error;
}

__OF_RAM_CODE om_error_t drv_oflash_reset(OM_OSPI_Type *om_flash)
{
    om_error_t error = OM_ERROR_OK;
    flash_frame_t rst_en_frame, rst_frame, rw_frame;
    oflash_env_t *env = &flash1_env;

    CMD_FRAME_SET(&rst_en_frame, FLASH_RESET_ENABLE_CFG(flash1_env.xpi_mode));
    CMD_FRAME_SET(&rst_frame, FLASH_RESET_CFG(flash1_env.xpi_mode));

    if ((error = oflash_read_reg(&rst_en_frame, NULL, 0)) != OM_ERROR_OK) {
        return error;
    }
    if ((error = oflash_read_reg(&rst_frame, NULL, 0)) != OM_ERROR_OK) {
        return error;
    }
    // reset env spi mode
    env->xpi_mode = M_SPI;
    if (env->read_cmd >= FLASH_FAST_READ_QPI_4_DUMMY) {
        env->read_cmd = FLASH_FAST_READ;
    }
    // reset env 3-byte address mode
    env->xba_mode = M_3BA;
    drv_ospi_set_addr_4byte(om_flash, env->xba_mode);
    rw_frame = flash_read_frame_get(env->read_cmd);
    drv_ospi_read_frame_set(om_flash, (const ospi_frame_t *)&rw_frame);
    rw_frame = flash_write_frame_get(env->write_cmd);
    drv_ospi_write_frame_set(om_flash, (const ospi_frame_t *)&rw_frame);
    return error;
}

__OF_RAM_CODE om_error_t drv_oflash_encrypt_enable(OM_OSPI_Type *om_flash, uint8_t enable)
{
    #if (RTE_EFUSE)
    if (enable) {
        drv_efuse_control(EFUSE_CONTROL_FETCH_UID, (void *)0);
    }
    #endif /* RTE_EFUSE */
    drv_ospi_set_crypt_control(om_flash, enable, enable);
    return OM_ERROR_OK;
}

om_error_t drv_oflash_list_start(OM_OSPI_Type *om_flash, flash_list_node_t *list_head, uint32_t node_timeout_ms)
{
    om_error_t error;
    oflash_env_t *env = &flash1_env;
    uint8_t dummy;

    if ((error = drv_oflash_read_cmd_set(om_flash, env->read_cmd)) != OM_ERROR_OK) {
        return error;
    }
    drv_ospi_list_start(om_flash, list_head);
    // dummy read to trigger list start
    return drv_oflash_read(om_flash, 0, &dummy, 1, node_timeout_ms);
}

#if (RTE_FLASH1_REGISTER_CALLBACK)
om_error_t drv_oflash_register_isr_callback(OM_OSPI_Type *om_flash, drv_isr_callback_t isr_cb)
{
    oflash_env_t *env = &flash1_env;

    env->isr_cb = isr_cb;
    return OM_ERROR_OK;
}

__WEAK void drv_oflash_isr_callback(OM_OSPI_Type *om_flash, drv_event_t event)
{
    oflash_env_t *env = &flash1_env;
    flash_state_t state = env->state;
    drv_event_t isr_evt = DRV_EVENT_COMMON_NONE;

    if (event & DRV_EVENT_COMMON_TRANSFER_COMPLETED) {
        if (state == FLASH_STATE_WRITING) {
            env->write_int_s = FLASH_TRANS_IDLE;
            if (env->data_cnt >= env->data_len) {
                env->data -= env->data_cnt;
                isr_evt |= DRV_EVENT_FLASH_WRITE_TRANSFER_COMPLETED;
            } else {
                // Writing not finished, wait for write continue
                return;
            }
        } else if (state == FLASH_STATE_READING) {
            isr_evt |= DRV_EVENT_COMMON_READ_COMPLETED;
        }
    }
    if (event & DRV_EVENT_OSPI_LIST_COMPLETED) {
        isr_evt |= DRV_EVENT_FLASH_LIST_COMPLETED;
    }
    if (event & DRV_EVENT_OSPI_LIST_NODE_COMPLETED) {
        isr_evt |= DRV_EVENT_FLASH_LIST_NODE_COMPLETED;
    }
    env->state = FLASH_STATE_INIT;
    if (isr_evt && env->isr_cb) {
        env->isr_cb(om_flash, isr_evt, (void *)env->data, (void *)env->data_len);
    }
}
#endif /* RTE_FLASH1_REGISTER_CALLBACK */
#endif /* (RTE_FLASH1) */

/** @} */
