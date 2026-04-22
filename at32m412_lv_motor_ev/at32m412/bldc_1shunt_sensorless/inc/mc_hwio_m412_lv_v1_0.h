/**
  **************************************************************************
  * @file     mc_hwio_m412_lv_v1_0.h
  * @brief    Definition and declaration of Hardware peripheral configuration
  **************************************************************************
  *                       Copyright notice & Disclaimer
  *
  * The software Board Support Package (BSP) that is made available to
  * download from Artery official website is the copyrighted work of Artery.
  * Artery authorizes customers to use, copy, and distribute the BSP
  * software and its related documentation for the purpose of design and
  * development in conjunction with Artery microcontrollers. Use of the
  * software is governed by this copyright notice and the following disclaimer.
  *
  * THIS SOFTWARE IS PROVIDED ON "AS IS" BASIS WITHOUT WARRANTIES,
  * GUARANTEES OR REPRESENTATIONS OF ANY KIND. ARTERY EXPRESSLY DISCLAIMS,
  * TO THE FULLEST EXTENT PERMITTED BY LAW, ALL EXPRESS, IMPLIED OR
  * STATUTORY OR OTHER WARRANTIES, GUARANTEES OR REPRESENTATIONS,
  * INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE, OR NON-INFRINGEMENT.
  *
  **************************************************************************
  */

#ifndef __MC_HWIO_M412_LV_V1_0_H
#define __MC_HWIO_M412_LV_V1_0_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mc_lib.h"

/**************** define Timer for PWM ******************/
#define PWM_ADVANCE_TIMER              TMR1
#define PWM_ADVANCE_TIMER_CRM_CLK      CRM_TMR1_PERIPH_CLOCK
#define PWM_ADVANCE_TIMER_CRM_RESET    CRM_TMR1_PERIPH_RESET
#define ADVTMR_PWM_CYCLE_IRQ           TMR1_OVF_TMR10_IRQHandler
#define ADVTMR_PWM_CYCLE_IRQn          TMR1_OVF_TMR10_IRQn
#define ADVTMR_CH4_ADC_TRIG_IRQ        TMR1_CH_IRQHandler
#define ADVTMR_CH4_ADC_TRIG_IRQn       TMR1_CH_IRQn
#define ADVTMR_CH1_EMF_PULL_IRQ        TMR1_CH_IRQHandler
#define ADVTMR_CH1_EMF_PULL_IRQn       TMR1_CH_IRQn
#define ADVTMR_PWM_BRK_IRQ             TMR1_BRK_TMR9_IRQHandler
#define ADVTMR_PWM_BRK_IRQn            TMR1_BRK_TMR9_IRQn

/**************** define GPIO for PWM *******************/
#define PWM_PHASE_A_HI_GPIO_CRM_CLK    CRM_GPIOA_PERIPH_CLOCK
#define PWM_PHASE_A_HI_PORT            GPIOA
#define PWM_PHASE_A_HI_GPIO_PIN        GPIO_PINS_8
#define PWM_PHASE_A_HI_PIN_SOURCE      GPIO_PINS_SOURCE8
#define PWM_PHASE_A_HI_IOMUX           GPIO_MUX_1
#define PWM_PHASE_B_HI_GPIO_CRM_CLK    CRM_GPIOA_PERIPH_CLOCK
#define PWM_PHASE_B_HI_PORT            GPIOA
#define PWM_PHASE_B_HI_GPIO_PIN        GPIO_PINS_9
#define PWM_PHASE_B_HI_PIN_SOURCE      GPIO_PINS_SOURCE9
#define PWM_PHASE_B_HI_IOMUX           GPIO_MUX_1
#define PWM_PHASE_C_HI_GPIO_CRM_CLK    CRM_GPIOA_PERIPH_CLOCK
#define PWM_PHASE_C_HI_PORT            GPIOA
#define PWM_PHASE_C_HI_GPIO_PIN        GPIO_PINS_10
#define PWM_PHASE_C_HI_PIN_SOURCE      GPIO_PINS_SOURCE10
#define PWM_PHASE_C_HI_IOMUX           GPIO_MUX_1
#define PWM_PHASE_A_LOW_GPIO_CRM_CLK   CRM_GPIOB_PERIPH_CLOCK
#define PWM_PHASE_A_LOW_PORT           GPIOB
#define PWM_PHASE_A_LOW_GPIO_PIN       GPIO_PINS_7
#define PWM_PHASE_A_LOW_PIN_SOURCE     GPIO_PINS_SOURCE7
#define PWM_PHASE_A_LOW_IOMUX          GPIO_MUX_1
#define PWM_PHASE_B_LOW_GPIO_CRM_CLK   CRM_GPIOA_PERIPH_CLOCK
#define PWM_PHASE_B_LOW_PORT           GPIOA
#define PWM_PHASE_B_LOW_GPIO_PIN       GPIO_PINS_12
#define PWM_PHASE_B_LOW_PIN_SOURCE     GPIO_PINS_SOURCE12
#define PWM_PHASE_B_LOW_IOMUX          GPIO_MUX_10
#define PWM_PHASE_C_LOW_GPIO_CRM_CLK   CRM_GPIOB_PERIPH_CLOCK
#define PWM_PHASE_C_LOW_PORT           GPIOB
#define PWM_PHASE_C_LOW_GPIO_PIN       GPIO_PINS_6
#define PWM_PHASE_C_LOW_PIN_SOURCE     GPIO_PINS_SOURCE6
#define PWM_PHASE_C_LOW_IOMUX          GPIO_MUX_1

