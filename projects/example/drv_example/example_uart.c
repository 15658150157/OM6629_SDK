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
 * @brief    example for using uart
 * @details  example for using uart: blocking, interrupt, and DMA
 * @version
 *
 * Version 1.0
 *  - Initial release
 *
 * @{
 */


/*******************************************************************************
 * INCLUDES
 */
#include "om_driver.h"


/*******************************************************************************
 * MACROS
 */
/// Test tx pad for uart1
#define PAD_UART1_TXD               5
/// Test rx pad for uart1
#define PAD_UART1_RXD               6
/// Test cts pad for uart1
#define PAD_UART1_CTS               7
/// Test rts pad for uart1
#define PAD_UART1_RTS               8
/// Test uart
#define TEST_UART                   OM_UART1
/// Test uart baudrate
#define UART_BAUDRATE              115200
/// Wait flag defination
#define SEND_INT_IS_DONE            (1 << 0)
#define SEND_DMA_IS_DONE            (1 << 1)
#define READ_INT_IS_DONE            (1 << 2)
#define READ_DMA_IS_DONE            (1 << 3)


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    volatile uint8_t st;
} uart_gpdma_int_wait_flag_t;


/*******************************************************************************
 * CONST & VARIABLES
 */
/// Test uart pin configuration
static const pin_config_t pin_config[] = {
    {PAD_UART1_TXD, {PINMUX_PAD5_UART1_TRX_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_UART1_RXD, {PINMUX_PAD6_UART1_RX_CFG}, PMU_PIN_MODE_PU, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_UART1_RTS, {PINMUX_PAD8_UART1_RTS_CFG}, PMU_PIN_MODE_PP, PMU_PIN_DRIVER_CURRENT_NORMAL},
    {PAD_UART1_CTS, {PINMUX_PAD7_UART1_CTS_CFG}, PMU_PIN_MODE_PU, PMU_PIN_DRIVER_CURRENT_NORMAL},
};
/// Buffer to store the received data
static uint32_t rec_data[4];
/// Buffer that stores the data to be sent
static uint32_t send_data[4];
/// DMA,INT wait flag
static uart_gpdma_int_wait_flag_t wait_flag;


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief callback for uart interrupt
 *
 * @param[in] om_uart    Pointer to uart
 * @param[in] event       uart event
 *                        - DRV_EVENT_COMMON_WRITE_COMPLETED
 *                        - DRV_EVENT_COMMON_DMA2PERIPH_COMPLETED
 *                        - DRV_EVENT_COMMON_READ_COMPLETED
 *                        - DRV_EVENT_UART_RX_TIMEOUT
 *                        - DRV_EVENT_COMMON_RX_OVERFLOW
 *                        - DRV_EVENT_UART_RX_PARITY_ERROR
 *                        - DRV_EVENT_UART_RX_BREAK
 *                        - DRV_EVENT_UART_RX_FRAME_ERROR
 * @param[in] rxbuf      Pointer to receive buffer
 * @param[in] rx_cnt     The number of received data
 *
 *******************************************************************************
 */
static void test_uart_cb(void *om_uart, drv_event_t event, void *rxbuf, void *rx_cnt)
{
    // Send what you receive
    if (event == DRV_EVENT_COMMON_READ_COMPLETED) {
        drv_uart_write(TEST_UART, (uint8_t *)rxbuf, (uint32_t)rx_cnt, 10);
        wait_flag.st |= READ_INT_IS_DONE;
        wait_flag.st |= READ_DMA_IS_DONE;
    } else if (event == DRV_EVENT_COMMON_WRITE_COMPLETED) {
        wait_flag.st |= SEND_INT_IS_DONE;
    } else if (event == DRV_EVENT_COMMON_GPDMA2PERIPH_COMPLETED) {
        wait_flag.st |= SEND_DMA_IS_DONE;
    }
}


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief example of sending and receiving in blocking mode
 *        1. send string "hello"
 *        2. block until 5 characters are received from user
 *        3. send the received data
 *******************************************************************************
 */
void example_uart_block(void)
{
    om_error_t error;

    drv_pin_init(pin_config, sizeof(pin_config) / sizeof(pin_config[0]));

    uart_config_t uart_cfg = {
        .baudrate       = UART_BAUDRATE,
        .flow_control   = UART_FLOW_CONTROL_NONE,
        .data_bit       = UART_DATA_BIT_8,
        .stop_bit       = UART_STOP_BIT_1,
        .parity         = UART_PARITY_NONE,
    };

    drv_uart_init(TEST_UART, &uart_cfg);

    memcpy(send_data, "hello\r\n", 7);
    drv_uart_write(TEST_UART, (uint8_t *)send_data, 7, 10);
    error = drv_uart_read(TEST_UART, (uint8_t *)rec_data, 5, 3000);
    if (error == OM_ERROR_TIMEOUT) {
        drv_uart_write(TEST_UART, (uint8_t *)"Read Timeout\n", 13, 10);
    }
    drv_uart_write(TEST_UART, (uint8_t *)rec_data, 5, 10);
}

/**
 *******************************************************************************
 * @brief example of sending and receiving in interrupt mode
 *        1. send string "hello"
 *        2. send what chip receives
 *
 *******************************************************************************
 */
void example_uart_int(void)
{
    drv_pin_init(pin_config, sizeof(pin_config) / sizeof(pin_config[0]));

    uart_config_t uart_cfg = {
        .baudrate       = UART_BAUDRATE,
        .flow_control   = UART_FLOW_CONTROL_NONE,
        .data_bit       = UART_DATA_BIT_8,
        .stop_bit       = UART_STOP_BIT_1,
        .parity         = UART_PARITY_NONE,
    };

    drv_uart_init(TEST_UART, &uart_cfg);
    drv_uart_register_isr_callback(TEST_UART, test_uart_cb);

    memcpy(send_data, "hello\r\n", 7);
    drv_uart_write_int(TEST_UART, (uint8_t *)send_data, 7);
    while (!(wait_flag.st & SEND_INT_IS_DONE));
    wait_flag.st &= ~SEND_INT_IS_DONE;

    drv_uart_read_int(TEST_UART, NULL, 0);
}

/**
 *******************************************************************************
 * @brief example of sending and receiving in dma mode
 *        1. send string "hello"
 *        2. when the user passes 5 characters, the chip will send those 5
 *           characters back
 *******************************************************************************
 */
void example_uart_dma(void)
{
    drv_pin_init(pin_config, sizeof(pin_config) / sizeof(pin_config[0]));

    uart_config_t uart_cfg = {
        .baudrate       = UART_BAUDRATE,
        .flow_control   = UART_FLOW_CONTROL_NONE,
        .data_bit       = UART_DATA_BIT_8,
        .stop_bit       = UART_STOP_BIT_1,
        .parity         = UART_PARITY_NONE,
    };

    drv_uart_init(TEST_UART, &uart_cfg);
    drv_uart_register_isr_callback(TEST_UART, test_uart_cb);

    drv_uart_gpdma_channel_allocate(TEST_UART, DRV_GPDMA_CHAN_ALL);

    memcpy(send_data, "hello\r\n", 7);
    drv_uart_write_dma(TEST_UART, (uint8_t *)send_data, 7);
    while (!(wait_flag.st & SEND_DMA_IS_DONE));
    wait_flag.st &= ~SEND_DMA_IS_DONE;

    drv_uart_read_dma(TEST_UART, (uint8_t *)rec_data, 5);
    while (!(wait_flag.st & READ_DMA_IS_DONE));
    wait_flag.st &= ~READ_DMA_IS_DONE;

    drv_uart_gpdma_channel_release(TEST_UART, DRV_GPDMA_CHAN_ALL);
}

/**
 *******************************************************************************
 * @brief example of sending and receiving in interrupt mode with flow control
 *        1. send string "hello"
 *        2. send what chip receives
 *
 *******************************************************************************
 */
void example_uart_flow_control(void)
{
    drv_pin_init(pin_config, sizeof(pin_config) / sizeof(pin_config[0]));

    uart_config_t uart_cfg = {
        .baudrate       = UART_BAUDRATE,
        .flow_control   = UART_FLOW_CONTROL_RTS_CTS,
        .data_bit       = UART_DATA_BIT_8,
        .stop_bit       = UART_STOP_BIT_1,
        .parity         = UART_PARITY_NONE,
    };

    drv_uart_init(TEST_UART, &uart_cfg);
    drv_uart_register_isr_callback(TEST_UART, test_uart_cb);

    memcpy(send_data, "hello\r\n", 7);
    drv_uart_write_int(TEST_UART, (uint8_t *)send_data, 7);
    while (!(wait_flag.st & SEND_INT_IS_DONE));
    wait_flag.st &= ~SEND_INT_IS_DONE;

    drv_uart_read_int(TEST_UART, NULL, 0);
}

/** @} */