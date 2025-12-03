/**
  **************************************************************************
  * @file     motor_control_drive_param.h
  * @brief    Motor-related, drive-related and control-related parmeters, such as number of motor poles, maximum sensing voltage/current and pid speed parameters
  *           User defined motor drive modes (current sampling mode, sensor mode, etc.)
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

#ifndef __MOTOR_CONTROL_DRIVE_PARAM_H
#define __MOTOR_CONTROL_DRIVE_PARAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mc_lib.h"

/* internal clock or external crytal */
//#define INTERNAL_CLOCK_SOURCE

/* choose hardware version */
#define M412_LV_V1_0

/* mosfet low side complement(high side PWM) setting */
#define COMPLEMENT
/* gate driver low side inverting logic input or non-inverting logic input */
//#define GATE_DRIVER_LOW_SIDE_INVERT

/* choose sensor */
//#define HALL_SENSORS
#define SENSORLESS
//#define BLDC_SENSORLESS_ADC
#define BLDC_SENSORLESS_COMP

/* use internal comparator of MCU */
#ifdef BLDC_SENSORLESS_COMP
#define INTERNAL_COMP
#endif

/* choose remove current-loop-control or not */
//#define WITHOUT_CURRENT_CTRL

/* choose low speed control or not */
#define LOW_SPEED_VOLT_CTRL

/* Enable current signal low-pass filtering mode or not */
#define CURRENT_LP_FILTER

#if defined SENSORLESS
/* choose large cogging or not 大齿锯提前补偿相位？ */
//#define LARGE_COGGING

/* choose phase advance or not 相位提前（默认不开启）。使用INIT_ANGLE_STARTUP启动时才需要设置，否则可以略过。使用初始角度检测启动时会因为电机特性是否大齿槽转矩导致检测方式的不同，因此需要根据电机特性设置此功能 */
//#define PHASE_ADVANCE

/* choose const current/voltage start-up */
//#define CONST_CURRENT_START
#define CONST_VOLTAGE_START

/* choose how to start up */
//#define INIT_ANGLE_STARTUP
//#define ALIGN_AND_GO_STARTUP
#define OPENLOOP_STARTUP
#endif

#ifdef BLDC_SENSORLESS_COMP
/* EMF continous sample */
#define EMF_CONTINOUS_SAMPLE
#define SPECIFIC_EACH_EMF_PIN     //避开其它相不需要比较器信号（默认开启）
#elif defined BLDC_SENSORLESS_ADC
/* choose EMF pull up or not */
#define EMF_PULL_UP
#endif

/* choose Winding parameter identification or not 电机线圈参数自动辨识（默认开启，若注释则关闭） */
//#define MOTOR_PARAM_IDENTIFY

/* use artery motor monitor or not */
#define USE_MOTOR_MONITOR   /* CTRL_SOURCE should be changed to CTRL_SOURCE_EXTERNAL if no motor monitor is used. */

/* use pwm input or not */
//#define PWM_INPUT

/********************************* Motor-related parameter *********************************/
/* Motor parameters  */
#define RS_LL                        (0.31)           /* Stator resistance, ohm */
#define LS_LL                        (0.0092)         /* Stator inductance, H */
#define POLE_PAIRS                   (14/2)
#define KE                           (0.001443f)      /* Back EMF constant(line-to-line, peak voltage), V/rpm */
#define NOMINAL_CURRENT              (20.0)

/* angle detect duty */
#define ANGLE_INIT_DETECT_DUTY       ((int16_t)(0.15*ANGLE_INIT_PERIOD))  /* 初始角度检测的PWM DUTY值（unit: PWM计时器时基） */
#define ANGLE_INIT_I_DIFF            ((int16_t) 500)                      /* 初始角度检测的电流差（unit: ADC数字量） */