/* timer for adc trigger source */
#define ADC_TIMER                      TMR1
#define ADC_TIMER_SELECT_CHANNEL       TMR_SELECT_CHANNEL_4
#define ADC_TIMER_CRM_CLK              CRM_TMR1_PERIPH_CLOCK
#define ADC_TIMER_CRM_RESET            CRM_TMR1_PERIPH_RESET
/**************** define ADC TRIG OUTPUT PIN ******************/
#define TMR_ADC_TRIG_GPIO_CRM_CLK      CRM_GPIOA_PERIPH_CLOCK
#define TMR_ADC_TRIG_PORT              GPIOA
#define TMR_ADC_TRIG_GPIO_PIN          GPIO_PINS_11
#define TMR_ADC_TRIG_GPIO_PIN_SOURCE   GPIO_PINS_SOURCE11
#define TMR_ADC_TRIG_IOMUX             GPIO_MUX_1
/**************** define CMP1 OUT PIN ******************/
#define TMR_CMP1_OUT_GPIO_CRM_CLK      CRM_GPIOA_PERIPH_CLOCK
#define TMR_CMP1_OUT_PORT              GPIOA
#define TMR_CMP1_OUT_GPIO_PIN          GPIO_PINS_11
#define TMR_CMP1_OUT_GPIO_PIN_SOURCE   GPIO_PINS_SOURCE11
#define TMR_CMP1_OUT_IOMUX             GPIO_MUX_12


#if defined HALL_SENSORS
/**************** define Timer for Hall ******************/
#define HALL_CAPTURE_TIMER             TMR3
#define HALL_CAPTURE_CRM_CLK           CRM_TMR3_PERIPH_CLOCK
#define HALL_CAPTURE_IRQ               TMR3_GLOBAL_IRQHandler
#define HALL_CAPTURE_IRQn              TMR3_GLOBAL_IRQn
#define HALL_CAPTURE_FILTER_CLK_DIV    TMR_CLOCK_DIV2
#define TMR_HALL_IN_FILTER             0x6                        /*0x0 ~ 0xF*/
#else
/**************** define Timer for Read EMF Comparator ******************/
// #define READ_EMF_TIMER                 TMR3
// #define READ_EMF_CRM_CLK               CRM_TMR3_PERIPH_CLOCK
// #define READ_EMF_IRQ                   TMR3_GLOBAL_IRQHandler
// #define READ_EMF_IRQn                  TMR3_GLOBAL_IRQn
// #define TMR_READ_EMF_FILTER            0x6                        /*0x0 ~ 0xF*/
#endif
/**************** define Timer for Change Phase ******************/
#define CHANGE_PHASE_TIMER             TMR7
#define CHANGE_PHASE_CRM_CLK           CRM_TMR7_PERIPH_CLOCK
#define CHANGE_PHASE_IRQ               TMR7_GLOBAL_IRQHandler
#define CHANGE_PHASE_IRQn              TMR7_GLOBAL_IRQn
#define TMR_CHANGE_PHASE_FILTER        0x6                        /*0x0 ~ 0xF*/
/**************** define Timer for blank signal trigger source ******************/
#define BLANK_TRIGGER_TIMER            TMR3                       
#define BLANK_TRIGGER_SELECT_CHANNEL   TMR_SELECT_CHANNEL_1
#define BLANK_TRIGGER_CRM_CLK          CRM_TMR3_PERIPH_CLOCK
#define BLANK_TRIGGER_SYNC_INPUT_SEL   TMR_SUB_INPUT_SEL_IS0
#define BLANK_TRIGGER_IRQ              TMR3_GLOBAL_IRQHandler
#define BLANK_TRIGGER_IRQn             TMR3_GLOBAL_IRQn
#define TMR_BLANK_TRIGGER_FILTER       0x6                        /*0x0 ~ 0xF*/

