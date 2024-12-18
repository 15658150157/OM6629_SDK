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
 * @brief    AUDIO driver source file
 * @details  AUDIO driver source file
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
#if (RTE_AUDIO_USE_INTERNAL)
#include "om_device.h"
#include "om_driver.h"


/*******************************************************************************
 * DEBUG DEFINES
 */
/// Audio Assert
#define AUDIO_ASSERT_EN         (1)

#if (AUDIO_ASSERT_EN)
#define AUDIO_ASSERT(x)         OM_ASSERT(x)
#else
#define AUDIO_ASSERT(x)
#endif /* AUDIO_ASSERT_EN */


/*******************************************************************************
 * TYPEDEFS
 */
/// Audio environment structure
typedef struct {
    audio_state_t state;
} audio_env_t;


/*******************************************************************************
 * CONST & VARIABLES
 */
/// Audio environment variable define
static audio_env_t audio_env = {
    .state      = AUDIO_STATE_IDLE
};

/// Audio resource variable define
static const drv_resource_t audio_resource = {
    .cap        = 0,    /* Not present */
    .reg        = (void *)OM_AUDIO,
    .env        = (void *)&audio_env,
    .irq_num    = AUDIO_IRQn,
    .irq_prio   = RTE_AUDIO_IRQ_PRIORITY,
};


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
void drv_audio_inside_init(void)
{
    audio_env_t *env = (audio_env_t *)audio_resource.env;

    /// Is inited?
    if(env->state != AUDIO_STATE_IDLE) {
        return;
    }

    /// TODO: Power on by PMU

    /// Reset Audio module and Enable clock
    DRV_RCC_RESET(RCC_CLK_AUDIO);

    /// Audio power on by fsm
    #if RTE_AUDIO_USE_ANA_MIC
    /// Enable Power on and Power off interrupt
    register_set(&OM_AUDIO->CODEC_INT_CTRL, MASK_2REG(AU_INT_CTRL_ON, 0, AU_INT_CTRL_OFF, 0));

    // 1. Power on
    register_set(&OM_AUDIO->CODEC_ANA_PWR_1, MASK_1REG(AU_ANA_PWR_ON_EN, 1));
    // 2. Wait for power on interrupt flag
    while((OM_AUDIO->CODEC_INT_STATUS & AU_INT_STATUS_ON_MASK) == 0);
    // 3. Write 1 to clear power on interrupt flag
    register_set(&OM_AUDIO->CODEC_INT_STATUS, MASK_1REG(AU_INT_STATUS_ON, 1));

    /// Disable DMIC
    register_set(&OM_AUDIO->ADC_CTRL, MASK_1REG(AU_CTRL_DMIC_EN, 0));

    #else

    /// Enable mute and unmute interrupt
    register_set(&OM_AUDIO->CODEC_INT_CTRL, MASK_2REG(AU_INT_CTRL_MUTE, 0, AU_INT_CTRL_UNMUTE, 0));

    /// Enable DMIC
    register_set(&OM_AUDIO->ADC_CTRL, MASK_1REG(AU_CTRL_DMIC_EN, 1));

    #endif /* RTE_AUDIO_USE_ANA_MIC */

    /// Enable DC offset
    register_set(&OM_AUDIO->ADC_CTRL, MASK_1REG(AU_CTRL_DC_EN, 1));

    /// Enable codec clock, i2s_sdi and i2s_sdo connecnt to internal codec
    register_set(&OM_AUDIO->CODEC_CLK_CTRL_1, MASK_2REG(AU_CLK_CTRL1_CLK_EN, 1, AU_CLK_I2S_CONN_CTRL, 3));

    /// Clear and Disable Audio interrupt
    NVIC_ClearPendingIRQ(AUDIO_IRQn);
    NVIC_SetPriority(AUDIO_IRQn, RTE_AUDIO_IRQ_PRIORITY);
    NVIC_DisableIRQ(AUDIO_IRQn);

    /// Set inited state
    env->state = AUDIO_STATE_INITED;
}

void drv_audio_inside_uninit(void)
{
    audio_env_t *env = (audio_env_t *)audio_resource.env;

    /// Is uninited?
    if(env->state == AUDIO_STATE_UNINITED) {
        return;
    }

    /// If working, stop first
    if(env->state > AUDIO_STATE_INITED) {
        return;
    }

    /// Set uninit state
    env->state = AUDIO_STATE_UNINITED;

    /// Disable codec clock, i2s_sdi and i2s_sdo reset to default
    register_set(&OM_AUDIO->CODEC_CLK_CTRL_1, MASK_2REG(AU_CLK_CTRL1_CLK_EN, 0, AU_CLK_I2S_CONN_CTRL, 0));

    /// Audio power down by fsm
    #if RTE_AUDIO_USE_ANA_MIC
    // 1. Set codec power off
    register_set(&OM_AUDIO->CODEC_ANA_PWR_1, MASK_1REG(AU_ANA_PWR_ON_EN, 0));
    // 2. Wait for power off interrupt flag
    while((OM_AUDIO->CODEC_INT_STATUS & AU_INT_STATUS_OFF_MASK) == 0);
    // 3. Write 1 to clear power off interrupt flag
    register_set(&OM_AUDIO->CODEC_INT_STATUS, MASK_1REG(AU_INT_STATUS_OFF, 1));
    #endif

    /// Gate audio clock
    DRV_RCC_CLOCK_ENABLE(RCC_CLK_AUDIO, 0);

    /// TODO: Power down by pmu
}

