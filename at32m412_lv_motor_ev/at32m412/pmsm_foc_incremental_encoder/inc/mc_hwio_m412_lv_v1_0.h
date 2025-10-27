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
/* 3-phase complementary pwm pin definition */
#define PWM_ADVANCE_TIMER                   TMR1
#define PWM_ADVANCE_TIMER_CRM_CLK           CRM_TMR1_PERIPH_CLOCK
#define ADVTMR_PWM_CYCLE_IRQ                TMR1_OVF_TMR10_IRQHandler
#define ADVTMR_PWM_CYCLE_IRQn               TMR1_OVF_TMR10_IRQn
#define ADVTMR_PWM_BRK_IRQ                  TMR1_BRK_TMR9_IRQHandler
#define ADVTMR_PWM_BRK_IRQn                 TMR1_BRK_TMR9_IRQn

/* timer for adc trigger source */
#define ADC_TIMER                           TMR1
#define ADC_TIMER_SELECT_CHANNEL            TMR_SELECT_CHANNEL_4
#define TMR_ADC_TRIG_SOURCE                 ADC_PREEMPT_TRIG_TMR1CH4
#define TMR_ADC_TRIG_SIGNAL                 ADC_PREEMPT_TRIG_EDGE_RISING
#define TMR_ADC_TRIG_NO_SIGNAL              ADC_PREEMPT_TRIG_EDGE_NONE
/* define ADC TRIG OUTPUT PIN */
#define TMR_ADC_TRIG_GPIO_CRM_CLK           CRM_GPIOA_PERIPH_CLOCK
#define TMR_ADC_TRIG_PORT                   GPIOA
#define TMR_ADC_TRIG_GPIO_PIN               GPIO_PINS_11
#define TMR_ADC_TRIG_GPIO_PIN_SOURCE        GPIO_PINS_SOURCE11
#define TMR_ADC_TRIG_IOMUX                  GPIO_MUX_1
#ifdef ONE_SHUNT
/* tmr adc dma definition */
#define TMR_ADC_DMA_CRM_CLK                 CRM_DMA1_PERIPH_CLOCK
#define TMR_ADC_DMA_CH                      DMA1_CHANNEL4
#define TMR_ADC_DMA                         DMA1
#define TMR_ADC_DMA_FLEX                    DMAMUX_DMAREQ_ID_TMR1_CH4
#define TMR_ADC_DMA_FLEX_CH                 DMA1MUX_CHANNEL4
#define TMR_CH_ADC_DMA_REQUEST              TMR_C4_DMA_REQUEST
#define TMR_ADC_DMA_PERIPHERAL_ADDR         ADC_TIMER->c4dt
#endif

/* timer for trig dma of switching op inp port mode trigger source */
#define SW_OP_INP_MODE_TIMER                TMR11
#define SW_OP_INP_MODE_TIMER_CRM_CLK        CRM_TMR11_PERIPH_CLOCK
#define SW_OP_INP_MODE_SELECT_CHANNEL       TMR_SELECT_CHANNEL_1
#define TMR_CH_SW_OP_INP_MODE_DMA_REQUEST   TMR_C1_DMA_REQUEST
#define TMR_SW_OP_INP_DMA                   DMA1
#define TMR_SW_OP_INP_DMA_CRM_CLK           CRM_DMA1_PERIPH_CLOCK
#define TMR_SW_OP_POS_DMA_CH                DMA1_CHANNEL7
#define TMR_SW_OP_POS_DMA_FLEX              DMAMUX_DMAREQ_ID_TMR11_CH1
#define TMR_SW_OP_POS_DMA_FLEX_CH           DMA1MUX_CHANNEL7
#define TMR_SW_OP_POS_DMA_PERIPHERAL_ADDR   SW_OP_INP_MODE_TIMER->c1dt