#define DMA_BLANK_TRIGGER              DMA1
#define DMA_BLANK_TRIGGER_REQUEST      TMR_C1_DMA_REQUEST
#define DMA_CHANNEL_BLANK_TRIGGER      DMA1_CHANNEL5
#define DMA_BLANK_TRIGGER_FLEX         DMAMUX_DMAREQ_ID_TMR3_CH1
#define DMA_BLANK_TRIGGER_FLEX_CH      DMA1MUX_CHANNEL5
#define DMA_BLANK_TRIGGER_FT_STS_FLAG  DMA1_FDT5_FLAG
#define DMA_BLANK_TRIGGER_GEN_SIGNAL   DMAMUX_GEN_ID_DMAMUX_CH5_EVT

#define BLANK_TRIGGER_GPIO_CRM_CLK     CRM_GPIOB_PERIPH_CLOCK
#define BLANK_TRIGGER_PORT             GPIOB
#define BLANK_TRIGGER_GPIO_PIN         GPIO_PINS_4
#define BLANK_TRIGGER_GPIO_PIN_SOURCE  GPIO_PINS_SOURCE4
#define BLANK_TRIGGER_IOMUX            GPIO_MUX_2                 
/**************** define Timer for blank signal ******************/
#define BLANK_TIMER                    TMR9
#define BLANK_TIMER_SELECT_CHANNEL     TMR_SELECT_CHANNEL_1
#define BLANK_CRM_CLK                  CRM_TMR9_PERIPH_CLOCK
#define BLANK_TMR_SYNC_INPUT_SEL       TMR_SUB_INPUT_SEL_IS1
#define CMP_BLANKING_SOURCE            CMP_BLANKING_TMR9_CH1
#define TMR_BLANK_FILTER               0x6                        /*0x0 ~ 0xF*/

#define DMA_BLANK_WINDOW               DMA1
#define DMA_CHANNEL_BLANK_WINDOW       DMA1_CHANNEL6
#define DMA_BLANK_WINDOW_FLEX          DMAMUX_DMAREQ_ID_REQ_G1
#define DMA_BLANK_WINDOW_FLEX_CH       DMA1MUX_CHANNEL6

#define BLANK_SIGNAL_GPIO_CRM_CLK      CRM_GPIOF_PERIPH_CLOCK
#define BLANK_SIGNAL_PORT              GPIOF
#define BLANK_SIGNAL_GPIO_PIN          GPIO_PINS_9
#define BLANK_SIGNAL_GPIO_PIN_SOURCE   GPIO_PINS_SOURCE9
#define BLANK_SIGNAL_IOMUX             GPIO_MUX_3
/**************** define GPIO for Hall *******************/
#define HALL_A_GPIO_CRM_CLK            CRM_GPIOB_PERIPH_CLOCK
#define HALL_A_PORT                    GPIOB
#define HALL_A_GPIO_PIN                GPIO_PINS_4
#define HALL_A_GPIO_PIN_SOURCE         GPIO_PINS_SOURCE4
#define HALL_A_IOMUX                   GPIO_MUX_2
//#define HALL_A_IDT                     idt_bit.idt4
#define HALL_B_GPIO_CRM_CLK            CRM_GPIOB_PERIPH_CLOCK
#define HALL_B_PORT                    GPIOB
#define HALL_B_GPIO_PIN                GPIO_PINS_5
#define HALL_B_GPIO_PIN_SOURCE         GPIO_PINS_SOURCE5
#define HALL_B_IOMUX                   GPIO_MUX_2
//#define HALL_B_IDT                     idt_bit.idt5
#define HALL_C_GPIO_CRM_CLK            CRM_GPIOB_PERIPH_CLOCK
#define HALL_C_PORT                    GPIOB
#define HALL_C_GPIO_PIN                GPIO_PINS_3
#define HALL_C_GPIO_PIN_SOURCE         GPIO_PINS_SOURCE3
#define HALL_C_IOMUX                   GPIO_MUX_2
//#define HALL_C_IDT                     idt_bit.idt0
/**************** define backEMF COMP *******************/
#define BEMF_COMP                      CMP2_SELECTION
#define COMP_A_GPIO_CRM_CLK            CRM_GPIOA_PERIPH_CLOCK
#define COMP_A_PORT                    GPIOA
#define COMP_A_GPIO_PIN                GPIO_PINS_4
#define COMP_A_GPIO_PIN_SOURCE         GPIO_PINS_SOURCE4
#define COMP_A_CH                      CMP_INVERTING_INM4

