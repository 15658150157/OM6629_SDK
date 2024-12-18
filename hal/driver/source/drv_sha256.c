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
 * @brief    SHA256 driver source file
 * @details  SHA256 driver source file
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
#if (RTE_SHA256)
#include <stddef.h>
#include "om_device.h"
#include "om_driver.h"


/*******************************************************************************
 * MACROS
 */
#define SHA256_BLOCK_SIZE       64

#define SHA256_WAIT_FOR_VALID()                                                \
    do {                                                                       \
        while (!(OM_SHA256->STATUS & SHA256_STATUS_VALID_MASK));               \
    } while (0)


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    uint32_t    data_byte_cnt;
    drv_state_t state;
} sha256_env_t;


/*******************************************************************************
 * CONST & VARIABLES
 */
static sha256_env_t sha256_env = {
    .data_byte_cnt = 0,
    .state         = DRV_STATE_STOP,
};


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
__STATIC_INLINE uint32_t endian_convert(uint32_t val)
{
    return (val & 0x000000FFU) << 24 |
           (val & 0x0000FF00U) << 8 |
           (val & 0x00FF0000U) >> 8 |
           (val & 0xFF000000U) >> 24;
}

static void sha256_start_block(void)
{
    if (sha256_env.state == DRV_STATE_START) {
        // Calculate First Block
        OM_SHA256->CTRL |= SHA256_CTRL_INIT_MASK;
        sha256_env.state = DRV_STATE_CONTINUE;
    } else {
        SHA256_WAIT_FOR_VALID();
        // Calculate Other Blocks
        OM_SHA256->CTRL |= SHA256_CTRL_NEXT_MASK;
    }
}


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief Start SHA256 calculation.
 *
 * @param[in] om_sha256     Pointer to SHA256 peripheral
 *******************************************************************************
 */
void drv_sha256_start(void)
{
    sha256_env.data_byte_cnt = 0;
    sha256_env.state = DRV_STATE_START;
    DRV_RCC_RESET((uint32_t)OM_SHA256);
}

/**
 *******************************************************************************
 * @brief Update SHA256 calculation.
 * NOTE:
 *   1. data is preferably 4-byte aligned
 *   2. when len < SHA256_BLOCK_SIZE, calculate digest directly
 *
 * @param[in] om_sha256     Pointer to SHA256 peripheral
 * @param[in] data          Data to be hashed
 * @param[in] len           Length of data to be hashed
 *******************************************************************************
 */
void drv_sha256_update(const uint8_t *data, uint32_t len)
{
    uint8_t i;

    OM_ASSERT(sha256_env.state == DRV_STATE_START || sha256_env.state == DRV_STATE_CONTINUE);
    OM_ASSERT(len <= SHA256_BLOCK_SIZE);

    if (len == SHA256_BLOCK_SIZE) {
        for (i = 0; i < SHA256_BLOCK_SIZE / 4; i++) {
            OM_SHA256->MSG[i] = ((uint32_t *)data)[i];
        }

        sha256_env.data_byte_cnt += SHA256_BLOCK_SIZE;
        sha256_start_block();
    } else {
        // 1. write block data into message register
        for (i = 0; i < (len / 4); i++) {
            OM_SHA256->MSG[i] = ((uint32_t *)data)[i];   /*lint !e2445 !e433*/
        }
        // write tail data that not aligned to word
        uint32_t tail_data = 0;
        for (int8_t j = len - 1; j >= (i * 4); j--) {
            tail_data = tail_data << 8;
            tail_data |= data[j];
        }
        OM_SHA256->MSG[i] = tail_data;

        sha256_env.data_byte_cnt += len;
        // add 1 at the end of data
        OM_SHA256->MSG[i] |= 0x80U << ((len & 0x03U) << 3);

        // 2. check if a new block with only length information is required
        if (len < 56) {
            // length information can be write in this block
            // add data length at the end of block
            OM_SHA256->MSG[15] = endian_convert(sha256_env.data_byte_cnt << 3);
        } else {
            // length information cannot be write in this block
            // start previous block
            sha256_start_block();
            // add data length at the end of block
            OM_SHA256->MSG[15] = endian_convert(sha256_env.data_byte_cnt << 3);
        }
        // 3. start last block
        sha256_start_block();
        // 4. change to the stop status
        sha256_env.state = DRV_STATE_STOP;
    }
}

/**
 *******************************************************************************
 * @brief Finish SHA256 calculation.
 *
 * @param[in]  om_sha256     Pointer to the SHA256 peripheral
 * @param[out] hash          Hash result
 *******************************************************************************
 */
void drv_sha256_finish(uint8_t hash[32])
{
    if (sha256_env.state != DRV_STATE_STOP) {
        OM_SHA256->MSG[0] = 0x80U;
        OM_SHA256->MSG[15] = endian_convert(sha256_env.data_byte_cnt << 3);
        sha256_start_block();
    }

    SHA256_WAIT_FOR_VALID();

    #if (RTE_SHA256_USING_BIG_ENDIAN)
    for (uint8_t i = 0; i < 8; i++) {
        ((uint32_t *)hash)[i] = OM_SHA256->DIG[i];
    }
    #else
    for (uint8_t i = 0; i < 8; i++) {
        ((uint32_t *)hash)[i] = endian_convert(OM_SHA256->DIG[7 - i]);
    }
    #endif

    DRV_RCC_CLOCK_ENABLE((uint32_t)OM_SHA256, 0);
}


#endif  /* (RTE_SHA256) */

/** @} */