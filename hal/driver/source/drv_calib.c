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
 * @brief    CALIB driver source file
 * @details  CALIB driver source file
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
#if (RTE_CALIB)
#include <stddef.h>
#include "om_driver.h"


/*******************************************************************************
 * TYPEDEFS
 */
typedef struct {
    bool rc32k_is_calibed;

    // Saved calibed data
    uint8_t icon_vco;
} drv_calib_t;


/*******************************************************************************
 * LOCAL VARIABLES
 */
static drv_calib_t drv_calib_env = {
    .rc32k_is_calibed = false
};


/*******************************************************************************
 * LOCAL FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief  calib patch
 *
 * @param[in] temperature  temperature
 *******************************************************************************
 **/
static void drv_calib_patch(bool is_calib_start)
{
    if (is_calib_start) {
        // FIX RF TX issue,增大PA开启后等待时间，避免PLL频率漂移
        REGW(&OM_DAIF->MAIN_ST_CFG0, MASK_1REG(DAIF_TXLDO_WAIT, 0x180));
        //默认值，不需要修改
        //REGW(&OM_DAIF->IR_RX_CFG, MASK_1REG(DAIF_WIN_CNT_THRESHOLD, 0xC000));
        //增大模拟调制增益
        REGW(&OM_DAIF->PLL_CTRL2, MASK_1REG(DAIF_EN_KVCO2, 3U));

        ///射频起来之前
        //增大RF电流，提高灵敏度
        REGW(&OM_DAIF->LNA_MIX_CFG, MASK_3REG(DAIF_RF_CONSTGM_CT,0x0,DAIF_RF_LNA_VBCT,0x3,DAIF_RF_MIX_VBCT,0x3));
        //提高TIA增益，提高灵敏度
        REGW(&OM_DAIF->TIA_DCOC_CFG, MASK_2REG(DAIF_TIA_GAIN_ME,1,DAIF_TIA_GAIN_MO,3));
        //提高RF电源电压增益，提高灵敏度，固定IFLDO电压
        REGW(&OM_PMU->ANA_REG, MASK_2REG(PMU_ANA_REG_RF_LDO_TRIM,3, PMU_ANA_REG_IF_LDO_TRIM, 2U));
        //better for TX drift
        REGW(&OM_DAIF->PLL_CTRL0, MASK_2REG(DAIF_BP_DIOX,0, DAIF_SEL_KVCO,3));
        drv_calib_repair_rf_pll_temperature_repair(true);

    } else { // FIX RF TX issue
        // save icon_vco
        OM_DAIF->DBG_REG = 0x17;
        drv_calib_env.icon_vco = (OM_DAIF->DBG_REG >> 25) & 0x7;
        OM_DAIF->DBG_REG = 0;
        //ramp的第二种模式,解决旁瓣杂散问题;
        REGW1(&OM_DAIF->TRX_SW_CFG, DAIF_PA_PATTERN_SEL_MASK);
        //增大PA开启后等待时间，避免PLL频率漂移
        REGW(&OM_DAIF->MAIN_ST_CFG0, MASK_1REG(DAIF_TXLDO_WAIT, 0x180));
        //pkd常开，避免对信号干扰
        REGW(&OM_DAIF->PD_CFG0, MASK_2REG(DAIF_PKDT_PD_ME,1, DAIF_PKDT_PD_MO,0));
        ///抗干扰策略2;
        REGWA(&OM_DAIF->FILT_AGC_LUT_REG0, MASK_3REG(DAIF_FILT_AGC_LUT_0, 0x104, DAIF_FILT_AGC_LUT_1, 0x105, DAIF_FILT_AGC_LUT_2, 0x108));
        REGWA(&OM_DAIF->FILT_AGC_LUT_REG1, MASK_3REG(DAIF_FILT_AGC_LUT_3, 0x10B, DAIF_FILT_AGC_LUT_4, 0x110, DAIF_FILT_AGC_LUT_5, 0x116));
        REGWA(&OM_DAIF->FILT_AGC_LUT_REG2, MASK_3REG(DAIF_FILT_AGC_LUT_6, 0x120, DAIF_FILT_AGC_LUT_7, 0x12C, DAIF_FILT_AGC_LUT_8, 0x140));

        // restore code
        drv_calib_rf_restore();
        drv_calib_repair_rf_pll_temperature_repair(false);
    }
}

/**
 *******************************************************************************
 * @brief  Auto frequency calibration
 *******************************************************************************
 */