#define COMP_B_GPIO_CRM_CLK            CRM_GPIOA_PERIPH_CLOCK
#define COMP_B_PORT                    GPIOA
#define COMP_B_GPIO_PIN                GPIO_PINS_5
#define COMP_B_GPIO_PIN_SOURCE         GPIO_PINS_SOURCE5
#define COMP_B_CH                      CMP_INVERTING_INM5

#define COMP_C_GPIO_CRM_CLK            CRM_GPIOA_PERIPH_CLOCK
#define COMP_C_PORT                    GPIOA
#define COMP_C_GPIO_PIN                GPIO_PINS_6
#define COMP_C_GPIO_PIN_SOURCE         GPIO_PINS_SOURCE6
#define COMP_C_CH                      CMP_INVERTING_INM7

#define COMP_N_GPIO_CRM_CLK            CRM_GPIOA_PERIPH_CLOCK
#define COMP_N_PORT                    GPIOA
#define COMP_N_GPIO_PIN                GPIO_PINS_7
#define COMP_N_GPIO_PIN_SOURCE         GPIO_PINS_SOURCE7

#define COMP_OUT_GPIO_CRM_CLK          CRM_GPIOB_PERIPH_CLOCK//CRM_GPIOB_PERIPH_CLOCK
#define COMP_OUT_PORT                  GPIOB//GPIOB
#define COMP_OUT_GPIO_PIN              GPIO_PINS_5//GPIO_PINS_9
#define COMP_OUT_GPIO_PIN_SOURCE       GPIO_PINS_SOURCE5//GPIO_PINS_SOURCE9
#define COMP_OUT_IOMUX                 GPIO_MUX_12//GPIO_MUX_12

#define COMP_OUT_CAPTURE_TIMER           TMR11
#define COMP_OUT_SELECT_CHANNEL          TMR_SELECT_CHANNEL_1
#define COMP_OUT_CAPTURE_CRM_CLK         CRM_TMR11_PERIPH_CLOCK
#define COMP_OUT_CAPTURE_FILTER_CLK_DIV  TMR_CLOCK_DIV2
#define COMP_OUT_CAPTURE_IRQ             TMR1_TRG_HALL_TMR11_IRQHandler
#define COMP_OUT_CAPTURE_IRQn            TMR1_TRG_HALL_TMR11_IRQn
#define COMP_OUT_CAPTURE_INPUT           TMR_TMR11_CH1_CMP2
#define COMP_OUT_FLAG                    TMR_C1_FLAG
#define COMP_OUT_INT                     TMR_C1_INT
#define TMR_COMP_OUT_CAPTURE_FILTER      0xF                        /*0x0 ~ 0xF*/
/**************** define Timer for pwm input *******************/
#define PWM_DUTY_INPUT_TIMER           TMR4
#define PWM_DUTY_INPUT_SELECT_CHANNEL  TMR_SELECT_CHANNEL_3          
#define PWM_DUTY_INPUT_CRM_CLK         CRM_TMR4_PERIPH_CLOCK
#define PWM_DUTY_INPUT_IRQ             TMR4_GLOBAL_IRQHandler
#define PWM_DUTY_INPUT_IRQn            TMR4_GLOBAL_IRQn
#define PWM_DUTY_INPUT_FLAG            TMR_C3_FLAG
#define PWM_DUTY_INPUT_INT             TMR_C3_INT
#define TMR_PWM_DUTY_INPUT_FILTER      0x1                      /* 0x0 ~ 0xF */