om_error_t drv_audio_inside_record_start(audio_record_config_t *config)
{
    audio_env_t *env = (audio_env_t *)audio_resource.env;

    AUDIO_ASSERT(config);
    AUDIO_ASSERT(config->channel == I2S_CHN_MONO);
    AUDIO_ASSERT(config->bit_width == I2S_BW_16BIT || config->bit_width == I2S_BW_24BIT);
    AUDIO_ASSERT(config->sample_rate == I2S_SR_8K || config->sample_rate == I2S_SR_16K || config->sample_rate == I2S_SR_32K);

    /// Is instanced?
    if(env->state & AUDIO_STATE_RECORD) {
        return OM_ERROR_STATUS;
    }

    /// Inited first
    if((env->state & AUDIO_STATE_INITED) == 0) {
        return OM_ERROR_STATUS;
    }

    env->state |= AUDIO_STATE_RECORD;

    /// Reset filter
    register_set(&OM_AUDIO->ADC_CTRL,MASK_1REG(AU_CTRL_SW_RESET_X, 1));
    register_set(&OM_AUDIO->CODEC_CLK_CTRL_1, MASK_1REG(AU_CLK_CTRL1_RSTN, 1));

    /// Wait unmute flow finish
    while((OM_AUDIO->CODEC_INT_STATUS & AU_INT_STATUS_UNMUTE_MASK) != AU_INT_STATUS_UNMUTE_MASK);

    /// When output is clipping, gain decreased automatically
    register_set(&OM_AUDIO->ADC_CTRL, MASK_1REG(AU_CTRL_ANTI_CLIP, 1));
    /// 7 means 0x7FF0，6 means 0x6ff0...
    register_set(&OM_AUDIO->ADC_THD_CTRL, MASK_1REG(AU_CLIP_THD_CTRL, 6));

    /// Set sample rete
    switch (config->sample_rate) {
        case I2S_SR_8K:
        /// 8K select 2M
        register_set(&OM_AUDIO->CODEC_CLK_CTRL_1, MASK_1REG( AU_CLK_CTRL1_DMIC_SEL, 0));
        register_set(&OM_AUDIO->ADC_CTRL, MASK_3REG(AU_CTRL_SR_4X, 1, AU_CTRL_SR_2X, 0, AU_CTRL_SR_1X, 0));
        break;

        case I2S_SR_16K:
        /// 8K select 2M
        register_set(&OM_AUDIO->CODEC_CLK_CTRL_1, MASK_1REG( AU_CLK_CTRL1_DMIC_SEL, 0));
        register_set(&OM_AUDIO->ADC_CTRL, MASK_3REG(AU_CTRL_SR_4X, 0, AU_CTRL_SR_2X, 1, AU_CTRL_SR_1X, 0));
        break;

        case I2S_SR_32K:
        /// 8K select 3.2M
        register_set(&OM_AUDIO->CODEC_CLK_CTRL_1, MASK_1REG( AU_CLK_CTRL1_DMIC_SEL, 1));
        register_set(&OM_AUDIO->ADC_CTRL, MASK_3REG(AU_CTRL_SR_4X, 0, AU_CTRL_SR_2X, 0, AU_CTRL_SR_1X, 1));
        break;

        default:
        AUDIO_ASSERT(0);
        break;
    }

    #if RTE_AUDIO_USE_ANA_MIC
    ///TODO set adc and pga gain
    #endif /* RTE_AUDIO_USE_ANA_MIC */

    /// Set extra digist gain
    register_set(&OM_AUDIO->ADC_CTRL, MASK_1REG(AU_CTRL_CIC_SCALE, config->gain_cic));

    /// Set volume
    register_set(&OM_AUDIO->ADC_VOL_CTRL, MASK_5REG(AU_VOL_UNMUTE_RATE, 5,
                                                    AU_VOL_MUTE_RATE,   5,
                                                    AU_VOL_ADCUNMU,     0,
                                                    AU_VOL_ADCMU,       0,
                                                    AU_VOL_MUTE_BYPASS, 1));

    register_set(&OM_AUDIO->ADC_VOL_CTRL, MASK_2REG(AU_VOL_LADC, config->volume, AU_VOL_UPDATE, 1));
    while(OM_AUDIO->ADC_VOL_CTRL & AU_VOL_UPDATE_MASK);

    /// Enable adc
    register_set(&OM_AUDIO->ADC_CTRL, MASK_1REG(AU_CTRL_EN, 1));

    return OM_ERROR_OK;
}

om_error_t drv_audio_inside_record_stop(void)
{
    audio_env_t *env = (audio_env_t *)audio_resource.env;

    /// Is stopped?
    if((env->state & AUDIO_STATE_RECORD) == 0) {
        return OM_ERROR_STATUS;
    }

    /// ADC off and clock gated
    register_set(&OM_AUDIO->ADC_CTRL, MASK_1REG(AU_CTRL_EN, 0));

    /// Update state
    env->state &= (~AUDIO_STATE_RECORD);

    return OM_ERROR_OK;
}

audio_state_t drv_audio_inside_work_state(void)
{
    audio_env_t *env = (audio_env_t *)audio_resource.env;
    return env->state;
}


#endif /* RTE_AUDIO_USE_INTERNAL */

/** @} */