/* hall learn table */
#if defined (SENSORLESS)
#define HALL_LEARN_DIR               (0)          /* Polarity, 0 or 1 */
#define HALL_LEARN_0_STATE           (2)          /* BH-CL */
#define HALL_LEARN_1_STATE           (3)          /* BH-AL */
#define HALL_LEARN_2_STATE           (1)          /* CH-AL */
#define HALL_LEARN_3_STATE           (5)          /* CH-BL */
#define HALL_LEARN_4_STATE           (4)          /* AH-BL */
#define HALL_LEARN_5_STATE           (6)          /* AH-CL */
#else
#define HALL_LEARN_DIR               (0)          /* Polarity, 0 or 1 */
#define HALL_LEARN_0_STATE           (1)          /* BH-CL */
#define HALL_LEARN_1_STATE           (5)          /* BH-AL */
#define HALL_LEARN_2_STATE           (4)          /* CH-AL */
#define HALL_LEARN_3_STATE           (6)          /* CH-BL */
#define HALL_LEARN_4_STATE           (2)          /* AH-BL */
#define HALL_LEARN_5_STATE           (3)          /* AH-CL */
#endif

/********************************* Drive-related parameter *********************************/
/* basic */
#define VDC_RATED                 ((double)16.8f)
#define BAT_LOW_VOLT              ((double)12.8f)                    /*!< minimum allowable battery voltage*/
#define V_SENSE_GAIN              (10.0f/(47.0f+10.0f))              /*!< 0.175439 */
#define MAX_CURRENT               (20.0f)                            /*!< 控制层的电流限幅（单位：A）。用于限制电流指令和控制器输出的上下界， */
#define MIN_CURRENT               (-MAX_CURRENT)                     /*!< 防止调节值超出允许范围，但不是保护触发阈值 */
#define CURRENT_SPAN_SHIFT        (1)
#define ADC_REFERENCE_VOLT        (3.271f)                           /*!< V */
#define ADC_DIGITAL_SCALE_12BITS  (4095.0f)
/* Clock */
#if defined AT32F413xx
#define SYSTEM_CORE_CLOCK               (200000000)
#elif defined AT32F421xx
#define SYSTEM_CORE_CLOCK               (120000000)
#elif defined AT32F415xx || defined AT32F423xx
#define SYSTEM_CORE_CLOCK               (150000000)
#elif defined AT32F403Axx || defined AT32F407xx
#define SYSTEM_CORE_CLOCK               (240000000)
#elif defined AT32F435xx || defined AT32F437xx
#define SYSTEM_CORE_CLOCK               (288000000)
#elif defined AT32F425xx
#define SYSTEM_CORE_CLOCK               (96000000)
#elif defined AT32L021xx
#define SYSTEM_CORE_CLOCK               (80000000)
#elif defined AT32M412xx || defined AT32M416xx || defined AT32F422xx || defined AT32F426xx
#define SYSTEM_CORE_CLOCK               (180000000)
#elif defined AT32F455xx || defined AT32F456xx || defined AT32F457xx 
#define SYSTEM_CORE_CLOCK               (192000000)
#elif defined AT32F402xx || defined AT32F405xx
#define SYSTEM_CORE_CLOCK               (216000000)
#endif
#define TMR_CLK                   (SYSTEM_CORE_CLOCK)
#define CHANGE_PHASE_TMR_DIV      (TMR_CLK/2/1000000)                /*!< 0.5usec/per cval */
/* Dead-time inserted */
#define DEADTIME_CLK_SFT_BITS     ((tmr_clock_division_type)2)
#define DEADTIME_NS               ((uint16_t)200)                    /* in nsec; check the MOSFET/IGBT/IPM specification
                                                                        SYSTEM_CORE_CLOCK = 240MHz, range is [0...2000];
                                                                        SYSTEM_CORE_CLOCK = 200MHz, range is [0...2500];
                                                                        SYSTEM_CORE_CLOCK = 150MHz, range is [0...3000];
                                                                        SYSTEM_CORE_CLOCK = 120MHz, range is [0...4000]; */