#define DMA_DSHOT_INPUT                DMA1
#define DMA_DSHOT_INPUT_CRM_CLK        CRM_DMA1_PERIPH_CLOCK
#define DMA_CHANNEL_DSHOT_INPUT        DMA1_CHANNEL7
#define DMA_DSHOT_INPUT_FLEX           DMAMUX_DMAREQ_ID_TMR4_CH3
#define DMA_DSHOT_INPUT_FLEX_CH        DMA1MUX_CHANNEL7
#define DMA_DSHOT_INPUT_IRQn           DMA1_Channel7_IRQn
#define DMA_DSHOT_INPUT_IRQHandler     DMA1_Channel7_IRQHandler
#define DMA_DSHOT_INPUT_FDT_FLAG       DMA1_FDT7_FLAG
#define DMA_DSHOT_INPUT_HDT_FLAG       DMA1_HDT7_FLAG
#define DMA_DSHOT_INPUT_DTERR_FLAG     DMA1_DTERR7_FLAG

#define PWM_DUTY_INPUT_GPIO_CRM_CLK      CRM_GPIOB_PERIPH_CLOCK
#define PWM_DUTY_INPUT_PORT              GPIOB
#define PWM_DUTY_INPUT_GPIO_PIN          GPIO_PINS_8
#define PWM_DUTY_INPUT_GPIO_PIN_SOURCE   GPIO_PINS_SOURCE8
#define PWM_DUTY_INPUT_IOMUX             GPIO_MUX_2
/* adc reading pin definition */
#define ADC_NORMAL_CONVERTER           ADC1
#define ADC_INSTANT_CONVERTER          ADC2

#define ADC_CONVERTER_CRM_CLK_DIV      ADC_HCLK_DIV_6

#define ADC_NORMAL_CRM_CLK             CRM_ADC1_PERIPH_CLOCK
#define ADC_INSTANT_CRM_CLK            CRM_ADC2_PERIPH_CLOCK

#define ADC_NORMAL_TRIG_SOURCE         ADC_ORDINARY_TRIG_TMR1CH1
#define ADC_NORMAL_TRIG_EDGE           ADC_ORDINARY_TRIG_EDGE_NONE

#define ADC_INSTANT_TRIG_SOURCE        ADC_PREEMPT_TRIG_TMR1CH4
#define ADC_INSTANT_TRIG_EDGE          ADC_PREEMPT_TRIG_EDGE_RISING

#define ADC_SHUNT_SAMP_READY_IRQ       ADC1_2_IRQHandler
#define ADC_SHUNT_SAMP_READY_IRQn      ADC1_2_IRQn
#define ADC_ORDINARY_CH_LEN            ADC_IDX_MAX
#define ADC_PREEMPT_CH_LEN             2

#define ADC_1V2_SAMPLETIME             ADC_SAMPLETIME_1_5
#define ADC_EMF_SAMPLETIME             ADC_SAMPLETIME_7_5
#define ADC_CURR_SAMPLETIME            ADC_SAMPLETIME_7_5

#define MCU_1V2_ADC_CH                 ADC_CHANNEL_17

/* dma1 ch1 for adc ordinary conversion*/
#define DMA_ADC_CRM_CLK                CRM_DMA1_PERIPH_CLOCK
#define ADC_ORDINARY_DMA_FLEX          DMAMUX_DMAREQ_ID_ADC1
#define ADC_ORDINARY_DMA_FLEX_CH       DMA1MUX_CHANNEL1
#define ADC_ORDINARY_DMA_FT_STS_FLAG   DMA1_FDT1_FLAG
#define DMA_CHANNEL_ADC_CONVERT        DMA1_CHANNEL1
/* adc preempt channel */
#define ADC_PREEMPT_DMA_FLEX           DMAMUX_DMAREQ_ID_TMR1_CH4
#define ADC_PREEMPT_DMA_FLEX_CH        DMA1MUX_CHANNEL4
#define DMA_CHANNEL_EMF_TRIG           DMA1_CHANNEL4