static void drv_calib_rf_afc(void)
{
    //initial afc/vtrack table
    for(int i=0; i<80; ++i) {
        REGW(&OM_DAIF->PLL_LUT_DBG, MASK_3REG( DAIF_PLL_LUT_DATA, 0x1124,
                                               DAIF_PLL_LUT_IDX, i,
                                               DAIF_PLL_LUT_WR, 1));
        // wait lut wr done
        while(OM_DAIF->PLL_LUT_DBG & DAIF_PLL_LUT_WR_MASK);
    }

    // do amp cali
    REGW1(&OM_DAIF->MIX_CFG1, DAIF_PEAKDET_START_MASK);
    // wait amp done
    while(OM_DAIF->MIX_CFG1 & DAIF_PEAKDET_START_MASK);

    //校准kdco前是0x0，校准kdco之后寄存器写为0x2;
    REGW(&OM_DAIF->PLL_CTRL0, MASK_1REG(DAIF_SEL_AFC_VC, 0x2));
    //VCO的电流大小，FT和CP会进行电流大小校准
    REGW(&OM_DAIF->PLL_CTRL1, MASK_1REG(DAIF_RDPLL_SEL_VCO_IBIAS, drv_calib_repair_env.vco_cur));

    //sw power on analog blocks
    // power on pll analog
    REGW(&OM_DAIF->PD_CFG1, MASK_4REG(DAIF_RFPLL_PD_ALL_ME,1, DAIF_RFPLL_PD_ALL_MO,0, DAIF_RFPLL_PD_VCDET_ME,1, DAIF_RFPLL_PD_VCDET_MO,0));
    REGW(&OM_DAIF->PD_CFG2, MASK_2REG(DAIF_RFPLL_PD_TXDAC_MO,0, DAIF_RFPLL_PD_TXDAC_ME,1));

    // digital ctrl
    REGW(&OM_DAIF->VCO_CTRL0, MASK_1REG(DAIF_AFC_EN_ME,0));

    // FIXME: Digital BUG: 2M issue
    REGW(&OM_DAIF->PLL_AFC_CTRL, MASK_2REG(DAIF_PLL_AFC_START_FREQ,2358, DAIF_PLL_AFC_STOP_FREQ,2512));
    //do vco afc
    REGW1(&OM_DAIF->VCO_CTRL0, DAIF_AFC_START_MASK);
    // wait afc done
    while(OM_DAIF->VCO_CTRL0 & DAIF_AFC_START_MASK);

    //pd_lotx power on
    REGW(&OM_DAIF->PD_CFG1, MASK_2REG(DAIF_RFPLL_PD_LOTX_MO,0, DAIF_RFPLL_PD_LOTX_ME,1));

    // KDCO
    if (drv_calib_repair_env.kdco_lut_1m >= 0) {
        REGW(&OM_DAIF->KDCO_LUT_1M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_0,  drv_calib_repair_env.kdco_lut_1m+2));
        REGW(&OM_DAIF->KDCO_LUT_1M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_1,  drv_calib_repair_env.kdco_lut_1m+1));
        REGW(&OM_DAIF->KDCO_LUT_1M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_2,  drv_calib_repair_env.kdco_lut_1m+1));
        REGW(&OM_DAIF->KDCO_LUT_1M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_3,  drv_calib_repair_env.kdco_lut_1m+1));
        REGW(&OM_DAIF->KDCO_LUT_1M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_4,  drv_calib_repair_env.kdco_lut_1m+0));
        REGW(&OM_DAIF->KDCO_LUT_1M_REG1, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_5,  drv_calib_repair_env.kdco_lut_1m+0)); //2440  1M
        REGW(&OM_DAIF->KDCO_LUT_1M_REG1, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_6,  drv_calib_repair_env.kdco_lut_1m+0)); //2448  1M
        REGW(&OM_DAIF->KDCO_LUT_1M_REG1, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_7,  drv_calib_repair_env.kdco_lut_1m+0));
        REGW(&OM_DAIF->KDCO_LUT_1M_REG1, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_8,  drv_calib_repair_env.kdco_lut_1m-1));
        REGW(&OM_DAIF->KDCO_LUT_1M_REG1, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_9,  drv_calib_repair_env.kdco_lut_1m-1));
        REGW(&OM_DAIF->KDCO_LUT_2M_REG1, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_10, drv_calib_repair_env.kdco_lut_1m-2));
        REGW(&OM_DAIF->KDCO_LUT_2M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_2M_0,  drv_calib_repair_env.kdco_lut_2m+1));
        REGW(&OM_DAIF->KDCO_LUT_2M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_2M_1,  drv_calib_repair_env.kdco_lut_2m+1));
        REGW(&OM_DAIF->KDCO_LUT_2M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_2M_2,  drv_calib_repair_env.kdco_lut_2m+0));
        REGW(&OM_DAIF->KDCO_LUT_2M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_2M_3,  drv_calib_repair_env.kdco_lut_2m+0));
        REGW(&OM_DAIF->KDCO_LUT_2M_REG1, MASK_1REG_SSAT(DAIF_KDCO_LUT_2M_4,  drv_calib_repair_env.kdco_lut_2m-1));
        REGW(&OM_DAIF->KDCO_LUT_2M_REG1, MASK_1REG_SSAT(DAIF_KDCO_LUT_2M_5,  drv_calib_repair_env.kdco_lut_2m-1));
        REGW(&OM_DAIF->PLL_CTRL1,        MASK_1REG(DAIF_CON_BIAS_IDAC_PLL,   drv_calib_repair_env.con_bias_idac_pll));
    } else {
        #if 1  //KDCO校准开始
        //do kdco -- 1M
        REGW(&OM_DAIF->VCO_CTRL1, MASK_2REG(DAIF_BLE_1M_2M_SEL_MO,0, DAIF_BLE_1M_2M_SEL_ME,1));
        // try kdco 1M mode
        REGW1(&OM_DAIF->VCO_CTRL0, DAIF_KDCO_START_MASK);
        //wait kdco done
        while(OM_DAIF->VCO_CTRL0 & DAIF_KDCO_START_MASK);
        // FIXME: Digital BUG: 2M issue
        REGW(&OM_DAIF->PLL_AFC_CTRL, MASK_2REG(DAIF_PLL_AFC_START_FREQ,2406, DAIF_PLL_AFC_STOP_FREQ,2476));
        //do kdco -- 2M
        REGW(&OM_DAIF->VCO_CTRL1, MASK_2REG(DAIF_BLE_1M_2M_SEL_MO,1, DAIF_BLE_1M_2M_SEL_ME,1));
        // try kdco 2M mode
        REGW1(&OM_DAIF->VCO_CTRL0, DAIF_KDCO_START_MASK);
        //wait kdco done
        while(OM_DAIF->VCO_CTRL0 & DAIF_KDCO_START_MASK);

        //校准kdco前是0x0，校准kdco之后寄存器写为0x2;
        REGW(&OM_DAIF->PLL_CTRL0, MASK_1REG(DAIF_SEL_AFC_VC, 0x2));
        #endif //KDCO校准结束
        #if 1
        ///DRV_RCC_CLOCK_ENABLE(RCC_CLK_DAIF, 1U);
        int16_t FT_KDCO_LUT_1M_5 = REGR(&OM_DAIF->KDCO_LUT_1M_REG1, DAIF_KDCO_LUT_1M_5_MASK, DAIF_KDCO_LUT_1M_5_POS);
        int16_t FT_BIAS_IDAC_PLL = REGR(&OM_DAIF->PLL_CTRL1, DAIF_CON_BIAS_IDAC_PLL_MASK, DAIF_CON_BIAS_IDAC_PLL_POS);
        int16_t FT_KDCO_LUT_2M_2 = REGR(&OM_DAIF->KDCO_LUT_2M_REG0, DAIF_KDCO_LUT_2M_2_MASK, DAIF_KDCO_LUT_2M_2_POS);
        FT_KDCO_LUT_1M_5 += 7;
        if (FT_KDCO_LUT_1M_5 < 0) {
            FT_KDCO_LUT_1M_5 = 0;
        } else if (FT_KDCO_LUT_1M_5 > 63) {
            FT_KDCO_LUT_1M_5 = 63;
        }
        FT_BIAS_IDAC_PLL += 3;
        if (FT_BIAS_IDAC_PLL < 0) {
            FT_BIAS_IDAC_PLL = 0;
        } else if (FT_BIAS_IDAC_PLL > 63) {
            FT_BIAS_IDAC_PLL = 63;
        }
        FT_KDCO_LUT_2M_2 += 8;//8=516  7=495.5
        if (FT_KDCO_LUT_2M_2 < 0) {
            FT_KDCO_LUT_2M_2 = 0;
        } else if (FT_KDCO_LUT_2M_2 > 63) {
            FT_KDCO_LUT_2M_2 = 63;
        }

        /*----------------------------------------1M------------------------------------*/
        int16_t ch0_1m, ch1_1m, ch2_1m, ch3_1m, ch4_1m, ch5_1m, ch6_1m, ch7_1m, ch8_1m, ch9_1m, ch10_1m;
        ch5_1m = FT_KDCO_LUT_1M_5;
        //1m_ch0: 1m_ch5 + 2
        ch0_1m = ch5_1m + 2;
        if (ch0_1m > 63) {
            ch0_1m = 63;
        }
        //1m_ch1: 1m_ch5 + 1
        //1m_ch2: 1m_ch5 + 1
        //1m_ch3: 1m_ch5 + 1
        ch1_1m = ch5_1m + 1;
        if (ch1_1m > 63) {
            ch1_1m = 63;
        }
        ch2_1m = ch1_1m;
        ch3_1m = ch1_1m;
        //1m_ch4: 1m_ch5 + 0
        //1m_ch5: 1m_ch5 + 0
        //1m_ch6: 1m_ch5 + 0
        //1m_ch7: 1m_ch5 + 0
        ch4_1m = ch5_1m + 0;
        ch6_1m = ch4_1m;
        ch7_1m = ch4_1m;
        //1m_ch8: 1m_ch5 - 1
        //1m_ch9: 1m_ch5 - 1
        ch8_1m = ch5_1m - 1;
        if (ch8_1m < 0) {
            ch8_1m = 0;
        }
        ch9_1m = ch8_1m;
        //1m_ch10: 1m_ch5 - 2
        ch10_1m = ch5_1m - 2;
        if (ch10_1m < 0) {
            ch10_1m = 0;
        }

        /*----------------------------------------2M------------------------------------*/
        int16_t ch0_2m, ch1_2m, ch2_2m, ch3_2m, ch4_2m, ch5_2m;
        ch2_2m = FT_KDCO_LUT_2M_2;
        //2m_ch0: 2m_2 + 1
        //2m_ch1: 2m_2 + 1
        ch0_2m = ch2_2m + 1;
        if (ch0_2m > 63) {
            ch0_2m = 63;
        }
        ch1_2m = ch0_2m;
        //2m_ch2: 2m_2 + 0
        //2m_ch3: 2m_2 + 0
        ch3_2m = ch2_2m;
        //2m_ch4: 2m_2 - 1
        //2m_ch5: 2m_2 - 1
        ch4_2m = ch2_2m - 1;
        if (ch4_2m < 0) {
            ch4_2m = 0;
        }
        ch5_2m = ch4_2m;
        REGW(&OM_DAIF->KDCO_LUT_1M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_0,  ch0_1m));
        REGW(&OM_DAIF->KDCO_LUT_1M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_1,  ch1_1m));
        REGW(&OM_DAIF->KDCO_LUT_1M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_2,  ch2_1m));
        REGW(&OM_DAIF->KDCO_LUT_1M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_3,  ch3_1m));
        REGW(&OM_DAIF->KDCO_LUT_1M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_4,  ch4_1m));
        REGW(&OM_DAIF->KDCO_LUT_1M_REG1, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_5,  ch5_1m)); //2440  1M
        REGW(&OM_DAIF->KDCO_LUT_1M_REG1, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_6,  ch6_1m)); //2448  1M
        REGW(&OM_DAIF->KDCO_LUT_1M_REG1, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_7,  ch7_1m));
        REGW(&OM_DAIF->KDCO_LUT_1M_REG1, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_8,  ch8_1m));
        REGW(&OM_DAIF->KDCO_LUT_1M_REG1, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_9,  ch9_1m));
        REGW(&OM_DAIF->KDCO_LUT_2M_REG1, MASK_1REG_SSAT(DAIF_KDCO_LUT_1M_10, ch10_1m));
        REGW(&OM_DAIF->KDCO_LUT_2M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_2M_0,  ch0_2m));
        REGW(&OM_DAIF->KDCO_LUT_2M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_2M_1,  ch1_2m));
        REGW(&OM_DAIF->KDCO_LUT_2M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_2M_2,  ch2_2m));
        REGW(&OM_DAIF->KDCO_LUT_2M_REG0, MASK_1REG_SSAT(DAIF_KDCO_LUT_2M_3,  ch3_2m));
        REGW(&OM_DAIF->KDCO_LUT_2M_REG1, MASK_1REG_SSAT(DAIF_KDCO_LUT_2M_4,  ch4_2m));
        REGW(&OM_DAIF->KDCO_LUT_2M_REG1, MASK_1REG_SSAT(DAIF_KDCO_LUT_2M_5,  ch5_2m));
        REGW(&OM_DAIF->PLL_CTRL1,        MASK_1REG(DAIF_CON_BIAS_IDAC_PLL,   FT_BIAS_IDAC_PLL));
        #endif
    }
    // restore 1m/2m FSM ctrl
    REGW(&OM_DAIF->VCO_CTRL1, MASK_1REG(DAIF_BLE_1M_2M_SEL_ME,0));
    //make analog blocks power controlled by trx FSM
    // change power control by FSM
    //pd_pll_all
    REGW(&OM_DAIF->PD_CFG1, MASK_2REG(DAIF_RFPLL_PD_ALL_ME,0, DAIF_RFPLL_PD_VCDET_ME,0));
    REGW(&OM_DAIF->PD_CFG2, MASK_1REG(DAIF_RFPLL_PD_TXDAC_ME,0));
    //pd_lotx
    REGW(&OM_DAIF->PD_CFG1, MASK_1REG(DAIF_RFPLL_PD_LOTX_ME,0));
}

