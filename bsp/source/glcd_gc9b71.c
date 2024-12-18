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
 * @brief    glcd driver
 * @details  graphics lcd gc9b71 driver
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
#include "om_driver.h"
#include "autoconf.h"
#if(CONFIG_GLCD_GC9B71)
#include "glcd.h"
#include "string.h"
#include <stdint.h>
#include "stdio.h"
#include "om_mem.h"
#include "om_compiler.h"

/*******************************************************************************
 * MACROS
 */
/// hardware limitation is 65535, but we use psram page size to support accross page read
#define GLCD_MAX_DATA_BYTES         (320*72*2)                         // usually psram page size
#define GLCD_DELAY                  DRV_DELAY_MS
#define GLCD_REG_DELAY(ms)          0xFF, ms
#define GLCD_KEEP_CS                0
#define GLCD_GC9B71_FORMAT          0x7
/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    volatile uint8_t lcd_finish;               // 0: lcd is busy, 1: lcd is idle (finish)
    volatile uint8_t flush_finish;              // 0: flush is busy, 1: flush is idle (finish)
    volatile uint8_t display_buffer_mode;       // 0: display buffer is increasing, 1: display buffer is stable, 2: stride mode
    uint32_t stride;                            // display buffer stride
    uint32_t width;                             // display buffer width
    uint8_t* display_buffer;                    // function glcd_disp_flush get user buffer stored in this variable
    uint32_t display_number_bytes;              // function glcd_disp_flush get user buffer length stored in this variable
    uint32_t max_number_bytes;                  // function glcd_write_memory once write max_number_bytes data to lcd
    uint8_t* clear_data_buffer;                 // function glcd_clear use this malloc buffer to clear display
    glcd_cb_t gc9c01_cb;                        // user callback function
    uint8_t clk_div;                            // clk_div
} glcd_env_t;


/*******************************************************************************
 * CONST & VARIABLES
 */
static glcd_env_t glcd_env = {
    .lcd_finish = 1,
    .flush_finish = 1,
    .gc9c01_cb = NULL,
    .display_buffer = NULL,
    .display_buffer_mode = 0,
    .display_number_bytes = 0,
    .max_number_bytes = GLCD_MAX_DATA_BYTES,
    .clear_data_buffer = NULL,
};

/**
 *  call GLCD_REG_DELAY(0) even if no need to delay
 */