#define CURR_PHASE_A_ADC_CH            ADC_CHANNEL_12
#define CURR_PHASE_A_ADC_GPIO_CRM_CLK  CRM_GPIOB_PERIPH_CLOCK
#define CURR_PHASE_A_ADC_PORT          GPIOB
#define CURR_PHASE_A_ADC_GPIO_PIN      GPIO_PINS_12

/* #define CURR_PHASE_B_ADC_CH            ADC_CHANNEL_2
#define CURR_PHASE_B_ADC_GPIO_CRM_CLK  CRM_GPIOA_PERIPH_CLOCK
#define CURR_PHASE_B_ADC_PORT          GPIOA
#define CURR_PHASE_B_ADC_GPIO_PIN      GPIO_PINS_2 */

#define CURR_BUS_ADC_CH                ADC_CHANNEL_9
#define CURR_BUS_ADC_GPIO_CRM_CLK      CRM_GPIOB_PERIPH_CLOCK
#define CURR_BUS_ADC_PORT              GPIOB
#define CURR_BUS_ADC_GPIO_PIN          GPIO_PINS_1
#ifdef BLDC_SENSORLESS_COMP
#define CURR_BUS_ADC_PREEMPT_CH        ADC_PREEMPT_CHANNEL_1
#else
#define CURR_BUS_ADC_PREEMPT_CH        ADC_PREEMPT_CHANNEL_2
#endif

#define VOLT_BUS_ADC_CH                ADC_CHANNEL_0
#define VOLT_BUS_ADC_GPIO_CRM_CLK      CRM_GPIOA_PERIPH_CLOCK
#define VOLT_BUS_ADC_PORT              GPIOA
#define VOLT_BUS_ADC_GPIO_PIN          GPIO_PINS_0

#define MOS_TEMP_ADC_CH                ADC_CHANNEL_23
#define MOS_TEMP_ADC_GPIO_CRM_CLK      CRM_GPIOF_PERIPH_CLOCK
#define MOS_TEMP_ADC_PORT              GPIOF
#define MOS_TEMP_ADC_GPIO_PIN          GPIO_PINS_9

#define POTENTIO_ADC_CH                ADC_CHANNEL_26
#define POTENTIO_ADC_GPIO_CRM_CLK      CRM_GPIOF_PERIPH_CLOCK
#define POTENTIO_ADC_PORT              GPIOF
#define POTENTIO_ADC_GPIO_PIN          GPIO_PINS_12

#define BEMF_A_ADC_CH                  ADC_CHANNEL_4
#define BEMF_A_ADC_GPIO_CRM_CLK        CRM_GPIOA_PERIPH_CLOCK
#define BEMF_A_ADC_PORT                GPIOA
#define BEMF_A_ADC_GPIO_PIN            GPIO_PINS_4

#define BEMF_B_ADC_CH                  ADC_CHANNEL_5
#define BEMF_B_ADC_GPIO_CRM_CLK        CRM_GPIOA_PERIPH_CLOCK
#define BEMF_B_ADC_PORT                GPIOA
#define BEMF_B_ADC_GPIO_PIN            GPIO_PINS_5

#define BEMF_C_ADC_CH                  ADC_CHANNEL_6
#define BEMF_C_ADC_GPIO_CRM_CLK        CRM_GPIOA_PERIPH_CLOCK
#define BEMF_C_ADC_PORT                GPIOA
#define BEMF_C_ADC_GPIO_PIN            GPIO_PINS_6

/**************** i2c interface ******************/
/* #define I2C_INTERFACE                   I2C2
#define I2C_SCL_PORT                    GPIOB
#define I2C_SCL_GPIO_PIN                GPIO_PINS_13
#define I2C_SDA_PORT                    GPIOB
#define I2C_SDA_GPIO_PIN                GPIO_PINS_14 */