/**
 *******************************************************************************
 * @brief  drv calib rf dcoc
 *******************************************************************************
 */
static void drv_calib_rf_dcoc(void)
{
    //do dcoc calib
    REGW1(&OM_DAIF->TIA_CR1, DAIF_TIA_START_MASK);
    // fix bug: When CE is set to high level, resetting the CPU may cause the chip to fail drv_calib_rf_dcoc () calibration.
    REGW(&OM_DAIF->VCO_CTRL0, MASK_3REG(DAIF_TRX_DBG,1, DAIF_RX_EN_MO,0, DAIF_TX_EN_MO,0));
    REGW(&OM_DAIF->VCO_CTRL0, MASK_3REG(DAIF_TRX_DBG,1, DAIF_RX_EN_MO,1, DAIF_TX_EN_MO,0));
    // wait dcoc done
    while(OM_DAIF->TIA_CR1 & DAIF_TIA_START_MASK);
    // fix bug: When CE is set to high level, resetting the CPU may cause the chip to fail drv_calib_rf_dcoc () calibration.
    REGW(&OM_DAIF->VCO_CTRL0, MASK_1REG(DAIF_TRX_DBG,0));
}

/**
 *******************************************************************************
 * @brief  drv calib rf txfe
 *******************************************************************************
 */