#define TMR_SW_OP_PORTA_DMA_CH              DMA1_CHANNEL5
#define TMR_SW_OP_PORTA_DMA_FLEX            DMAMUX_DMAREQ_ID_REQ_G1
#define TMR_SW_OP_PORTA_DMA_FLEX_CH         DMA1MUX_CHANNEL5
#define TMR_SW_OP_PORTA_DMA_PERIPHERAL_ADDR GPIOA->cfgr
#define TMR_SW_OP_PORTB_DMA_CH              DMA1_CHANNEL6
#define TMR_SW_OP_PORTB_DMA_FLEX            DMAMUX_DMAREQ_ID_REQ_G2
#define TMR_SW_OP_PORTB_DMA_FLEX_CH         DMA1MUX_CHANNEL6
#define TMR_SW_OP_PORTB_DMA_PERIPHERAL_ADDR GPIOB->cfgr


/**************** define GPIO for PWM *******************/
/***********************************************************
The definitions must be established in the following order:
    TMRx_CH1  -> PHASE_A_HI
    TMRx_CH1C -> PHASE_A_LOW
    TMRx_CH2  -> PHASE_B_HI
    TMRx_CH2C -> PHASE_B_LOW
    TMRx_CH3  -> PHASE_C_HI
    TMRx_CH3C -> PHASE_C_LOW
************************************************************/
#define PWM_PHASE_A_HI_GPIO_CRM_CLK         CRM_GPIOA_PERIPH_CLOCK
#define PWM_PHASE_A_HI_PORT                 GPIOA
#define PWM_PHASE_A_HI_GPIO_PIN             GPIO_PINS_8
#define PWM_PHASE_A_HI_PIN_SOURCE           GPIO_PINS_SOURCE8
#define PWM_PHASE_A_HI_IOMUX                GPIO_MUX_1
#define PWM_PHASE_B_HI_GPIO_CRM_CLK         CRM_GPIOA_PERIPH_CLOCK
#define PWM_PHASE_B_HI_PORT                 GPIOA
#define PWM_PHASE_B_HI_GPIO_PIN             GPIO_PINS_9
#define PWM_PHASE_B_HI_PIN_SOURCE           GPIO_PINS_SOURCE9
#define PWM_PHASE_B_HI_IOMUX                GPIO_MUX_1
#define PWM_PHASE_C_HI_GPIO_CRM_CLK         CRM_GPIOA_PERIPH_CLOCK
#define PWM_PHASE_C_HI_PORT                 GPIOA
#define PWM_PHASE_C_HI_GPIO_PIN             GPIO_PINS_10
#define PWM_PHASE_C_HI_PIN_SOURCE           GPIO_PINS_SOURCE10
#define PWM_PHASE_C_HI_IOMUX                GPIO_MUX_1
#define PWM_PHASE_A_LOW_GPIO_CRM_CLK        CRM_GPIOC_PERIPH_CLOCK
#define PWM_PHASE_A_LOW_PORT                GPIOC
#define PWM_PHASE_A_LOW_GPIO_PIN            GPIO_PINS_13
#define PWM_PHASE_A_LOW_PIN_SOURCE          GPIO_PINS_SOURCE13
#define PWM_PHASE_A_LOW_IOMUX               GPIO_MUX_10
#define PWM_PHASE_B_LOW_GPIO_CRM_CLK        CRM_GPIOB_PERIPH_CLOCK
#define PWM_PHASE_B_LOW_PORT                GPIOB
#define PWM_PHASE_B_LOW_GPIO_PIN            GPIO_PINS_8
#define PWM_PHASE_B_LOW_PIN_SOURCE          GPIO_PINS_SOURCE8
#define PWM_PHASE_B_LOW_IOMUX               GPIO_MUX_1
#define PWM_PHASE_C_LOW_GPIO_CRM_CLK        CRM_GPIOB_PERIPH_CLOCK
#define PWM_PHASE_C_LOW_PORT                GPIOB
#define PWM_PHASE_C_LOW_GPIO_PIN            GPIO_PINS_9
#define PWM_PHASE_C_LOW_PIN_SOURCE          GPIO_PINS_SOURCE9
#define PWM_PHASE_C_LOW_IOMUX               GPIO_MUX_1
#define PWM_BRK_GPIO_CRM_CLK                CRM_GPIOB_PERIPH_CLOCK
//#define PWM_BRK_PORT                        GPIOB
//#define PWM_BRK_GPIO_PIN                    GPIO_PINS_12
//#define PWM_BRK_GPIO_PIN_SOURCE             GPIO_PINS_SOURCE12
//#define PWM_BRK_IOMUX                       GPIO_MUX_2
#define TMR_BRKIN_SOURCE                    TMR_BRKIN_SOURCE_CMP1
//#define TMR_TRGO_SOURCE                     TMR_PRIMARY_SEL_C6ORAW