/* Current */
#define R_SHUNT                   (0.002f)                                                    /*!< 0.002 ohm */
#define OP_GAIN                   (66.5f/2.4f)                                                /*!< gain = 27.708 */
#define CURR_OFFSET_VOLT          (ADC_REFERENCE_VOLT/2.0f)                                   /*!< 1.65 V */

/* EMF */
#define EMF_SENSE_GAIN            (3.0f/(43.0f+3.0f))   /*!< 0.0652 */
#define GATE_DELAY_TIME           (0.8)          /*!< usec */   
#define EMF_SIG_RISING_TIME       (9.8)          /*!< usec */
#define EMF_SIG_FALLING_TIME      (21.5)         /*!< usec */ 

#define EMF_RISE_BLANK_TIME           (31.0f)        /*!< usec */
#define EMF_FALL_BLANK_TIME_HIGH_SPD  (54.0f)        /*!< usec */
#define EMF_FALL_BLANK_TIME_LOW_SPD   (23.5f)        /*!< usec */
#define EMF_BLANK_TIME_CHANGED_RPM    (1000.0f)      /*!< rpm */
#define EMF_MIN_VALUE                 (90)

/* Only sensorless-ADC need to set */
#ifdef EMF_PULL_UP
#define EMF_LOW_SPD_OFFSET_RISING       ((int16_t)(750))
#define EMF_LOW_SPD_OFFSET_FALLING      ((int16_t)(680))
#else
#define EMF_LOW_SPD_OFFSET_RISING       ((int16_t)(30))
#define EMF_LOW_SPD_OFFSET_FALLING      ((int16_t)(10))
#endif
#define EMF_HIGH_SPD_OFFSET_RISING      ((int16_t)(-100))
#define EMF_HIGH_SPD_OFFSET_FALLING     ((int16_t)(-100))

/***************** Protection *****************/
/* Current */
#define OVER_CURRENT_SW           (30.0)       /*!< A */

// #if defined AT32M412xx || defined AT32M416xx
// #define DAC_VREF_SOURCE           (DAC_VDDA)
// #define OCP_CURRENT               (50.0)       /*!< A */
// #define BUS_CURR_CMP_OCP_VOLT     (OCP_CURRENT*R_SHUNT*OP_GAIN+CURR_OFFSET_VOLT)
// #define TMR_BRK_FILTER_COUNT      (3)
// #endif

/* Bus voltage */
#define OVER_VOLT_THRESHOLD       (17)         /*!< V */
#define UNDER_VOLT_THRESHOLD      (12)          /*!< V */
/* Temperature sensing section */
/* V[V]=V0+dV/dT[V/Celsius]*(T-T0)[Celsius]*/
#define V0_V                      (2.81943)    /*!< in Volts */
#define T0_C                      (0)          /*!< in Celsius degrees */
#define dV_dT                     (-0.0242314) /*!< V/Celsius degrees */
#define OVER_TEMP_THRESHOLD       (70)         /*!< Celsius degrees */
/* error code mask */
#define MC_ERROR_MASK             (err_code_type) (MC_OVER_VOLT_ERROR | MC_UNDER_VOLT_ERROR | MC_OVER_TEMP_ERROR | MC_OVER_CURRENT_ERROR | MC_ENCODER_ERROR | MC_HALL_ERROR | MC_PARAM_IDENT_ERROR | MC_HALL_LEARN_ERROR)