static void drv_calib_rf_txfe(void)
{
    REGW(&OM_DAIF->VCO_CTRL0, MASK_3REG(DAIF_TRX_DBG,1, DAIF_RX_EN_MO,0, DAIF_TX_EN_MO,1));
    DRV_DELAY_US(70);
    REGW(&OM_DAIF->PA_GAIN_REG_1, MASK_6REG(DAIF_PA_GAIN_1_DRV0_NCT, 16, DAIF_PA_GAIN_1_DRV0_PCT, 16, DAIF_PA_GAIN_2_DRV0_NCT, 16,
                                            DAIF_PA_GAIN_2_DRV0_PCT, 16, DAIF_PA_GAIN_1_DRIV_NUM, 0,  DAIF_PA_GAIN_2_DRIV_NUM, 1));
    REGW(&OM_DAIF->PA_GAIN_REG_2, MASK_6REG(DAIF_PA_GAIN_1_DRV0_NCT, 16, DAIF_PA_GAIN_1_DRV0_PCT, 16, DAIF_PA_GAIN_2_DRV0_NCT, 16,
                                            DAIF_PA_GAIN_2_DRV0_PCT, 16, DAIF_PA_GAIN_1_DRIV_NUM, 4,  DAIF_PA_GAIN_2_DRIV_NUM, 6));
    REGW(&OM_DAIF->PA_GAIN_REG_3, MASK_6REG(DAIF_PA_GAIN_1_DRV0_NCT, 16, DAIF_PA_GAIN_1_DRV0_PCT, 16, DAIF_PA_GAIN_2_DRV0_NCT, 16,
                                            DAIF_PA_GAIN_2_DRV0_PCT, 16, DAIF_PA_GAIN_1_DRIV_NUM, 11, DAIF_PA_GAIN_2_DRIV_NUM, 13));
    REGW(&OM_DAIF->PA_GAIN_REG_4, MASK_6REG(DAIF_PA_GAIN_1_DRV0_NCT, 16, DAIF_PA_GAIN_1_DRV0_PCT, 16, DAIF_PA_GAIN_2_DRV0_NCT, 16,
                                            DAIF_PA_GAIN_2_DRV0_PCT, 16, DAIF_PA_GAIN_1_DRIV_NUM, 14, DAIF_PA_GAIN_2_DRIV_NUM, 16));
    REGW(&OM_DAIF->PA_GAIN_REG_5, MASK_6REG(DAIF_PA_GAIN_1_DRV0_NCT, 16, DAIF_PA_GAIN_1_DRV0_PCT, 16, DAIF_PA_GAIN_2_DRV0_NCT, 16,
                                            DAIF_PA_GAIN_2_DRV0_PCT, 16, DAIF_PA_GAIN_1_DRIV_NUM, 18, DAIF_PA_GAIN_2_DRIV_NUM, 20));
    REGW(&OM_DAIF->PA_GAIN_REG_6, MASK_6REG(DAIF_PA_GAIN_1_DRV0_NCT, 16, DAIF_PA_GAIN_1_DRV0_PCT, 16, DAIF_PA_GAIN_2_DRV0_NCT, 16,
                                            DAIF_PA_GAIN_2_DRV0_PCT, 16, DAIF_PA_GAIN_1_DRIV_NUM, 23, DAIF_PA_GAIN_2_DRIV_NUM, 27));
    REGW(&OM_DAIF->PA_GAIN_REG_7, MASK_6REG(DAIF_PA_GAIN_1_DRV0_NCT, 16, DAIF_PA_GAIN_1_DRV0_PCT, 16, DAIF_PA_GAIN_2_DRV0_NCT, 16,
                                            DAIF_PA_GAIN_2_DRV0_PCT, 16, DAIF_PA_GAIN_1_DRIV_NUM, 31, DAIF_PA_GAIN_2_DRIV_NUM, 38));
    REGW(&OM_DAIF->PA_GAIN_REG_8, MASK_6REG(DAIF_PA_GAIN_1_DRV0_NCT, 16, DAIF_PA_GAIN_1_DRV0_PCT, 16, DAIF_PA_GAIN_2_DRV0_NCT, 16,
                                            DAIF_PA_GAIN_2_DRV0_PCT, 16, DAIF_PA_GAIN_1_DRIV_NUM, 43, DAIF_PA_GAIN_2_DRIV_NUM, 55));
    REGW(&OM_DAIF->PA_GAIN_REG_9, MASK_6REG(DAIF_PA_GAIN_1_DRV0_NCT, 16, DAIF_PA_GAIN_1_DRV0_PCT, 16, DAIF_PA_GAIN_2_DRV0_NCT, 16,
                                            DAIF_PA_GAIN_2_DRV0_PCT, 16, DAIF_PA_GAIN_1_DRIV_NUM, 59, DAIF_PA_GAIN_2_DRIV_NUM, 63));
    //防止二次校准出问题，校准前需要关掉这个DONE信号
    REGW0(&OM_DAIF->PA_GAIN_CFG, DAIF_PA_GAIN_CALI_DONE_MASK);
    //do pag calib
    REGW1(&OM_DAIF->PA_GAIN_CFG, DAIF_PA_GAIN_START_MASK);
    //wait pag done
    om_error_t ret; // Max: 2ms
    DRV_WAIT_MS_UNTIL_TO(OM_DAIF->PA_GAIN_CFG & DAIF_PA_GAIN_START_MASK, 4, ret); (void) ret;
    REGW(&OM_DAIF->VCO_CTRL0, MASK_3REG(DAIF_TRX_DBG,0, DAIF_RX_EN_MO,0, DAIF_TX_EN_MO,0));
    // default is 0DBM
    ///REGW(&OM_DAIF->PA_CNS, MASK_2REG(DAIF_EN_BYPASS_PALDO,0, DAIF_PA_GAIN_IDX_REG,RF_TX_POWER_0DBM));
}