/**************** define Timer for Hall ******************/
/* hall sensor pin definition */
#define HALL_CAPTURE_TIMER                  TMR3
#define HALL_CAPTURE_CRM_CLK                CRM_TMR3_PERIPH_CLOCK
#define HALL_CAPTURE_IRQ                    TMR3_GLOBAL_IRQHandler
#define HALL_CAPTURE_IRQn                   TMR3_GLOBAL_IRQn
#define HALL_CAPTURE_FILTER_CLK_DIV         TMR_CLOCK_DIV2
#define TMR_HALL_IN_FILTER                  0x6                        /* 0x0 ~ 0xF */

/**************** define GPIO for Hall *******************/
#define HALL_A_GPIO_CRM_CLK                 CRM_GPIOB_PERIPH_CLOCK
#define HALL_A_PORT                         GPIOB
#define HALL_A_GPIO_PIN                     GPIO_PINS_4
#define HALL_A_GPIO_PIN_SOURCE              GPIO_PINS_SOURCE4
#define HALL_A_IOMUX                        GPIO_MUX_2
#define HALL_B_GPIO_CRM_CLK                 CRM_GPIOB_PERIPH_CLOCK
#define HALL_B_PORT                         GPIOB
#define HALL_B_GPIO_PIN                     GPIO_PINS_5
#define HALL_B_GPIO_PIN_SOURCE              GPIO_PINS_SOURCE5
#define HALL_B_IOMUX                        GPIO_MUX_2
#define HALL_C_GPIO_CRM_CLK                 CRM_GPIOB_PERIPH_CLOCK
#define HALL_C_PORT                         GPIOB
#define HALL_C_GPIO_PIN                     GPIO_PINS_3
#define HALL_C_GPIO_PIN_SOURCE              GPIO_PINS_SOURCE3
#define HALL_C_IOMUX                        GPIO_MUX_2

/**************** define GPIO for Encoder *******************/
/* encoder sensor pin definition */
#define ENCODER_MODE_TIMER                  TMR3
#define ENCODER_MODE_CRM_CLK                CRM_TMR3_PERIPH_CLOCK
#define ENCODER_A_GPIO_CRM_CLK              CRM_GPIOB_PERIPH_CLOCK
#define ENCODER_A_PORT                      GPIOB
#define ENCODER_A_GPIO_PIN                  GPIO_PINS_4
#define ENCODER_A_GPIO_PIN_SOURCE           GPIO_PINS_SOURCE4
#define ENCODER_A_IOMUX                     GPIO_MUX_2
#define ENCODER_B_GPIO_CRM_CLK              CRM_GPIOB_PERIPH_CLOCK
#define ENCODER_B_PORT                      GPIOB
#define ENCODER_B_GPIO_PIN                  GPIO_PINS_5
#define ENCODER_B_GPIO_PIN_SOURCE           GPIO_PINS_SOURCE5
#define ENCODER_B_IOMUX                     GPIO_MUX_2