/********************************* Control-related parameter *********************************/
#define PWM_FREQ                   (20000)                            /*!< Hz */
#define MOTOR_CONTROL_MODE         (motor_control_mode)(SPEED_CTRL)
#define CTRL_SOURCE                (ctrl_source_type)(CTRL_SOURCE_SOFTWARE)  /* 1.CTRL_SOURCE_SOFTWARE 2.CTRL_SOURCE_EXTERNAL */
#define UI_UART_BAUDRATE           (1500000UL) /*!< bit/s */
/* I-TUNE PARAMETER */
#define TUNE_TARGET_CURRENT        (0.5)      /*!< A */
#define TUNE_CURRENT_TOTAL_PERIOD  (50)       /*!< msec */
#define TUNE_CURRENT_STEP_PERIOD   (5)        /*!< msec */
/* I-SAMPLE PARAMETER */
#define I_SAMP_MIN_TIME            (180)      /*!< nsec */
#define I_SAMP_DELAY               (1000)     /*!< nsec */
#define CURR_LP_BANDWIDTH          (2000.0f)  /*!< 2*pi*freq */
/* EMF-SAMPLE PARAMETER */
#define SENSE_HALL_TIMES           (8)
#define EMF_CHK_TIMES              (4)
#define REBOOT_PERIOD_MS           (1000)     /*!< msec */
/* START-UP PARAMETER */
#define START_CURRENT              (0.2)      /*!< A */
#define START_VOLTAGE              (1.5)      /*!< V */
/* SPEED */
#define SPEED_FILTER_TIMES         (6)
#define SPD_LP_BANDWIDTH           (300.0f)   /* 2*pi*freq */
#define PWM_IN_FILTER_TIMES        (5)
#define SPD_CMD_FILTER_TIMES       (5)
/* SPEED PARAMETER */
#if defined LOW_SPEED_VOLT_CTRL
#if defined SENSORLESS
#define MIN_SPEED_RPM              (100)      /*!< rpm */
#elif defined HALL_SENSORS 
#define MIN_SPEED_RPM              (120)      /*!< rpm */
#endif
#else
#define MIN_SPEED_RPM              (350)      /*!< rpm */
#endif
#define MAX_SPEED_RPM              (7200)     /*!< rpm */
#define MAX_CCW_SPEED_RPM          (7200)     /*!< rpm */

#define ACC_SPD_SLOPE              (5)        /*!< rpm/ms */
#define DEC_SPD_SLOPE              (5)

#if defined SENSORLESS
#define MIN_SENSE_SPEED            (100)      /*!< Lowest computable speed : 60*TMR_CLK*6/POLE_PAIRS/0x7FFFFFFFL */
#else
#define MIN_SENSE_SPEED            (10)
#endif
/* PWM INPUT(external speed command calculation) */
#define PWM_IN_START_DUTY          ((int16_t) (0.06f*32767))
#define PWM_IN_STOP_DUTY           ((int16_t) (0.05f*32767))
/* brake duty */
#define BRAKE_DUTY                 (30)      /*!< brake force(%)*/
#define DRAG_BRAKE_DUTY            (30)      /*!< drag brake force(%)*/
/* Beep sound */
#define TONE_FREQ                  (8000)
#define TONE_AMP                   (4000)
#define TONE_PERIOD                (100)
/* SP */
#define SP_MAX_VOLT                (3.3f)    /*!< V */
#define SP_THRESHOLD               (0.1f)    /*!< V */
#define SP_RUN_VALUE               (0.2f)    /*!< V */
#define SP_STOP_VALUE              (0.11f)   /*!< V */
/* open loop control */
#define OLC_INIT_SPD               (100)     /*!< rpm */
#define OLC_FINAL_SPD              (600)     /*!< rpm */
#define OLC_TIMES                  (200)     /*!< Accumulated 200 times to reach the final speed */
#define OLC_INIT_VOLT              (0.14)    /*!< V */
#define OLC_VOLT_INC               (0.003)
/* open loop start-up */
#define OLC_STARTUP_PERIOD         (1500)     /*!< msec */
/* align and go start-up */
#define LOCK_VOLT                  (2.5)     /*!< 启动前转子对齐电压V */
#define LOCK_PERIOD                (500)     /*!< 启动前转子对齐时间msec */
/* hall learning可能用于霍尔自学习，用不上 */
#define LEARN_TIME                 (10000)   /*!< msec */
#define LEARN_ALIGN_TIME           (500)     /*!< msec */
/* pi parameter */
#define PID_IS_KP_DEFUALT          31000
#define PID_IS_KI_DEFUALT          2600
#define PID_IS_KP_DIV              8192
#define PID_IS_KP_DIV_LOG          LOG2(PID_IS_KP_DIV)
#define PID_IS_KI_DIV              32768
#define PID_IS_KI_DIV_LOG          LOG2(PID_IS_KI_DIV)