/**
 *******************************************************************************
 * @brief  drv calib rf store
 *******************************************************************************
 */
static void drv_calib_rf_store(void)//delete
{
}

/**
 *******************************************************************************
 * @brief  calib rc32k accuracy check
 *
 * @return accuracy
 *******************************************************************************
 **/
static uint32_t drv_calib_rc32k_count_by_xtal32m(uint32_t win_32k_num)
{
    //check rc32k clock again with intr enabled
    REGWA(&OM_DAIF->CLK_CHK_CNS, MASK_5REG(
                DAIF_RX_RCCAL_START,1, DAIF_CLK_CHK_INTR_EN,1, DAIF_CLK_CHK_INTR_ST,1, DAIF_CLK_CHK_REF_SEL,0,
                DAIF_CLK_CHK_WIN_CNT_THRSH, win_32k_num)); // win_cnt: 32k period number
    // wait rc 32k check done -- 2
    while(OM_DAIF->CLK_CHK_CNS & DAIF_RX_RCCAL_START_MASK);
    DRV_DELAY_US(100);

    // got rc 32k check status
    return OM_DAIF->CLK_CHK_STATUS; // in win_cnt, how many 32MHz number
}


/*******************************************************************************
 * PUBLIC FUNCTIONS
 */
/**
 *******************************************************************************
 * @brief  calib rc32k accuracy check
 *
 * @return ppm
 *******************************************************************************
 **/