#if defined ABZ || defined MAGNET_ENCODER_W_ABZ
/* encoder index */
#define ENCODER_Z_GPIO_CRM_CLK              CRM_GPIOB_PERIPH_CLOCK
#define ENCODER_Z_PORT                      GPIOB
#define ENCODER_Z_GPIO_PIN                  GPIO_PINS_3
#define ENCODER_Z_PORT_SOURCE               SCFG_PORT_SOURCE_GPIOB
#define ENCODER_Z_GPIO_PIN_SOURCE           SCFG_PINS_SOURCE3
#define ENCODER_Z_EXINT_CRM_CLK             CRM_SCFG_PERIPH_CLOCK
#define ENCODER_Z_EXINT_LINE                EXINT_LINE_3
#define EXINT_ENCODER_IDX_IRQ               EXINT3_IRQHandler
#define EXINT_ENCODER_IDX_IRQn              EXINT3_IRQn
#endif

/**************** define Timer for pwm input *******************/
#define PWM_DUTY_INPUT_TIMER                TMR4
#define PWM_DUTY_INPUT_SELECT_CHANNEL       TMR_SELECT_CHANNEL_1          
#define PWM_DUTY_INPUT_CRM_CLK              CRM_TMR4_PERIPH_CLOCK
#define PWM_DUTY_INPUT_IRQ                  TMR4_GLOBAL_IRQHandler
#define PWM_DUTY_INPUT_IRQn                 TMR4_GLOBAL_IRQn
#define PWM_DUTY_INPUT_FLAG                 TMR_C1_FLAG
#define PWM_DUTY_INPUT_INT                  TMR_C1_INT
#define TMR_PWM_DUTY_INPUT_FILTER           0xF												/* 0x0 ~ 0xF */

#define PWM_DUTY_INPUT_GPIO_CRM_CLK         CRM_GPIOF_PERIPH_CLOCK
#define PWM_DUTY_INPUT_PORT                 GPIOF
#define PWM_DUTY_INPUT_GPIO_PIN             GPIO_PINS_9
#define PWM_DUTY_INPUT_GPIO_PIN_SOURCE      GPIO_PINS_SOURCE9
#define PWM_DUTY_INPUT_IOMUX                GPIO_MUX_2

/**************** define Synchronous Timer for read Magnetic encoder value *******************/
#define SYNC_TIMER                          TMR9
#define SYNC_TIMER_SELECT_CHANNEL           TMR_SELECT_CHANNEL_1
#define SYNC_TIMER_CRM_CLK                  CRM_TMR9_PERIPH_CLOCK
#define SYNC_TIMER_INT_SOURCE               TMR_SUB_INPUT_SEL_IS0
#define SYNC_TIMER_CH_INT                   TMR_C1_INT
#define SYNC_TIMER_CH_FLAG                  TMR_C1_FLAG
#define SYNC_TIMER_CH_IRQ                   TMR1_BRK_TMR9_IRQHandler
#define SYNC_TIMER_CH_IRQn                  TMR1_BRK_TMR9_IRQn

/**************** define Timer for speed control loop *******************/
#define SPEED_LOOP_TIMER                    TMR6
#define SPEED_LOOP_TIMER_CRM_CLK            CRM_TMR6_PERIPH_CLOCK
#define SPEED_LOOP_TIMER_IRQ                TMR6_DAC_GLOBAL_IRQHandler
#define SPEED_LOOP_TIMER_IRQn               TMR6_DAC_GLOBAL_IRQn
/* adc reading pin definition */
#define ADC_CONVERTER                       ADC1
#define ADC_CONVERTER_CRM_CLK               CRM_ADC1_PERIPH_CLOCK
#define ADC_CONVERTER_CRM_CLK_DIV           ADC_HCLK_DIV_6
#define ADC_SHUNT_SAMP_READY_IRQ            ADC1_2_IRQHandler
#define ADC_SHUNT_SAMP_READY_IRQn           ADC1_2_IRQn
#define ADC_ORDINARY_CH_LEN                 ADC_IDX_MAX
#define ADC_PREEMPT_SAMPLETIME              ADC_SAMPLETIME_1_5
#define ADC_SIMULTANE_CONVERTER             ADC2
#define ADC_SIMULTANE_CONVERTER_CRM_CLK     CRM_ADC2_PERIPH_CLOCK