#define PID_SPD_KP_DEFUALT         5000
#define PID_SPD_KI_DEFUALT         100
#define PID_SPD_KP_DIV             1024
#define PID_SPD_KP_DIV_LOG         LOG2(PID_SPD_KP_DIV)
#define PID_SPD_KI_DIV             1024
#define PID_SPD_KI_DIV_LOG         LOG2(PID_SPD_KI_DIV)

/* low speed voltage control parameter */
#define HYSTERESIS_LOW_SPEED       (600)     /*!< 切入高速电流环控制切换点rpm */
#define HYSTERESIS_HIGH_SPEED      (800)     /*!< 切入低速电流环控制切换点rpm */
/* low speed pid parameter for voltage control WITHOUT CURRENT-LOOP专用 */
#define PID_SPD_VOLT_KP_DEFUALT         5000
#define PID_SPD_VOLT_KI_DEFUALT         100
#define PID_SPD_VOLT_KP_GAIN_DIV        1024
#define PID_SPD_VOLT_KP_GAIN_DIV_LOG    LOG2(PID_SPD_VOLT_KP_GAIN_DIV)
#define PID_SPD_VOLT_KI_GAIN_DIV        1024
#define PID_SPD_VOLT_KI_GAIN_DIV_LOG    LOG2(PID_SPD_VOLT_KI_GAIN_DIV)

/* EMF sample */
#define EMF_CHANGE_PERCENT_H            (55)        /*!< pwm duty(%) */
#define EMF_CHANGE_PERCENT_L            (35)        /*!< pwm duty(%) */
#define MIN_SAMPLE_INTERVAL_US          (1.3f)      /*!< EMF采样触发的最小防护间隔时间，避免采样过于靠近PWM开关边缘 us, >= 1us */
#define EMF_SAMPLE_LEAD_PWM             (0.02f*PWM_PERIOD + MIN_SAMPLE_INTERVAL)  //定义EMF采样的提前量（以PWM计数为单位）
#define EMF_HIGH_SPD_SAMPLE_CNT         (0.1f*PWM_PERIOD)    /*!< 高速工况下的EMF采样计数/时间窗，10% pwm duty */
#ifdef BLDC_SENSORLESS_ADC
#define EMF_PHASE_ADV_SPD               (15000)     /*!< rpm */
#else
#define EMF_PHASE_ADV_SPD               (30000)     /*!< rpm */
#endif
#define EMF_MIN_DELAY_US                (10)        /*!< usec */
#define EMF_AVOID_NOISE_INIT_PERIOD     (1)         /*!< msec */

#ifdef EMF_CONTINOUS_SAMPLE
#define EMF_SAMPLE_INTERVAL             (0.05f*PWM_PERIOD)  /*!< 5% pwm duty*/
#define SENSE_GPIN_DELAY                (100)       /*!< the delay from ovf of emf timer to sense input pins of emf. comparators*/
#define EMF_LOW_SPD_CONT_SAMPLE_END     (PWM_PERIOD - EMF_SAMPLE_INTERVAL - SENSE_GPIN_DELAY)
#define EMF_AVOID_NOISE_TIMES           (2)
#else
#define EMF_AVOID_NOISE_TIMES           (2)
#endif

/* led blink setting */
#define LED_BLINK_PERIOD                (500)    /*!< ms */
/* current pid auto-tune */
#define AUTO_TUNE_CURR_BANDWIDTH        (3000)   /*!< 2*pi*freq */

#ifdef __cplusplus
}
#endif

#endif