/**************** define uart Tx and Rx ******************/
#define COMM_UART                        USART2
#define COMM_UART_CRM_CLK                CRM_USART2_PERIPH_CLOCK
#define COMM_UART_TX_GPIO_CRM_CLK        CRM_GPIOA_PERIPH_CLOCK
#define COMM_UART_TX_PORT                GPIOA
#define COMM_UART_TX_GPIO_PIN_SOURCE     GPIO_PINS_SOURCE2
#define COMM_UART_TX_PIN                 GPIO_PINS_2
#define COMM_UART_TX_IOMUX               GPIO_MUX_7
#define COMM_UART_RX_GPIO_CRM_CLK        CRM_GPIOA_PERIPH_CLOCK
#define COMM_UART_RX_PORT                GPIOA
#define COMM_UART_RX_GPIO_PIN_SOURCE     GPIO_PINS_SOURCE3
#define COMM_UART_RX_PIN                 GPIO_PINS_3
#define COMM_UART_RX_IOMUX               GPIO_MUX_7
#define COMM_UART_IOMUX                  NULL
#define COMM_UART_IRQn                   USART2_IRQn
#define COMM_UART_IRQHandler             USART2_IRQHandler

/**************** define DMA for uart2 Tx and Rx ******************/
#define DMA_UART                         DMA1
#define DMA_UART_CRM_CLK                 CRM_DMA1_PERIPH_CLOCK
#define DMA_UART_TX_CHANNEL              DMA1_CHANNEL2
#define DMA_UART_RX_TX_IRQn              DMA1_Channel3_2_IRQn
#define DMA_UART_RX_TX_IRQHandler        DMA1_Channel3_2_IRQHandler
#define DMA_UART_TX_FDT_FLAG             DMA1_FDT2_FLAG
#define DMA_UART_TX_FLEX                 DMAMUX_DMAREQ_ID_USART2_TX
#define DMA_UART_TX_FLEX_CHANNEL         DMA1MUX_CHANNEL2

#define DMA_UART_RX_CHANNEL              DMA1_CHANNEL3
#define DMA_UART_RX_FDT_FLAG             DMA1_FDT3_FLAG
#define DMA_UART_RX_FLEX                 DMAMUX_DMAREQ_ID_USART2_RX
#define DMA_UART_RX_FLEX_CHANNEL         DMA1MUX_CHANNEL3

/******************* define led *******************/
#define ERROR_LED_GPIO_CRM_CLK        CRM_GPIOC_PERIPH_CLOCK
#define ERROR_LED_PORT                GPIOC
#define ERROR_LED_GPIO_PIN            GPIO_PINS_13

#define ADC_TRIG_LED_GPIO_CRM_CLK     CRM_GPIOA_PERIPH_CLOCK
#define ADC_TRIG_LED_PORT             GPIOA
#define ADC_TRIG_LED_GPIO_PIN         GPIO_PINS_11

#define LED_R_GPIO_CRM_CLK            CRM_GPIOB_PERIPH_CLOCK
#define LED_R_PORT                    GPIOB 
#define LED_R_GPIO_PIN                GPIO_PINS_10

#define LED_G_GPIO_CRM_CLK            CRM_GPIOA_PERIPH_CLOCK
#define LED_G_PORT                    GPIOA
#define LED_G_GPIO_PIN                GPIO_PINS_1

/* #define TEST_A_GPIO_CRM_CLK           CRM_GPIOF_PERIPH_CLOCK
#define TEST_A_PORT                   GPIOF
#define TEST_A_GPIO_PIN               GPIO_PINS_13
#define TEST_A_GPIO_PIN_SOURCE        GPIO_PINS_SOURCE13

#define TEST_B_GPIO_CRM_CLK           CRM_GPIOF_PERIPH_CLOCK
#define TEST_B_PORT                   GPIOF
#define TEST_B_GPIO_PIN               GPIO_PINS_7
#define TEST_B_GPIO_PIN_SOURCE        GPIO_PINS_SOURCE7

#define TEST_C_GPIO_CRM_CLK           CRM_GPIOF_PERIPH_CLOCK
#define TEST_C_PORT                   GPIOF
#define TEST_C_GPIO_PIN               GPIO_PINS_12
#define TEST_C_GPIO_PIN_SOURCE        GPIO_PINS_SOURCE12 */

/* emf_pull_up */
#define EMF_PULL_UP_GPIO_CRM_CLK      CRM_GPIOF_PERIPH_CLOCK
#define EMF_PULL_UP_PORT              GPIOF
#define EMF_PULL_UP_GPIO_PIN          GPIO_PINS_8

/******************* define button *******************/
#define HALL_LEARN_BUTTON_PIN            GPIO_PINS_13
#define HALL_LEARN_BUTTON_PORT           GPIOF
#define HALL_LEARN_BUTTON_CRM_CLK        CRM_GPIOF_PERIPH_CLOCK