/* dma1 ch1 for adc ordinary conversion */
#define ADC_ORDINARY_DMA_CRM_CLK            CRM_DMA1_PERIPH_CLOCK
#define ADC_ORDINARY_DMA_CHANNEL            DMA1_CHANNEL1
#define ADC_ORDINARY_DMA                    DMA1
#define ADC_ORDINARY_DMA_FLEX               DMAMUX_DMAREQ_ID_ADC1
#define ADC_ORDINARY_DMA_FLEX_CH            DMA1MUX_CHANNEL1
#define ADC_ORDINARY_DMA_FT_STS_FLAG        DMA1_FDT1_FLAG

#define CURR_PHASE_A_ADC_CH                 ADC_CHANNEL_12
#define CURR_PHASE_A_ADC_GPIO_CRM_CLK       CRM_GPIOB_PERIPH_CLOCK
#define CURR_PHASE_A_ADC_PORT               GPIOB
#define CURR_PHASE_A_ADC_GPIO_PIN           GPIO_PINS_12

#define CURR_PHASE_B_ADC_CH                 ADC_CHANNEL_2
#define CURR_PHASE_B_ADC_GPIO_CRM_CLK       CRM_GPIOA_PERIPH_CLOCK
#define CURR_PHASE_B_ADC_PORT               GPIOA
#define CURR_PHASE_B_ADC_GPIO_PIN           GPIO_PINS_2

//#define CURR_PHASE_C_ADC_CH                 ADC_CHANNEL_2
//#define CURR_PHASE_C_ADC_GPIO_CRM_CLK       CRM_GPIOA_PERIPH_CLOCK
//#define CURR_PHASE_C_ADC_PORT               GPIOA
//#define CURR_PHASE_C_ADC_GPIO_PIN           GPIO_PINS_2

#define CURR_BUS_ADC_CH                     ADC_CHANNEL_9
#define CURR_BUS_ADC_GPIO_CRM_CLK           CRM_GPIOB_PERIPH_CLOCK
#define CURR_BUS_ADC_PORT                   GPIOB
#define CURR_BUS_ADC_GPIO_PIN               GPIO_PINS_1

#define VOLT_BUS_ADC_CH                     ADC_CHANNEL_0
#define VOLT_BUS_ADC_GPIO_CRM_CLK           CRM_GPIOA_PERIPH_CLOCK
#define VOLT_BUS_ADC_PORT                   GPIOA
#define VOLT_BUS_ADC_GPIO_PIN               GPIO_PINS_0

#define MOS_TEMP_ADC_CH                     ADC_CHANNEL_24
#define MOS_TEMP_ADC_GPIO_CRM_CLK           CRM_GPIOF_PERIPH_CLOCK
#define MOS_TEMP_ADC_PORT                   GPIOF
#define MOS_TEMP_ADC_GPIO_PIN               GPIO_PINS_10

#define POTENTIO_ADC_CH                     ADC_CHANNEL_26
#define POTENTIO_ADC_GPIO_CRM_CLK           CRM_GPIOF_PERIPH_CLOCK
#define POTENTIO_ADC_PORT                   GPIOF
#define POTENTIO_ADC_GPIO_PIN               GPIO_PINS_12

//#define IBUS_AVG_ADC_CH                     ADC_CHANNEL_9
//#define IBUS_AVG_ADC_GPIO_CRM_CLK           CRM_GPIOB_PERIPH_CLOCK
//#define IBUS_AVG_ADC_PORT                   GPIOB
//#define IBUS_AVG_ADC_GPIO_PIN               GPIO_PINS_1

#if defined VOLT_SENSE || defined WIND_SENSE
#define BEMF_A_ADC_CH                       ADC_CHANNEL_4
#define BEMF_A_ADC_GPIO_CRM_CLK             CRM_GPIOA_PERIPH_CLOCK
#define BEMF_A_ADC_PORT                     GPIOA
#define BEMF_A_ADC_GPIO_PIN                 GPIO_PINS_4