int drv_calib_rc32k_accuracy_check(uint32_t win_32k_num)
{
    // >>> 16*(1.0/32768.0)/(1.0/32000000)
    // 15625.0 (62ppm)
    // >>> 32*(1.0/32768.0)/(1.0/32000000)
    // 31250.0 (32ppm)
    // >>> 64*(1.0/32768.0)/(1.0/32000000)
    // 62500.0 (15ppm)

    int clk_num_32m;
    int clk_num_32m_std = win_32k_num * 32000000LL / 32768;

    DRV_RCC_ANA_CLK_ENABLE();

    REGW1(&OM_DAIF->CLK_ENS, DAIF_CC_CLK_EN_MASK);

    clk_num_32m = drv_calib_rc32k_count_by_xtal32m(win_32k_num);

    REGW0(&OM_DAIF->CLK_ENS, DAIF_CC_CLK_EN_MASK);

    DRV_RCC_ANA_CLK_RESTORE();

    return 1000000 * (clk_num_32m - clk_num_32m_std) / clk_num_32m_std;
}

/**
 *******************************************************************************
 * @brief  calib rc32k begin
 *******************************************************************************
 **/
void drv_calib_rc32k_full(void)
{
    // PMU reg bypass disable
    REGW(&OM_PMU->CLK_CTRL_1, MASK_1REG(PMU_CLK_CTRL_1_RC_32K_RCTUNE_SEL, 0));
    while((OM_PMU->STATUS_READ & PMU_STATUS_READ_SEL_RC32K_TUNE_MASK) != 0);

    // un-reset
    REGW1(&OM_DAIF->RC32K_TUN, DAIF_RC_32K_TUNE_SWRSTN_MASK);

    //rc 32k tune
    // do rc 32k tune 1
    REGW1(&OM_DAIF->RC32K_TUN, DAIF_RC_32K_TUNE_START_MASK);

    // wait rc32k tune 1 done
    while(OM_DAIF->RC32K_TUN & DAIF_RC_32K_TUNE_START_MASK);

    // Save
    REGW(&OM_PMU->CLK_CTRL_2, MASK_1REG(PMU_CLK_CTRL_2_RTUNE_RC32K_REG, REGR(&OM_DAIF->RC32K_TUN_OUT, MASK_POS(DAIF_RC32K_RTUNE))));
    REGW(&OM_PMU->CLK_CTRL_2, MASK_1REG(PMU_CLK_CTRL_2_CTUNE_RC32K_REG, REGR(&OM_DAIF->RC32K_TUN_OUT, MASK_POS(DAIF_RC32K_CTUNE))));
    REGW(&OM_PMU->CLK_CTRL_1, MASK_1REG(PMU_CLK_CTRL_1_RCTUNE_RC32K_UPDATE_REG, 1));
    while(OM_PMU->STATUS_READ & PMU_STATUS_READ_RCTUNE_RC32K_UPDATE_STATUS_MASK);

    // use REG to switch it
    REGW(&OM_PMU->CLK_CTRL_1, MASK_1REG(PMU_CLK_CTRL_1_RC_32K_RCTUNE_SEL, 1));
    while((OM_PMU->STATUS_READ & PMU_STATUS_READ_SEL_RC32K_TUNE_MASK) == 0);

    // reset
    REGW0(&OM_DAIF->RC32K_TUN, DAIF_RC_32K_TUNE_SWRSTN_MASK);

    // Must delay 1 32k to switch it
    DRV_DELAY_US(60);

    drv_calib_env.rc32k_is_calibed = true;
}