#define HALL_LEARN_BUTTON_PORT_SOURCE    SCFG_PORT_SOURCE_GPIOF
#define HALL_LEARN_BUTTON_PIN_SOURCE     SCFG_PINS_SOURCE13
#define HALL_LEARN_BUTTON_EXINT_LINE     EXINT_LINE_13

/* opa pins definition */
#define BUS_OPA                          OP3
#define BUS_INP_PIN                      GPIO_PINS_0
#define BUS_INP_PORT                     GPIOB
#define BUS_INP_CRM_CLK                  CRM_GPIOB_PERIPH_CLOCK

#define BUS_INM_PIN                      GPIO_PINS_2
#define BUS_INM_PORT                     GPIOB
#define BUS_INM_CRM_CLK                  CRM_GPIOB_PERIPH_CLOCK

#define BUS_OUT_PIN                      GPIO_PINS_1
#define BUS_OUT_PORT                     GPIOB
#define BUS_OUT_CRM_CLK                  CRM_GPIOB_PERIPH_CLOCK

#define OCP_COMP                         CMP1_SELECTION
#define COMP_BUS_GPIO_CRM_CLK            CRM_GPIOB_PERIPH_CLOCK
#define COMP_BUS_PORT                    GPIOB
#define COMP_BUS_GPIO_PIN                GPIO_PINS_1
#define COMP_BUS_GPIO_PIN_SOURCE         GPIO_PINS_SOURCE1
#define COMP_BUS_CH                      CMP_NON_INVERTING_INP3
#define TMR_BRKIN_SOURCE                 TMR_BRKIN_SOURCE_CMP1

typedef enum
{
  ADC_BUS_VOLT_IDX                  = 0x00,
  ADC_MOS_TEMP_IDX                  = 0x01,
  ADC_POTENTIO_IDX                  = 0x02,
  ADC_IDX_MAX                       = 0x03
} adc_in_idx;

extern __IO uint16_t adc_in_tab[ADC_IDX_MAX];

void nvic_config(void);
void gpio_hall_init(void);
void gpio_output_init(void);
void pull_up_config(void);

/******************** functions ********************/
/* led operation function */
void led_config(void);
void led_on(gpio_type *led_gpio_port, uint16_t led_gpio_pin);
void led_off(gpio_type *led_gpio_port, uint16_t led_gpio_pin);
void led_toggle(gpio_type *led_gpio_port, uint16_t led_gpio_pin);
void led_init(void);
void led_blink(void);

/* button operation function */
void button_switch_init(void);
void button_exint_init(void);

/* systick function */
void systick_init(void);

/* COMM uart init function */
void uart_init(usart_config_type *usart_config);

/* DMA UART init function */
void dma_uart_configuration(void);

/* timer init function for read EMF */
void tmr_read_emf_init(void);
/* timer init function for Hall */
void tmr_hall_init(void);
/* timer init function for PWM */
void tmr_pwm_init(void);
/* timer init function for DSHOT input */
void dshot_input_timer_init(void);
void dshot_bidir_send_telemetry_nrz(uint16_t telemetry_data);
extern const uint8_t dshot_gcr_encode_lut[16];
void dshot_bidir_reset_hardware(void);
/* timer init function for change phase */
void tmr_sensorless_change_phase_init(void);
/* timer init function for read EMF */
void tmr_read_emf_init(void);
void tmr_comp_capture_init();
/* timer init function for timer */
void tmr_blank_trigger_init(void);
void tmr_blank_init(void);
/* ADC function for current sampling */
void adc_ordinary_config(void);
void adc_preempt_config(void);
/* OPA */
void opa_init(void);
void opa_calibration(op_type* op_num);
/* CMP */
void ocp_cmp_config(void);
void cmp2_config(void);

/* bldc config */
void bldc_angle_init_config(angle_init_type *angle_init, adc_sample_type *adc_sample);
void bldc_sensorless_detectEMF_config(adc_sample_type *adc_sample);

/* voltage calibration */
void get_int_vref_cal_ratio(void);

/* motor parameter identify configuration */
void motor_parameter_ID_config(void);

#ifdef __cplusplus
}
#endif

#endif