#define BEMF_B_ADC_CH                       ADC_CHANNEL_5
#define BEMF_B_ADC_GPIO_CRM_CLK             CRM_GPIOA_PERIPH_CLOCK
#define BEMF_B_ADC_PORT                     GPIOA
#define BEMF_B_ADC_GPIO_PIN                 GPIO_PINS_5

#define BEMF_C_ADC_CH                       ADC_CHANNEL_6
#define BEMF_C_ADC_GPIO_CRM_CLK             CRM_GPIOA_PERIPH_CLOCK
#define BEMF_C_ADC_PORT                     GPIOA
#define BEMF_C_ADC_GPIO_PIN                 GPIO_PINS_6
#endif

/* define over current comparator input */
#define BUS_CURR_CMP_GPIO_CRM_CLK           CRM_GPIOB_PERIPH_CLOCK
#define BUS_CURR_CMP_PORT                   GPIOB
#define BUS_CURR_CMP_GPIO_PIN               GPIO_PINS_1
#define BUS_CURR_CMP                        CMP1_SELECTION
#define BUS_CURR_CMP_NON_INVERTING_INP      CMP_NON_INVERTING_INP3
#define BUS_CURR_CMP_INVERTING_INM          CMP_INVERTING_INM2
//#define BUS_CURR_CMP_BLANKING_SOURCE        CMP_BLANKING_TMR1_CH5

/**************** define comm uart Tx and Rx ******************/
#define COMM_UART                           USART1
#define COMM_UART_CRM_CLK                   CRM_USART1_PERIPH_CLOCK
#define COMM_UART_TX_GPIO_CRM_CLK           CRM_GPIOB_PERIPH_CLOCK
#define COMM_UART_TX_PORT                   GPIOB
#define COMM_UART_TX_GPIO_PIN_SOURCE        GPIO_PINS_SOURCE6
#define COMM_UART_TX_PIN                    GPIO_PINS_6
#define COMM_UART_TX_IOMUX                  GPIO_MUX_7
#define COMM_UART_RX_GPIO_CRM_CLK           CRM_GPIOB_PERIPH_CLOCK
#define COMM_UART_RX_PORT                   GPIOB
#define COMM_UART_RX_GPIO_PIN_SOURCE        GPIO_PINS_SOURCE7
#define COMM_UART_RX_PIN                    GPIO_PINS_7
#define COMM_UART_RX_IOMUX                  GPIO_MUX_7
#define COMM_UART_IOMUX                     (uint32_t)NULL
#define COMM_UART_IRQn                      USART1_IRQn
#define COMM_UART_IRQHandler                USART1_IRQHandler

/**************** define DMA for uart Tx and Rx ******************/
#define DMA_UART                            DMA1
#define DMA_UART_CRM_CLK                    CRM_DMA1_PERIPH_CLOCK
#define DMA_UART_RX_TX_CHANNEL_IRQn         DMA1_Channel3_2_IRQn
#define DMA_UART_RX_TX_IRQHandler           DMA1_Channel3_2_IRQHandler
#define DMA_UART_TX_CHANNEL                 DMA1_CHANNEL2
#define DMA_UART_TX_FDT_FLAG                DMA1_FDT2_FLAG
#define DMA_UART_TX_FLEX_CHANNEL            DMA1MUX_CHANNEL2
#define DMA_UART_TX_FLEX                    DMAMUX_DMAREQ_ID_USART1_TX
#define DMA_UART_RX_CHANNEL                 DMA1_CHANNEL3
#define DMA_UART_RX_FDT_FLAG                DMA1_FDT3_FLAG
#define DMA_UART_RX_FLEX_CHANNEL            DMA1MUX_CHANNEL3
#define DMA_UART_RX_FLEX                    DMAMUX_DMAREQ_ID_USART1_RX

/******************* define led *******************/
/* error led state */
#define ERROR_LED_GPIO_CRM_CLK              CRM_GPIOC_PERIPH_CLOCK
#define ERROR_LED_PORT                      GPIOC
#define ERROR_LED_GPIO_PIN                  GPIO_PINS_14