static const uint8_t glcd_init_seq[] = {
    0x1, 0xfe , GLCD_REG_DELAY(0),
    0x1, 0xef , GLCD_REG_DELAY(0),
    0x2, 0x80, 0x11, GLCD_REG_DELAY(0),
    0x2, 0x81, 0x70, GLCD_REG_DELAY(0),
    0x2, 0x82, 0x09, GLCD_REG_DELAY(0),
    0x2, 0x83, 0x03, GLCD_REG_DELAY(0),
    0x2, 0x84, 0x22, GLCD_REG_DELAY(0),
    0x2, 0x89, 0x18, GLCD_REG_DELAY(0),
    0x2, 0x8A, 0x40, GLCD_REG_DELAY(0),
    0x2, 0x8B, 0x0A, GLCD_REG_DELAY(0),
    0x2, 0x3a, GLCD_GC9B71_FORMAT, GLCD_REG_DELAY(0),
    0x2, 0x36, 0x40, GLCD_REG_DELAY(0),
    0x2, 0xEC, 0x07, GLCD_REG_DELAY(0),
    0x7, 0x74, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, GLCD_REG_DELAY(0),
    0x2, 0x98, 0x3E, GLCD_REG_DELAY(0),
    0x2, 0x99, 0x3E, GLCD_REG_DELAY(0),
    0x3, 0xA1, 0x01, 0x04, GLCD_REG_DELAY(0),
    0x3, 0xA2, 0x01, 0x04, GLCD_REG_DELAY(0),
    0x2, 0xCB, 0x02, GLCD_REG_DELAY(0),
    0x2, 0xAC, 0x44, GLCD_REG_DELAY(0),
    0x3, 0x7C, 0xB6, 0x25, GLCD_REG_DELAY(0),
    0x2, 0xF6, 0x80, GLCD_REG_DELAY(0),
    0x3, 0xB5, 0x09, 0x09, GLCD_REG_DELAY(0),
    0x3, 0xEB, 0x01, 0x7B, GLCD_REG_DELAY(0),
    0x5, 0x60, 0x58, 0x09, 0x5B, 0x56, GLCD_REG_DELAY(0),
    0x5, 0x63, 0x5B, 0x05, 0x5B, 0x56, GLCD_REG_DELAY(0),
    0x7, 0x64, 0x38, 0x0F, 0x73, 0x17, 0x5B, 0x56, GLCD_REG_DELAY(0),
    0x7, 0x66, 0x38, 0x0F, 0x73, 0x17, 0x5B, 0x56, GLCD_REG_DELAY(0),
    0x7, 0x65, 0x38, 0x0B, 0x73, 0x13, 0x5B, 0x56, GLCD_REG_DELAY(0),
    0x21, 0x6E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x0D, 0x0F, 0x09, 0x0B, 0x07, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x14, 0x12,
    0x10, 0x0E, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, GLCD_REG_DELAY(0),
    0x8, 0x6C, 0xCC, 0x0C, 0xCC, 0x84, 0xCC, 0x04, 0x50, GLCD_REG_DELAY(0),
    0x2, 0x7D, 0x72, GLCD_REG_DELAY(0),
    0xb, 0x70, 0x02, 0x03, 0x09, 0x04, 0x0C, 0x06, 0x09, 0x05, 0x0C, 0x06, GLCD_REG_DELAY(0),
    0x5, 0x90, 0x06, 0x06, 0x05, 0x06, GLCD_REG_DELAY(0),
    0x4, 0x93, 0x45, 0xFF, 0x00, GLCD_REG_DELAY(0),
    0x2, 0xC3, 0x40, GLCD_REG_DELAY(0),
    0x2, 0xC4, 0x60, GLCD_REG_DELAY(0),
    0x2, 0xC9, 0x3d, GLCD_REG_DELAY(0),
    0x7, 0xF0, 0x47, 0x07, 0x08, 0x02, 0x00, 0x27, GLCD_REG_DELAY(0),
    0x7, 0xF2, 0x47, 0x08, 0x07, 0x02, 0x00, 0x27, GLCD_REG_DELAY(0),
    0x7, 0xF1, 0x41, 0xaA, 0x5a, 0x28, 0x2C, 0xeF, GLCD_REG_DELAY(0),
    0x7, 0xF3, 0x41, 0x9A, 0x5a, 0x28, 0x2C, 0xeF, GLCD_REG_DELAY(0),
    0x2, 0xF9, 0x30, GLCD_REG_DELAY(0),
    0x2, 0xBE, 0x11, GLCD_REG_DELAY(0),
    0x3, 0xFB, 0x00, 0x00, GLCD_REG_DELAY(0),
    0x3, 0xAA, 0xb0, 0x00, GLCD_REG_DELAY(0),
    0x1, 0x11 , GLCD_REG_DELAY(120),
    0x1, 0x29 , GLCD_REG_DELAY(0),
    0x1, 0x2C , GLCD_REG_DELAY(120),
};


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
static void gc9b71_callback(glcd_cb_t* cb);

static void glcd_raw_init(uint8_t clk_div)
{
    lcd_config_t lcd_config = {
        .clk_div = clk_div,
        .spi_mode = LCD_SPI_MODE_0,
    };
    drv_lcd_init(&lcd_config);
    drv_lcd_register_isr_callback(OM_LCD, (drv_isr_callback_t)gc9b71_callback);
}

static void glcd_hw_reset(void)
{
    drv_gpio_write(PORT_GLCD_LCD_RST, 1 << IDX_GLCD_LCD_RST, 1);
    GLCD_DELAY(60);
    drv_gpio_write(PORT_GLCD_LCD_RST, 1 << IDX_GLCD_LCD_RST, 0);
    GLCD_DELAY(100);
    drv_gpio_write(PORT_GLCD_LCD_RST, 1 << IDX_GLCD_LCD_RST, 1);
    GLCD_DELAY(60);
}

/**
 *  the param and must be in uncachable ram
 */
