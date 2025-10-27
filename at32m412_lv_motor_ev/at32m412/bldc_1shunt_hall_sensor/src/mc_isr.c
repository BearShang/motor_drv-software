/**
  **************************************************************************
  * @file     mc_isr.c
  * @brief    Relevant motor control interrupt functions
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

#include "mc_lib.h"

/** @addtogroup at32m412_lv_motor_ev
  * @{
  */

/** @addtogroup bldc_1shunt_hall_sensor bldc_1shunt_hall_sensor
  * @{
  */

/** @defgroup mc_isr
  * @brief Relevant motor control interrupt functions
  * @{
  */

/**
  * @brief  PWM timer interrupt handler for update event
  * @param  none
  * @retval none
  */
void ADVTMR_PWM_CYCLE_IRQ(void)
{
  static int8_t ui_count = 0;

  if (PWM_ADVANCE_TIMER->ists_bit.ovfif && PWM_ADVANCE_TIMER->iden_bit.ovfien)
  {
    /* clear update flag of pwm timer */
    tmr_flag_clear(PWM_ADVANCE_TIMER, TMR_OVF_FLAG);

    adc_ordinary_software_trigger_enable(ADC_NORMAL_CONVERTER, TRUE);

#ifdef MOTOR_PARAM_IDENTIFY
    if (motor_param_ident.state_flag == PROCESSING)
    {
      if (motor_param_ident.id_flag == SET)
      {
        param_identify(&motor_param_ident);
      }
      tmr_channel_value_set(ADC_TIMER, ADC_TIMER_SELECT_CHANNEL, motor_param_ident.duty); /* trigger adc current sensing */
    }
#endif

    if(bldc_rdy == SET)
    {
      if(closeloop_rdy != SET)
      {
        if(hall_learn.start_flag != SET)
        {
          bldc_open_loop_ctrl(&hall, &openloop, &rotor_speed);
        }
        else
        {
          bldc_hall_learning(&hall, &hall_learn);
        }
      }
      else
      {
        /* Current loop control */
        if(current_loop_ctrl == SET)
        {
#ifdef CURRENT_LP_FILTER
          volt_cmd = pid_controller(&pid_is, (current.Ibus.Iref - ibus_filterd));
#else
          volt_cmd = pid_controller(&pid_is, (current.Ibus.Iref - current.Ibus.Ireal_pu));
#endif
#if defined LOW_SPEED_VOLT_CTRL || defined WITHOUT_CURRENT_CTRL
          pid_spd_volt.integral = volt_cmd << pid_spd_volt.ki_shift;
        }
        else
        {
          /* update pid integral */
          pid_is.integral = volt_cmd << pid_is.ki_shift;
#endif
        }
      }

      /* Set PWM duty */
      pwm_comp_value = (abs(volt_cmd) * (PWM_PERIOD + 1)) >> 15;
      pwm_duty_update();

      /* Set ADC sample point */
      calc_adc_sample_point(&adc_sample, pwm_comp_value);
      set_adc_sample_point(&adc_sample);
    }
    /* UI sample */
#if defined USE_MOTOR_MONITOR
    if(++ui_count >= ui_wave_param.sample_cycle)
    {
      ui_wave_param.user_define_a = (int16_t)pwm_comp_value;
      ui_wave_param.user_define_b = (int16_t)rotor_speed.val;
      ui_save_monitor_data();
      ui_count = 0;
    }

#endif
  }
}

/**
  * @brief  PWM timer interrupt handler for break event
  * @param  none
  * @retval none
  */
void ADVTMR_PWM_BRK_IRQ(void)
{
  if (PWM_ADVANCE_TIMER->ists_bit.brkif && PWM_ADVANCE_TIMER->iden_bit.brkie)
  {
    tmr_flag_clear(PWM_ADVANCE_TIMER, TMR_BRK_FLAG);
    /* Over current protection */
    error_code |= error_code_mask & MC_OVER_CURRENT_ERROR;
  }
}

/**
  * @brief  hall timer interrupt handler for Hall edge input
  * @param  none
  * @retval none
  */
void HALL_CAPTURE_IRQ(void)
{
  hall_isr_handler();
}

/**
  * @brief  adc interrupt handler for end of preempted channel conversion
  * @param  none
  * @retval none
  */
void ADC_SHUNT_SAMP_READY_IRQ(void)
{
  if (adc_flag_get(ADC_INSTANT_CONVERTER, ADC_PCCE_FLAG) != RESET)
  {
    adc_flag_clear(ADC_INSTANT_CONVERTER, ADC_PCCE_FLAG);
#ifdef MOTOR_PARAM_IDENTIFY
    if (esc_state == ESC_STATE_WINDING_PARAM_ID)
    {
      current_read_1shunt_ID(&current);
    }
    else
#endif
      current_read_bldc(&current);
#ifdef CURRENT_LP_FILTER
      ibus_filterd = lowpass_filtering(&current_LPF, current.Ibus.Ireal_pu);
#endif
  }
}