/* operating status LEDs */
#define STATUS1_LED_GPIO_CRM_CLK            CRM_GPIOC_PERIPH_CLOCK
#define STATUS1_LED_PORT                    GPIOC
#define STATUS1_LED_GPIO_PIN                GPIO_PINS_15

/******************* define button *******************/
typedef enum
{
  USER_BUTTON                               = 0,
  NO_BUTTON                                 = 1
} button_type;

//#define USER_BUTTON_PIN                     GPIO_PINS_13
//#define USER_BUTTON_PORT                    GPIOF
//#define USER_BUTTON_CRM_CLK                 CRM_GPIOF_PERIPH_CLOCK

//#define BUTTON_PORT_SOURCE                  SCFG_PORT_SOURCE_GPIOF
//#define BUTTON_PIN_SOURCE                   SCFG_PINS_SOURCE13
//#define BUTTON_EXINT_CRM_CLK                CRM_SCFG_PERIPH_CLOCK
//#define BUTTON_EXINT_LINE                   EXINT_LINE_13
//#define BUTTON_EXINT_IRQn                   EXINT15_10_IRQn
//#define BUTTON_EXINT_IRQHandler             EXINT15_10_IRQHandler

//#define HALL_LEARN_BUTTON_PIN               GPIO_PINS_13
//#define HALL_LEARN_BUTTON_PORT              GPIOF
//#define HALL_LEARN_BUTTON_CRM_CLK           CRM_GPIOF_PERIPH_CLOCK

//#define HALL_LEARN_BUTTON_PORT_SOURCE       SCFG_PORT_SOURCE_GPIOB
//#define HALL_LEARN_BUTTON_PIN_SOURCE        SCFG_PINS_SOURCE15
//#define HALL_LEARN_BUTTON_EXINT_CRM_CLK     CRM_SCFG_PERIPH_CLOCK
//#define HALL_LEARN_BUTTON_EXINT_LINE        EXINT_LINE_15

//#define REVERSE_SW_PIN                      GPIO_PINS_14
//#define REVERSE_SW_PORT                     GPIOB
//#define REVERSE_SW_CRM_CLK                  CRM_GPIOB_PERIPH_CLOCK

//#define BRAKE_SW_PIN                        GPIO_PINS_13
//#define BRAKE_SW_PORT                       GPIOB
//#define BRAKE_SW_CRM_CLK                    CRM_GPIOB_PERIPH_CLOCK

//#define LOCK_MOTOR_SW_PIN                   GPIO_PINS_9
//#define LOCK_MOTOR_SW_PORT                  GPIOF
//#define LOCK_MOTOR_SW_CRM_CLK               CRM_GPIOF_PERIPH_CLOCK

//#define PARKING_LOCK_SW_PIN                 GPIO_PINS_9
//#define PARKING_LOCK_SW_PORT                GPIOF
//#define PARKING_LOCK_SW_CRM_CLK             CRM_GPIOF_PERIPH_CLOCK

//#define MODE1_BUTTON_PIN                    GPIO_PINS_14
//#define MODE1_BUTTON_PORT                   GPIOB
//#define MODE1_BUTTON_CRM_CLK                CRM_GPIOB_PERIPH_CLOCK

//#define MODE2_BUTTON_PIN                    GPIO_PINS_13
//#define MODE2_BUTTON_PORT                   GPIOB
//#define MODE2_BUTTON_CRM_CLK                CRM_GPIOB_PERIPH_CLOCK

#define CAN_STB_PIN                         GPIO_PINS_15
#define CAN_STB_PORT                        GPIOA
#define CAN_STB_CRM_CLK                     CRM_GPIOA_PERIPH_CLOCK

#define EMF_CTRL_PIN                        GPIO_PINS_8
#define EMF_CTRL_PORT                       GPIOF
#define EMF_CTRL_CRM_CLK                    CRM_GPIOF_PERIPH_CLOCK

#define TEST_PIN1_PIN                       GPIO_PINS_13
#define TEST_PIN1_PORT                      GPIOF
#define TEST_PIN1_CRM_CLK                   CRM_GPIOF_PERIPH_CLOCK