/**
 *******************************************************************************
 * @brief  drv calib rc32k fast
 *******************************************************************************
 */
void drv_calib_rc32k_fast(void)
{
    int jitter_new = 0;
    uint8_t  rc32k_rtune =0;
    uint8_t  rc32k_delta_num = 0;
    bool recalib_32k;

    jitter_new = drv_calib_rc32k_accuracy_check(256);

    do {
        const uint16_t jitter_tab[] = {
            200, 1, 300, 3, 400, 4, 500,   5,  600, 6,
            700, 7, 800, 8, 900, 9, 1000, 10,
        };

        recalib_32k = true;
        for (uint8_t i=0; i<sizeof(jitter_tab); i+=2) {
            if (abs(jitter_new) <= jitter_tab[i]) {
                rc32k_delta_num = jitter_tab[i+1];
                recalib_32k = false;
                break;
            }
        }
    } while(0);

    while((rc32k_delta_num > 0) || recalib_32k) {
        rc32k_rtune = REGR(&OM_PMU->CLK_CTRL_2, MASK_POS(PMU_CLK_CTRL_2_RTUNE_RC32K_REG));
        if(jitter_new >= 0) {
            if(rc32k_rtune <= 0) {
                recalib_32k = true;
            } else {
                rc32k_rtune -= ((rc32k_delta_num >= 4) ? 2 : 1);
            }
        } else {
            if(rc32k_rtune >= 255) {
                recalib_32k = true;
            } else {
                rc32k_rtune += ((rc32k_delta_num >= 4) ? 2 : 1);
            }
        }

        if (recalib_32k) {
            drv_calib_rc32k_full();
            break;
        } else {
            REGW(&OM_PMU->CLK_CTRL_2, MASK_1REG(PMU_CLK_CTRL_2_RTUNE_RC32K_REG, rc32k_rtune));
            REGW(&OM_PMU->CLK_CTRL_1, MASK_2REG(PMU_CLK_CTRL_1_RC_32K_RCTUNE_SEL,1, PMU_CLK_CTRL_1_RCTUNE_RC32K_UPDATE_REG,1));
        }

        jitter_new = drv_calib_rc32k_accuracy_check(128);
        if ((jitter_new <= 200) && (jitter_new >= -200)) {
            break;
        }

        rc32k_delta_num--;
        if (rc32k_delta_num == 0) {
            if ((jitter_new >= 500) || (jitter_new <= -500)) {
                drv_calib_rc32k_full();
                break;
            }
        }
    }
}

/**
 *******************************************************************************
 * @brief  calib rc32k to 62ppm
 *******************************************************************************
 **/
void drv_calib_rc32k(void)
{
    // Eanble depend clock
    drv_pmu_ana_enable(true, PMU_ANA_CALIB_RC32K);

    // do calib rc32k
    if (drv_calib_env.rc32k_is_calibed) {
        drv_calib_rc32k_fast();
    } else {
        drv_calib_rc32k_full();
    }

    // Disable depend clock
    drv_pmu_ana_enable(false, PMU_ANA_CALIB_RC32K);
}

/**
 *******************************************************************************
 * @brief calib sys rc32m
 *******************************************************************************
 **/
void drv_calib_sys_rc32m(void)
{
    // clock
    REGW1(&OM_DAIF->CLK_ENS, DAIF_RC_32M_TUNE_CLK_EN_MASK|DAIF_XTAL32M_CTRL_CLK_EN_MASK);

    // FSM
    REGW(&OM_PMU->CLK_CTRL_1, MASK_1REG(PMU_CLK_CTRL_1_CTUNE_OSC_SEL, 0));

    //rc 32m tune
    // do rc 32m tune 1
    REGW1(&OM_DAIF->RC32M_TUN, DAIF_RC_32M_TUNE_START_MASK);
    // wait rc32m tune 1 done
    while(OM_DAIF->RC32M_TUN & DAIF_RC_32M_TUNE_START_MASK);

    // save
    REGW(&OM_PMU->CLK_CTRL_1, MASK_2REG(PMU_CLK_CTRL_1_CTUNE_OSC_SEL, 1,
                                        PMU_CLK_CTRL_1_CT_OSC32M,     REGR(&OM_DAIF->RC32M_TUN,MASK_POS(DAIF_OSC32M_CT))));
}

/**
 *******************************************************************************
 * @brief  drv calib sys rc
 *******************************************************************************
 */
