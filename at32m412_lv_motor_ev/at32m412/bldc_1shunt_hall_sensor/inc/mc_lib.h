/**
  **************************************************************************
  * @file     mc_lib.h
  * @brief    Unified header file management
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

#ifndef __MC_LIB_H
#define __MC_LIB_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************** Definition (DON'T EDIT) ***********************/
#define SIX_STEP_CONTROL
#define ONE_SHUNT
/* include file */
#include <stdlib.h>
#include "stdio.h"
#include "at32m412_416.h"
#include "system_at32m412_416.h"
#include "at32m412_416_clock.h"
#include "at32m412_416_mc.h"

#include "motor_control_drive_param.h"

#include <math.h>
#include "mc_delay.h"
#include "mc_type.h"
#include "mc_hwio_m412_lv_v1_0.h"
#include "mc_curr_fdbk.h"
#include "mc_comm_uart.h"
#include "mc_pid_controller.h"
#include "mc_math.h"
#if defined (HALL_SENSORS)
#include "mc_hall.h"
#else 
#include "mc_bldc_sensorless.h"
#endif
#include "user_interface_bldc.h"
#include "mc_bldc_globals.h"
#include "mc_bldc.h"
#include "mc_flash_data_table.h"
#include "motor_control_bldc.h"
#include "mc_bldc_kernal.h"
#include "mc_esc_state.h"

#if defined (SIX_STEP_CONTROL) && defined (HALL_SENSORS)
#define FIRMWARE_ID BLDC_HALL_SENSOR
#elif defined (SIX_STEP_CONTROL) && defined (SENSORLESS) && defined (BLDC_SENSORLESS_ADC)
#define FIRMWARE_ID BLDC_SENSOR_LESS_ADC
#elif defined (SIX_STEP_CONTROL) && defined (SENSORLESS) && defined (BLDC_SENSORLESS_COMP)
#define FIRMWARE_ID BLDC_SENSOR_LESS_COMP
#endif


#ifdef __cplusplus
}
#endif

#endif