/* opa pins definition */
#define OP1_INP_PIN                         GPIO_PINS_1
#define OP1_INP_PORT                        GPIOA
#define OP1_INP_CRM_CLK                     CRM_GPIOA_PERIPH_CLOCK

#define OP1_INM_PIN                         GPIO_PINS_3
#define OP1_INM_PORT                        GPIOA
#define OP1_INM_CRM_CLK                     CRM_GPIOA_PERIPH_CLOCK

//#define OP2_INP_PIN                         GPIO_PINS_7
//#define OP2_INP_PORT                        GPIOA
//#define OP2_INP_CRM_CLK                     CRM_GPIOA_PERIPH_CLOCK

//#define OP2_INM_PIN                         GPIO_PINS_5
//#define OP2_INM_PORT                        GPIOA
//#define OP2_INM_CRM_CLK                     CRM_GPIOA_PERIPH_CLOCK

#define OP3_INP_PIN                         GPIO_PINS_0
#define OP3_INP_PORT                        GPIOB
#define OP3_INP_CRM_CLK                     CRM_GPIOB_PERIPH_CLOCK

#define OP3_INM_PIN                         GPIO_PINS_2
#define OP3_INM_PORT                        GPIOB
#define OP3_INM_CRM_CLK                     CRM_GPIOB_PERIPH_CLOCK

#define OP4_INP_PIN                         GPIO_PINS_11
#define OP4_INP_PORT                        GPIOB
#define OP4_INP_CRM_CLK                     CRM_GPIOB_PERIPH_CLOCK

#define OP4_INM_PIN                         GPIO_PINS_10
#define OP4_INM_PORT                        GPIOB
#define OP4_INM_CRM_CLK                     CRM_GPIOB_PERIPH_CLOCK

typedef enum
{
#if defined VOLT_SENSE || defined WIND_SENSE
  ADC_BEMF_A_IDX,
  ADC_BEMF_B_IDX,
  ADC_BEMF_C_IDX,
#endif
  ADC_BUS_VOLT_IDX,
  ADC_MOS_TEMP_IDX,
  ADC_POTENTIO_IDX,
  //ADC_IBUS_AVE_IDX,
  ADC_IDX_MAX
} adc_in_idx;

extern __IO uint16_t adc_in_tab[ADC_IDX_MAX];

void nvic_config(void);
void tmr_pwm_init(void);
void adc_ordinary_config(void);
void adc_preempt_config(void);
void encoder_timer_init(void);
void magnetic_encoder_timer_init(void);
void hall_timer_init(void);
void speed_timer_init(void);
void brake_pwm_init(void);
void uart_init(usart_config_type *usart_config);
void opa_calibration(op_type* op_num);

/******************** functions ********************/
/* led operation function */
void led_init(void);
void led_on(gpio_type *led_gpio_port, uint16_t led_gpio_pin);
void led_off(gpio_type *led_gpio_port, uint16_t led_gpio_pin);
void led_toggle(gpio_type *led_gpio_port, uint16_t led_gpio_pin);
void led_config(void);
void led_blink(void);

/* mode switch configuration */
void mode_switch_init(void);

/* button operation function */
void button_exint_init(void);

/* initial angle detection configuration */
void foc_angle_init_config(void);

/* motor parameter identify configuration */
void motor_parameter_ID_config(void);

/* get internal vref function */
void get_int_vref_cal_ratio(void);

/* genernal purpos of input and output pins configuration */
void gpio_pins_init(void);

/* opa function initialization */
void opa_init(void);

/* compararor function initialization */
void ocp_cmp_config(void);

/* break input sensing function */
void brake_input_handler(void);

/* clear opa inp pin output function in digital output mode */
void opa_inp_gpio_clr(void);

/* initial timer for trig dma to switch op inp pin mode */
void sw_op_inp_mode_timer_init(void);

/* timer init function for PWM input */
void pwm_in_timer_init(void);

#ifdef __cplusplus
}
#endif

#endif