static void glcd_write_reg(uint8_t reg, const uint8_t *param, uint16_t param_bytes)
{
    while(!glcd_env.lcd_finish);
    drv_lcd_control(LCD_CONTROL_SET_DATA_BIT, (void *)1);
    drv_lcd_control(LCD_CONTROL_SET_DATA_WIDTH, (void *)8);
    drv_lcd_control(LCD_CONTROL_SET_BUF_MODE_COMMON, (void *)0);
    drv_lcd_control(LCD_CONTROL_TX_DATA_MODE, (void *)8);
    uint8_t cmd[] = {0x02, 0x00, 0x00, 0x00};
    cmd[2] = reg;
    drv_lcd_write(cmd, 32, param, param_bytes);
    drv_lcd_control(LCD_CONTROL_SET_DATA_BIT, (void *)4);
    drv_lcd_control(LCD_CONTROL_SET_DATA_WIDTH, (void *)32);
    drv_lcd_control(LCD_CONTROL_TX_DATA_MODE, (void *)32);
    drv_lcd_control(LCD_CONTROL_SET_BUF_MODE_666_888, (void *)0);
}

static void glcd_write_reg_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    static uint8_t coord_dma[4];
    coord_dma[1] = x1 & 0xFF;
    coord_dma[0] = (x1 >> 8) & 0xFF;
    coord_dma[3] = x2 & 0xFF;
    coord_dma[2] = (x2 >> 8) & 0xFF;

    uint8_t reg = 0x2A;
    glcd_write_reg(reg, coord_dma, 4);

    coord_dma[1] = y1 & 0xFF;
    coord_dma[0] = (y1 >> 8) & 0xFF;
    coord_dma[3] = y2 & 0xFF;
    coord_dma[2] = (y2 >> 8) & 0xFF;
    reg = 0x2B;
    glcd_write_reg(reg, coord_dma, 4);
}

static void glcd_read_reg(uint8_t reg, uint8_t *param, uint16_t param_bytes)
{
    while(!glcd_env.lcd_finish);
    drv_lcd_control(LCD_CONTROL_SET_DATA_BIT, (void *)1);
    drv_lcd_control(LCD_CONTROL_SET_DATA_WIDTH, (void *)8);
    drv_lcd_control(LCD_CONTROL_SET_BUF_MODE_COMMON, (void *)0);
    uint8_t cmd[] = {0x00, 0x00, 0x00, 0x03};
    cmd[1] = reg;
    drv_lcd_read(cmd, 32, param, param_bytes);
    drv_lcd_control(LCD_CONTROL_SET_DATA_BIT, (void *)4);
    drv_lcd_control(LCD_CONTROL_SET_DATA_WIDTH, (void *)8);
    drv_lcd_control(LCD_CONTROL_SET_BUF_MODE_666_888, (void *)0);
}

static void glcd_write_memory(uint8_t write_command, uint8_t* data, uint16_t data_bytes)
{
    uint8_t cmd[] = {0x32, 0x00, write_command, 0x00};
    while(!glcd_env.lcd_finish);
    glcd_env.lcd_finish = 0;
    drv_lcd_write_int(cmd, 32, data, data_bytes);
}

/**
 * @brief Initialize the lcd hardware by sending glcd_init_seq[]
 */
static void glcd_init_sequence()
{
    uint8_t reg;
    uint8_t cmd_lens;
    uint8_t data_bytes;
    uint32_t i = 0;
    while(i < sizeof(glcd_init_seq)) {
        cmd_lens = glcd_init_seq[i];
        data_bytes = cmd_lens - 1;
        reg = glcd_init_seq[i+1];
        glcd_write_reg(reg, &(glcd_init_seq[i+2]), data_bytes);
        i += (cmd_lens + 1);
        if(glcd_init_seq[i] == 0xff) {
            GLCD_DELAY(glcd_init_seq[i+1]);
            i+=2;
        }
    }
}