void drv_calib_sys_rc(void)
{
    // clock
    REGW1(&OM_DAIF->CLK_ENS, DAIF_MAIN_FSM_CLK_EN_MASK|DAIF_PLL_CLK_REF_EN_MASK|DAIF_PLL_VTRACK_CLK_EN_MASK);
    REGW1(&OM_DAIF->CLK_CFG, DAIF_XTAL32M_EN_CKO16M_DIG_MASK|DAIF_XTAL32M_EN_CKO16M_ANA_MASK);
    // Power
    REGW(&OM_DAIF->PD_CFG0, MASK_2REG(DAIF_RF_LDO_PD_MO,0,DAIF_RF_LDO_PD_ME,1));
    DRV_DELAY_US(10);

    //do pag calib
    REGW1(&OM_DAIF->RX_RCCAL_CTRL, DAIF_RX_RCCAL_START_MASK);
    // fix bug: When CE is set to high level, resetting the CPU may cause the chip to fail drv_calib_sys_rc () calibration.
    REGW(&OM_DAIF->VCO_CTRL0, MASK_3REG(DAIF_TRX_DBG,1, DAIF_RX_EN_MO,0, DAIF_TX_EN_MO,0));
    REGW(&OM_DAIF->VCO_CTRL0, MASK_3REG(DAIF_TRX_DBG,1, DAIF_RX_EN_MO,1, DAIF_TX_EN_MO,0));
    // wait pag done
    while(OM_DAIF->RX_RCCAL_CTRL & DAIF_RX_RCCAL_START_MASK);

    // Save
    REGW(&OM_PMU->CLK_CTRL_1, MASK_1REG(PMU_CLK_CTRL_1_CT_FDOUBLER,
                REGR(&OM_DAIF->TIA_DCOC_R3, MASK_POS(DAIF_TIA_CTUNE_FSM))));

    // Power
    REGW(&OM_DAIF->PD_CFG0, MASK_2REG(DAIF_RF_LDO_PD_MO,1,DAIF_RF_LDO_PD_ME,0));

    // fix bug: When CE is set to high level, resetting the CPU may cause the chip to fail drv_calib_rf_dcoc () calibration.
    REGW(&OM_DAIF->VCO_CTRL0, MASK_1REG(DAIF_TRX_DBG,0));
    // Wait idle
    DRV_DAIF_WAIT_IDLE();
    // clock
    REGW0(&OM_DAIF->CLK_ENS, DAIF_MAIN_FSM_CLK_EN_MASK|DAIF_PLL_VTRACK_CLK_EN_MASK);
    REGW0(&OM_DAIF->CLK_CFG, DAIF_XTAL32M_EN_CKO16M_DIG_MASK|DAIF_XTAL32M_EN_CKO16M_ANA_MASK);
}

/**
 *******************************************************************************
 * @brief  drv calib rf
 *******************************************************************************
 */
void drv_calib_rf(void)
{
    if (SYS_IS_FPGA()) {
        return;
    }

    drv_pmu_ana_enable(true, PMU_ANA_RF);
    // enable clock
    uint32_t clk_ens_save = OM_DAIF->CLK_ENS;
    REGW1(&OM_DAIF->CLK_ENS, DAIF_PLL_AFC_CLK_EN_MASK | DAIF_RX_AGC_CLK_EN_MASK |
                DAIF_PLL_VTRACK_CLK_EN_MASK | DAIF_PLL_LUT_CLK_EN_MASK | DAIF_MAIN_FSM_CLK_EN_MASK |
                DAIF_PLL_CLK_AFC_EN_MASK | DAIF_SDM_CLK_EN_MASK | DAIF_PLL_CLK_REF_EN_MASK | DAIF_PLL_AMP_CLK_EN_MASK);

    // patch
    drv_calib_patch(true/*is_calib_start*/);

    // do calib
    drv_calib_rf_afc();
    drv_calib_rf_dcoc();
    drv_calib_rf_txfe();

    // store calib value
    drv_calib_rf_store();

    // patch
    drv_calib_patch(false/*is_calib_start*/);

    // restore clock
    OM_DAIF->CLK_ENS = clk_ens_save;

    drv_pmu_ana_enable(false, PMU_ANA_RF);
}

/**
 *******************************************************************************
 * @brief  drv calib rf restore
 *******************************************************************************
 */
__RAM_CODES("PM")
void drv_calib_rf_restore(void)
{
    // Restore icon_vco
    REGW(&OM_DAIF->MIX_CFG1, MASK_2REG(DAIF_ICON_VCO_MO,drv_calib_env.icon_vco, DAIF_ICON_VCO_ME,1));

#if 0 // Fix RX gain (FIXME)
    REGW(&OM_DAIF->LNA_MIX_CFG, MASK_2REG(DAIF_RF_ATTEN_EN_MO,0,  DAIF_RF_ATTEN_EN_ME,1));
    REGW(&OM_DAIF->LNA_MIX_CFG, MASK_2REG(DAIF_RF_ATTEN_GAIN_MO,2,DAIF_RF_ATTEN_GAIN_ME,1));
    REGW(&OM_DAIF->LNA_MIX_CFG, MASK_2REG(DAIF_RF_LNA_GAIN_MO,5,  DAIF_RF_LNA_GAIN_ME,1));  // useful
    REGW(&OM_DAIF->TIA_LPF_CFG, MASK_2REG(DAIF_FILTER_GTUNE_MO,4, DAIF_FILTER_GTUNE_ME,1)); // useful
#endif

    // FIX: prevent BLE rxen and mac_rx_win has gap
    //REGW(&OM_DAIF->MAIN_ST_CFG2, MASK_1REG(DAIF_RX_PLL_WAIT, 0x510));
}

/**
 *******************************************************************************
 * @brief  drv calib sys restore
 *******************************************************************************
 */
__RAM_CODES("PM")
void drv_calib_sys_restore(void)
{
}

#endif  /* RTE_CALIB*/

/** @} */
