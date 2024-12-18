/* ----------------------------------------------------------------------------
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 * -------------------------------------------------------------------------- */

/**
 * @defgroup ECDSA ECDSA
 * @ingroup  DRIVER
 * @brief    virtual ECDSA driver
 * @details  virtual ECDSA driver apis and typedefs header file
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

#ifndef __DRV_ECDSA_H
#define __DRV_ECDSA_H


/*******************************************************************************
 * INCLUDES
 */
#include "RTE_driver.h"
#if (RTE_ECDSA)
#include <stdint.h>
#include "om_driver.h"

#ifdef __cplusplus
extern "C"
{
#endif


/*******************************************************************************
 * TYPEDEFS
 */
/// Ecdsa configuration
typedef struct {
    // Random seed for ECDSA
    uint32_t random_seed;
} ecdsa_config_t;


/*******************************************************************************
 * EXTERN FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief ECDSA driver initialization
 *
 * @param[in] cfg  Pointer to ecdsa configuration
 *******************************************************************************
 */
extern void drv_ecdsa_init(ecdsa_config_t *cfg);

/**
 *******************************************************************************
 * @brief Generate ECDSA public key from private key
 *
 * @param[in]  private_key   Pointer to private key, 4-byte alignment is required
 * @param[out] public_key    Pointer to generated public key, 4-byte alignment is required
 *
 * @return om_error
 *******************************************************************************
 */
extern om_error_t drv_ecdsa_generate_public_key(const void *private_key, void *public_key);

/**
 *******************************************************************************
 * @brief Generate ECDSA private key and public key
 *
 * @param[out] private_key   Pointer to generated private key, 4-byte alignment is required
 * @param[out] public_key    Pointer to generated public key, 4-byte alignment is required
 *
 * @return om_error
 *******************************************************************************
 */
extern om_error_t drv_ecdsa_generate_key_pair(void *private_key, void *public_key);

/**
 *******************************************************************************
 * @brief ECDSA sign
 *
 * @param[in] private_key   Pointer to private key, 4-byte alignment is required
 * @param[in] msg_hash      Pointer to message hash
 * @param[in] hash_size     Message hash size
 * @param[out] signature    Pointer to generated signature
 *
 * @return om_error
 *******************************************************************************
 */
extern om_error_t drv_ecdsa_sign(const void *private_key, const void *msg, uint32_t msg_len, void *signature);

/**
 *******************************************************************************
 * @brief ECDSA verify
 *
 * @param[in] public_key    Pointer to public key, 4-byte alignment is required
 * @param[in] msg_hash      Pointer to message hash
 * @param[in] hash_size     Message hash size
 * @param[in] signature     Pointer to signature
 *
 * @return om_error
 *******************************************************************************
 */
extern om_error_t drv_ecdsa_verify(const void *public_key, const void *msg, uint32_t msg_len, const void *signature);


#endif  /* (RTE_ECDSA) */

#endif  /* __DRV_ECDSA_H */

/** @} */
