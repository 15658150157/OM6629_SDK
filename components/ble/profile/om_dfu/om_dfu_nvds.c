/* ----------------------------------------------------------------------------
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 * -------------------------------------------------------------------------- */

/**
 * @version
 * Version V20210119.1.0
 *  - Initial release
 *
 * @{
 */

/*******************************************************************************
 * INCLUDES
 */
#include <string.h>
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "om_dfu.h"
#include "om_dfu_nvds.h"

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static uint8_t om_dfu_nvds_enable_dummy(void)
{
    return OM_DFU_NVDS_ST_SUCCESS;
}
static uint8_t om_dfu_nvds_get_dummy(uint32_t id, uint32_t *lengthPtr, void *buf)
{
    memset(buf, 0xFF, *lengthPtr);
    return OM_DFU_NVDS_ST_SUCCESS;
}
static uint8_t om_dfu_nvds_put_dummy(uint32_t id, uint32_t length, void *buf)
{
    return OM_DFU_NVDS_ST_SUCCESS;
}
static uint8_t om_dfu_nvds_del_dummy(uint32_t id, uint32_t length)
{
    return OM_DFU_NVDS_ST_SUCCESS;
}
static uint8_t om_dfu_nvds_disable_dummy(void)
{
    return OM_DFU_NVDS_ST_SUCCESS;
}

const dfu_nvds_itf_t dfu_nvds_itf[] = {
    { //DFU_NVDS_ITF_TYPE_MBR,
        om_dfu_nvds_enable_dummy,
        om_dfu_nvds_get_dummy,
        om_dfu_nvds_put_dummy,
        om_dfu_nvds_del_dummy,
        om_dfu_nvds_disable_dummy
    },
    { //DFU_NVDS_ITF_TYPE_FLASH,
        om_dfu_nvds_enable_dummy,
        om_dfu_nvds_get_dummy,
        om_dfu_nvds_put_dummy,
        om_dfu_nvds_del_dummy,
        om_dfu_nvds_disable_dummy
    },
    { //DFU_NVDS_ITF_TYPE_CFG,
        om_dfu_nvds_enable_dummy,
        om_dfu_nvds_get_dummy,
        om_dfu_nvds_put_dummy,
        om_dfu_nvds_del_dummy,
        om_dfu_nvds_disable_dummy
    },
    { //DFU_NVDS_ITF_TYPE_EXT_FLASH,
        om_dfu_nvds_enable_dummy,
        om_dfu_nvds_get_dummy,
        om_dfu_nvds_put_dummy,
        om_dfu_nvds_del_dummy,
        om_dfu_nvds_disable_dummy
    },
    { //DFU_NVDS_ITF_TYPE_DUMMY,
        om_dfu_nvds_enable_dummy,
        om_dfu_nvds_get_dummy,
        om_dfu_nvds_put_dummy,
        om_dfu_nvds_del_dummy,
        om_dfu_nvds_disable_dummy
    },
};

/** @} */