/**
  * @brief  this function handles systick handler.
  * @param  none
  * @retval none
  */
void SysTick_Handler(void)
{
  static uint16_t led_blink_cnt = 0;
 
  if(ctrl_source == CTRL_SOURCE_EXTERNAL)
  {
    external_input_handler();
  }
 
  ESC_State_Task(esc_state_old);

  iMosTemperature = (int16_t)(((uint32_t)(adc_in_tab[ADC_MOS_TEMP_IDX] * TEMPER_A + TEMPER_B + TEMPER_C) * 100) >> 16); /* Celsius degrees = iMosTemperature/100 */
  ui_wave_param.iMosTemperature_meas = (int16_t)(iMosTemperature);
  ui_wave_param.iBusVoltage_meas = (int16_t)(calcValueByVref(adc_in_tab[ADC_BUS_VOLT_IDX]));
  ui_wave_param.speed_meas_filter_pu = (int16_t)(rotor_speed.filtered * RPM_TO_SPEED_PU >> 15);
  ui_wave_param.speed_reference_pu = (int16_t)(speed_ramp.command * RPM_TO_SPEED_PU >> 15);

  /* Over/under voltage protection */
  if (adc_in_tab[ADC_BUS_VOLT_IDX] < UNDERVOLTAGE_THRESHOLD_d)
  {
    error_code |= error_code_mask & MC_UNDER_VOLT_ERROR;
  }
  else if (adc_in_tab[ADC_BUS_VOLT_IDX] > OVERVOLTAGE_THRESHOLD_d)
  {
    error_code |= error_code_mask & MC_OVER_VOLT_ERROR;
  }

  /* MOS Temperature protection */
  if (adc_in_tab[ADC_MOS_TEMP_IDX] < TEMPERATURE_THRESHOLD_d)
  {
    error_code |= error_code_mask & MC_OVER_TEMP_ERROR;
  }

  /* Enter error state handler */
  if (error_code != MC_NO_ERROR)
  {
    esc_state = ESC_STATE_ERROR;
  }
  normal_state_led_blink(&led_blink_cnt,LED_G_PORT,LED_G_GPIO_PIN);
}

#if defined PWM_INPUT
flag_status is_first_capture = SET;
void PWM_DUTY_INPUT_IRQ(void)
{
  int32_t pulse_width;
  uint16_t capture_value, pwn_in_period, pin_state, duty_cycle;
  static uint16_t no_signal_counter;

  if (tmr_flag_get(PWM_DUTY_INPUT_TIMER, PWM_DUTY_INPUT_FLAG) != RESET)
  {
    /* clear flags of ch1 events */
    tmr_flag_clear(PWM_DUTY_INPUT_TIMER, PWM_DUTY_INPUT_FLAG);

    capture_value = tmr_channel_value_get(PWM_DUTY_INPUT_TIMER, PWM_DUTY_INPUT_SELECT_CHANNEL);
    pin_state = gpio_input_data_bit_read(PWM_DUTY_INPUT_PORT, PWM_DUTY_INPUT_GPIO_PIN);
    pulse_width = (int32_t) capture_value - pwm_in_pulse_rising_old;

    if (pulse_width < 0)
    {
      pulse_width += 0xFFFF;
    }

    if(is_first_capture)
    {
      is_first_capture = RESET;
      PWM_DUTY_INPUT_TIMER -> cval = 0;
    }
    else
    {
      if (pin_state != RESET)
      {
        pwn_in_period = (uint16_t) pulse_width;

        duty_cycle = (uint16_t)((((uint32_t) pwm_in_high_width << 15) - 1) / pwn_in_period);

        if (duty_cycle >= PWM_IN_START_DUTY)
        {
          speed_ramp.cmd_final = (duty_cycle * MAX_SPEED_RPM) >> 15;
          start_stop_btn_flag = SET;
        }
        else if (duty_cycle <= PWM_IN_STOP_DUTY)
        {
          speed_ramp.cmd_final = 0;
          start_stop_btn_flag = RESET;
        }

        pwm_in_pulse_rising_old = capture_value;
        no_signal_counter = 0;
      }
      else
      {
        pwm_in_high_width = (uint16_t) pulse_width;
      }
    }
  }
  if (PWM_DUTY_INPUT_TIMER->ists_bit.ovfif && PWM_DUTY_INPUT_TIMER->iden_bit.ovfien)
  {
    /* clear flags of overflow events */
    tmr_flag_clear(PWM_DUTY_INPUT_TIMER, TMR_OVF_FLAG);
    no_signal_counter++;
    if(no_signal_counter >= 3)
    {
      speed_ramp.cmd_final = 0;
      start_stop_btn_flag = RESET;
    }
  }
}
#endif