// lcd call back
static void gc9b71_callback(glcd_cb_t* cb)
{
    glcd_env.lcd_finish = 1;

    if(glcd_env.display_number_bytes > 0) {   // still have data to transfer
        if(((uint32_t)glcd_env.display_buffer >= 0x70000000) && GLCD_KEEP_CS) {     // if data from psram , cs need to be reset to support cross page
            register_set(&OM_LCD->COMMAND, MASK_1REG(LCD_COMMAND_KEEP_CS, 1));
        }
        uint32_t current_send_bytes;
        uint32_t disp_buf_incress_len;
        if(glcd_env.display_buffer_mode == 1) {
            current_send_bytes = glcd_env.display_number_bytes > glcd_env.max_number_bytes? glcd_env.max_number_bytes : glcd_env.display_number_bytes;
            disp_buf_incress_len = current_send_bytes;
            glcd_env.display_number_bytes -= current_send_bytes;
        } else if(glcd_env.display_buffer_mode == 2) {
            current_send_bytes = glcd_env.width;
            disp_buf_incress_len = glcd_env.stride;
            glcd_env.display_number_bytes -= 1;
        } else {
            current_send_bytes = glcd_env.display_number_bytes > glcd_env.max_number_bytes? glcd_env.max_number_bytes : glcd_env.display_number_bytes;
            disp_buf_incress_len = 0;
            glcd_env.display_number_bytes -= current_send_bytes;
        }
        glcd_write_memory(0x3C, glcd_env.display_buffer, current_send_bytes);   // call glcd_write_memory will reset cs.
        glcd_env.display_buffer += disp_buf_incress_len;
    } else {
        if(glcd_env.gc9c01_cb) {          // run gc9c01 call back if registered
            glcd_env.gc9c01_cb();
        }
        if(glcd_env.clear_data_buffer) {  // run clear data buffer if glcd_clear
            om_mem_free(OM_MEM(0), glcd_env.clear_data_buffer);
            glcd_env.clear_data_buffer = NULL;
        }
        glcd_env.flush_finish = 1;        // flush finish
    }
}


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
void glcd_read_id()
{
    uint8_t reg_adr = 0x04;
    uint8_t id[8];
    glcd_read_reg(reg_adr, id, 8);
}

void glcd_read_status()
{
    uint8_t reg_adr = 0x09;
    uint8_t lcd_status[8];
    glcd_read_reg(reg_adr, lcd_status, 8);
}

void glcd_write_reg_value(uint8_t value)
{
    static uint8_t coord_dma[4];
    uint8_t reg_adr = 0x53;
    memset(coord_dma, value, 4);
    glcd_write_reg(reg_adr, coord_dma, 1);
}

void glcd_set_brightness(uint8_t brightness)
{
    uint8_t reg = 0x51;
    uint8_t data = brightness;
    glcd_write_reg(reg, &data, 1);
}

void glcd_control(glcd_control_t control)
{
    glcd_write_reg(control, 0, 0);
}

void glcd_init(uint8_t clk_div)
{
    glcd_raw_init(clk_div);
    glcd_hw_reset();
    glcd_init_sequence();
    glcd_clear(0xffffff, 0, 0, GLCD_X_RESOLUTION - 1, GLCD_Y_RESOLUTION - 1);
    glcd_disp_flush_finish();
    glcd_clear(0xffffff, 0, 0, GLCD_X_RESOLUTION - 1, GLCD_Y_RESOLUTION - 1);
    glcd_disp_flush_finish();
}

void glcd_set_disp_window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    glcd_write_reg_window(x1, y1, x2, y2);
}

void glcd_disp_flush(void *color, uint32_t color_len)
{
    // send data to lcd, data over than GLCD_MAX_DATA_BYTES will transfer in irq.
    // GLCD_MAX_DATA_BYTES usually is a psram page size.
    while(!glcd_env.flush_finish);
    glcd_env.flush_finish = 0;
    uint32_t left_data_bytes = color_len;
    uint32_t data_index = 0;
    uint32_t current_send_bytes = left_data_bytes < GLCD_MAX_DATA_BYTES? left_data_bytes : GLCD_MAX_DATA_BYTES;
    left_data_bytes -= current_send_bytes;
    data_index += current_send_bytes;
    glcd_env.display_number_bytes = left_data_bytes;
    glcd_env.display_buffer = (uint8_t*)color + data_index;
    glcd_env.display_buffer_mode = 1;
    glcd_env.max_number_bytes = GLCD_MAX_DATA_BYTES;
    if(((uint32_t)glcd_env.display_buffer >= 0x70000000) && GLCD_KEEP_CS) {     // if data from psram , cs need to be reset to support cross page
        register_set(&OM_LCD->COMMAND, MASK_1REG(LCD_COMMAND_KEEP_CS, 1));
    }
    glcd_write_memory(0x2C, color, current_send_bytes);
}


