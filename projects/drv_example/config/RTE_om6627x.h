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
 * @brief    RTE driver
 * @details  All the RTE configuration file
 *
 * @version
 * Version 1.0
 *  - Initial release
 *
 * @{
 */

//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------

#ifndef __RTE_OM6627X_H
#define __RTE_OM6627X_H

#ifndef OM6627X
#error "The RTE file is not match with the selected device"
#endif

// <o.0> RTE_SYSTICK
#define RTE_SYSTICK                                     0
//   <o.0..2> RTE_SYSTICK_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_SYSTICK_IRQ_PRIORITY                        7
// <o.0> RTE_SYSTICK_REGISTER_CALLBACK
#define RTE_SYSTICK_REGISTER_CALLBACK                   0

// <o.0> RTE_RCC
#define RTE_RCC                                         1

// <o.0> RTE_PIN
#define RTE_PIN                                         1

// <o.0> RTE_ISR
#define RTE_ISR                                         1

// <o.0> RTE_ICACHE
#define RTE_ICACHE                                      1

// <o.0> RTE_CALIB
#define RTE_CALIB                                       1

// <o.0> RTE_GPDMA
#define RTE_GPDMA                                       1
// <o.0..2> RTE_GPDMA_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_GPDMA_IRQ_PRIORITY                          3

// <o.0> RTE_GPIO0
#define RTE_GPIO0                                       1
// <o.0> RTE_GPIO1
#define RTE_GPIO1                                       1
//   <o.0..2> RTE_GPIO_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_GPIO_IRQ_PRIORITY                           7
// <o.0> RTE_GPIO_REGISTER_CALLBACK
#define RTE_GPIO_REGISTER_CALLBACK                      1

// <o.0> RTE_UART0
#define RTE_UART0                                       1
//   <o.0..2> RTE_UART0_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_UART0_IRQ_PRIORITY                          3
// <o.0> RTE_UART1
#define RTE_UART1                                       1
//   <o.0..2> RTE_UART1_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_UART1_IRQ_PRIORITY                          3
// <o.0> RTE_UART2
#define RTE_UART2                                       1
//   <o.0..2> RTE_UART2_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_UART2_IRQ_PRIORITY                          3
// <o.0> RTE_UART_REGISTER_CALLBACK
#define RTE_UART_REGISTER_CALLBACK                      1

// <o.0> RTE_SPI0
#define RTE_SPI0                                        1
//   <o.0..2> RTE_SPI0_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_SPI0_IRQ_PRIORITY                           3
// <o.0> RTE_SPI1
#define RTE_SPI1                                        1
//   <o.0..2> RTE_SPI1_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_SPI1_IRQ_PRIORITY                           3
// <o.0> RTE_SPI_REGISTER_CALLBACK
#define RTE_SPI_REGISTER_CALLBACK                       1
// <o.0> RTE_SPI_CSN_MANUAL_CONTROL
#define RTE_SPI_CSN_MANUAL_CONTROL                      0

// <o.0> RTE_TIM0
#define RTE_TIM0                                        1
//   <o.0..2> RTE_TIM0_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_TIM0_IRQ_PRIORITY                           3
// <o.0> RTE_TIM1
#define RTE_TIM1                                        1
//   <o.0..2> RTE_TIM1_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_TIM1_IRQ_PRIORITY                           3
// <o.0> RTE_TIM2
#define RTE_TIM2                                        1
//   <o.0..2> RTE_TIM2_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_TIM2_IRQ_PRIORITY                           3
// <o.0> RTE_TIM_REGISTER_CALLBACK
#define RTE_TIM_REGISTER_CALLBACK                       1

// <o.0> RTE_LPTIM
#define RTE_LPTIM                                       1
//   <o.0..2> RTE_LPTIM_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_LPTIM_IRQ_PRIORITY                          7
// <o.0> RTE_LPTIM_REGISTER_CALLBACK
#define RTE_LPTIM_REGISTER_CALLBACK                     1

// <o.0> RTE_WDT
#define RTE_WDT                                         1
//   <o.0..2> RTE_WDT_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_WDT_IRQ_PRIORITY                            0
// <o.0> RTE_WDT_REGISTER_CALLBACK
#define RTE_WDT_REGISTER_CALLBACK                       1

// <o.0> RTE_RNG
#define RTE_RNG                                         1

// <o.0> RTE_PMU
#define RTE_PMU                                         1
//   <o.0..2> RTE_PMU_PIN_WAKEUP_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_PMU_PIN_WAKEUP_IRQ_PRIORITY                 4
// <o.0> RTE_PMU_PIN_WAKEUP_REGISTER_CALLBACK
#define RTE_PMU_PIN_WAKEUP_REGISTER_CALLBACK            1
//   <o.0..2> RTE_PMU_POF_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_PMU_POF_IRQ_PRIORITY                        4
// <o.0> RTE_PMU_POF_REGISTER_CALLBACK
#define RTE_PMU_POF_REGISTER_CALLBACK                   1

// <o.0> RTE_PMU_TIMER
#define RTE_PMU_TIMER                                   1
//   <o.0..2> RTE_PMU_TIMER_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_PMU_TIMER_IRQ_PRIORITY                      15
// <o.0> RTE_PMU_TIMER_REGISTER_CALLBACK
#define RTE_PMU_TIMER_REGISTER_CALLBACK                 1

// <o.0> RTE_RTC
#define RTE_RTC                                         1
//   <o.0..2> RTE_RTC_SECOND_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_RTC_SECOND_IRQ_PRIORITY                     7
//   <o.0..2> RTE_RTC_ALARM_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_RTC_ALARM_IRQ_PRIORITY                      7
// <o.0> RTE_RTC_REGISTER_CALLBACK
#define RTE_RTC_REGISTER_CALLBACK                       1

// <o.0> RTE_FLASH0
#define RTE_FLASH0                                      1
//   <o.0..2> RTE_FLASH0_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_FLASH0_IRQ_PRIORITY                         3
// <o.0> RTE_FLASH0_REGISTER_CALLBACK
#define RTE_FLASH0_REGISTER_CALLBACK                    1
// <o.0> RTE_FLASH0_XIP
#define RTE_FLASH0_XIP                                  1

// <o.0> RTE_OSPI1
#define RTE_OSPI1                                       1
//   <o.0..2> RTE_OSPI1_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_OSPI1_IRQ_PRIORITY                          3
// <o.0> RTE_OSPI1_REGISTER_CALLBACK
#define RTE_OSPI1_REGISTER_CALLBACK                     1

// <o.0> RTE_FLASH1
#define RTE_FLASH1                                      (1 && RTE_OSPI1)
// <o.0> RTE_FLASH1_XIP
#define RTE_FLASH1_XIP                                  0

// <o.0> RTE_PSRAM
#define RTE_PSRAM                                       (0 && RTE_OSPI1)

// <o.0> RTE_RF
#define RTE_RF                                          1
//   <o.0..2> RTE_RF_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_RF_IRQ_PRIORITY                             10

// <o.0> RTE_USB
#define RTE_USB                                         1
//   <o.0..2> RTE_USB_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_USB_IRQ_PRIORITY                            8
//   <o.0..2> RTE_USB_WK_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_USB_WK_IRQ_PRIORITY                         9
// <o.0> RTE_USB_REGISTER_CALLBACK
#define RTE_USB_REGISTER_CALLBACK                       0

// <o.0> RTE_EFUSE
#define RTE_EFUSE                                       1
// <o.0> RTE_EFUSE_IRQ_PRIORITY
#define RTE_EFUSE_IRQ_PRIORITY                          1
// <o.0> RTE_EFUSE_REGISTER_CALLBACK
#define RTE_EFUSE_REGISTER_CALLBACK                     1

// <o.0> RTE_SHA256
#define RTE_SHA256                                      1
// <o.0> RTE_SHA256_USING_BIG_ENDIAN
#define RTE_SHA256_USING_BIG_ENDIAN                     1

// <o.0> RTE_ECDSA
#define RTE_ECDSA                                       1

// <o.0> RTE_AES
#define RTE_AES                                         1

// <o.0> RTE_I2C0
#define RTE_I2C0                                        1
//   <o.0..2> RTE_I2C0_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_I2C0_IRQ_PRIORITY                           3
// <o.0> RTE_I2C1
#define RTE_I2C1                                        1
//   <o.0..2> RTE_I2C1_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_I2C1_IRQ_PRIORITY                           3
// <o.0> RTE_I2C_REGISTER_CALLBACK
#define RTE_I2C_REGISTER_CALLBACK                       1

// <o.0> RTE_I2S
#define RTE_I2S                                         1
//   <o> RTE_I2S_MODE
//   <0=>I2S   <1=>Left align   <2=>Right align   <3=>DSP_A   <7=>DSP_B
#define RTE_I2S_MODE                                    0
//   <o.0..2> RTE_I2S_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_I2S_IRQ_PRIORITY                            7
// <o.0> RTE_I2S_REGISTER_CALLBACK
#define RTE_I2S_REGISTER_CALLBACK                       1
//   <o> RTE_I2S_GPDMA_LLP_CHAIN_NUM
#define RTE_I2S_GPDMA_LLP_CHAIN_NUM                     2

// <o.0> RTE_AUDIO
#define RTE_AUDIO                                       1
//   <o.0..2> RTE_AUDIO_IRQ_PRIORITY
//     <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_AUDIO_IRQ_PRIORITY                          15
//   <o.0> RTE_AUDIO_USE_INTERNAL
#define RTE_AUDIO_USE_INTERNAL                          (1 && RTE_AUDIO)
//   <o.0> RTE_AUDIO_USE_EXTERNAL
#define RTE_AUDIO_USE_EXTERNAL                          (RTE_AUDIO && (!RTE_AUDIO_USE_INTERNAL))
//   <o.0> RTE_AUDIO_USE_ANA_MIC
#define RTE_AUDIO_USE_ANA_MIC                           0

// <o.0> RTE_QDEC
#define RTE_QDEC                                        1
//   <o.0..2> RTE_QDEC_IRQ_PRIORITY
//       <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_QDEC_IRQ_PRIORITY                           3
// <o.0> RTE_QDEC_REGISTER_CALLBACK
#define RTE_QDEC_REGISTER_CALLBACK                      1

// <o.0> RTE_RGB
#define RTE_RGB                                         1
//   <o.0..2> RTE_RGB_IRQ_PRIORITY
//       <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_RGB_IRQ_PRIORITY                            6
// <o.0> RTE_RGB_REGISTER_CALLBACK
#define RTE_RGB_REGISTER_CALLBACK                       1

// <o.0> RTE_GPADC
#define RTE_GPADC                                       0
//   <o.0..2> RTE_GPADC_IRQ_PRIORITY
//       <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_GPADC_IRQ_PRIORITY                          6
// <o.0> RTE_GPADC_REGISTER_CALLBACK
#define RTE_GPADC_REGISTER_CALLBACK                     1

// <o.0> RTE_IRTX
#define RTE_IRTX                                        1
//   <o.0..2> RTE_IRTX_IRQ_PRIORITY
//       <0=>0   <1=>1   <2=>2   <3=>3   <4=>4   <5=>5   <6=>6   <7=>7
#define RTE_IRTX_IRQ_PRIORITY                           6
// <o.0> RTE_GPADC_REGISTER_CALLBACK
#define RTE_IRTX_REGISTER_CALLBACK                      1

// <o.0> RTE_OM24G
#define RTE_OM24G                                       1
// <o.0> RTE_OM24G_IRQ_PRIORITY
#define RTE_OM24G_IRQ_PRIORITY                          2
// <o.0> RTE_OM24G_REGISTER_CALLBACK
#define RTE_OM24G_REGISTER_CALLBACK                     1

//   <o.0..2> RTE_IRTX_IRQ_PRIORITY
//       <0=>0   <1=>1   <2=>2   <3=>3
// 0: Compatible with om66XX  1: Compatible with TI2640  2: Compatible with SILICONLAB  3: Compatible with NORDIC
#define RTE_OM24G_RF_MODE                               0

// <o.0> RTE_LCD
#define RTE_LCD                                         1
// <o.0> RTE_LCD_IRQ_PRIORITY
#define RTE_LCD_IRQ_PRIORITY                            0
// <o.0> RTE_LCD_REGISTER_CALLBACK
#define RTE_LCD_REGISTER_CALLBACK                       1


#endif  /* __RTE_OM6627X_H */


/** @} */