void glcd_disp_flush_stride(void *color, uint32_t width, uint32_t stride, uint32_t lines)
{
    // send data to lcd, data over than GLCD_MAX_DATA_BYTES will transfer in irq.
    // GLCD_MAX_DATA_BYTES usually is a psram page size.
    while(!glcd_env.flush_finish);
    glcd_env.flush_finish = 0;
    uint32_t current_send_bytes = width;
    glcd_env.stride = stride;
    glcd_env.width = width;
    glcd_env.display_number_bytes = lines - 1;
    glcd_env.display_buffer = (uint8_t*)color + stride;
    glcd_env.display_buffer_mode = 2;
    glcd_env.max_number_bytes = GLCD_MAX_DATA_BYTES;
    if(((uint32_t)glcd_env.display_buffer >= 0x70000000) && GLCD_KEEP_CS) {     // if data from psram , cs need to be reset to support cross page
        register_set(&OM_LCD->COMMAND, MASK_1REG(LCD_COMMAND_KEEP_CS, 1));
    }
    glcd_write_memory(0x2C, color, current_send_bytes);
}

void glcd_set_register(uint8_t reg)
{
    glcd_write_reg(reg, 0, 0);
}

void glcd_callback_register(glcd_cb_t cb)
{
    glcd_env.gc9c01_cb = cb;
}

void glcd_clear(uint32_t color, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint32_t pixel_width = x2 - x1 + 1;
    uint32_t pixel_height = y2 - y1 + 1;
    glcd_env.clear_data_buffer  = om_mem_malloc(OM_MEM(0), pixel_width * LCD_BYTES_PER_PIX);
    for(uint32_t i = 0; i < pixel_width * LCD_BYTES_PER_PIX; i+=3) {
        ((uint8_t*)glcd_env.clear_data_buffer)[i] = color;
        ((uint8_t*)glcd_env.clear_data_buffer)[i+1] = color>>8;
        ((uint8_t*)glcd_env.clear_data_buffer)[i+2] = color>>16;
    }
    while(!glcd_env.flush_finish);
    glcd_env.flush_finish = 0;
    uint32_t left_data_bytes = pixel_width * pixel_height * LCD_BYTES_PER_PIX;
    glcd_set_disp_window(x1, y1, x2, y2);
    uint32_t current_send_bytes = left_data_bytes < pixel_width * LCD_BYTES_PER_PIX? left_data_bytes : pixel_width * LCD_BYTES_PER_PIX;
    left_data_bytes -= current_send_bytes;
    glcd_env.display_number_bytes = left_data_bytes;
    glcd_env.display_buffer = glcd_env.clear_data_buffer;
    glcd_env.display_buffer_mode = 0;
    glcd_env.max_number_bytes = pixel_width * LCD_BYTES_PER_PIX;
    glcd_write_memory(0x2C, glcd_env.clear_data_buffer, current_send_bytes);
}

void glcd_clear_line(uint8_t* color_buf, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    uint32_t pixel_width = x2 - x1 + 1;
    uint32_t pixel_height = y2 - y1 + 1;
    glcd_env.clear_data_buffer  = om_mem_malloc(OM_MEM(0), pixel_width * LCD_BYTES_PER_PIX);
    for(uint32_t i = 0; i < pixel_width * LCD_BYTES_PER_PIX / 2; i++) {
        ((uint16_t*)glcd_env.clear_data_buffer)[i] = color_buf[i];   /*lint !e2445 */
    }
    while(!glcd_env.flush_finish);
    glcd_env.flush_finish = 0;
    uint32_t left_data_bytes = pixel_width * pixel_height * LCD_BYTES_PER_PIX;
    glcd_set_disp_window(x1, y1, x2, y2);
    uint32_t current_send_bytes = left_data_bytes < pixel_width * LCD_BYTES_PER_PIX? left_data_bytes : pixel_width * LCD_BYTES_PER_PIX;
    left_data_bytes -= current_send_bytes;
    glcd_env.display_number_bytes = left_data_bytes;
    glcd_env.display_buffer = glcd_env.clear_data_buffer;
    glcd_env.display_buffer_mode = 0;
    glcd_env.max_number_bytes = pixel_width * LCD_BYTES_PER_PIX;
    glcd_write_memory(0x2C, glcd_env.clear_data_buffer, current_send_bytes);
}

void glcd_disp_flush_finish()
{
    while(!glcd_env.flush_finish);
}

#endif  /*  RTE_LCD && CONFIG_DISP_GC9B71 */
