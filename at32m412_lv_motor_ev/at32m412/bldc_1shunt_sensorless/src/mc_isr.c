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

int16_t *EmfContSampleEnd[2] = {&(lowspd_sample_end), &(highspd_sample_end)};
flag_status detect_zcp_flag = RESET;

/** @addtogroup at32m412_lv_motor_ev
  * @{
  */

/** @addtogroup bldc_1shunt_sensorless bldc_1shunt_sensorless
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
        bldc_open_loop_ctrl(&hall, &openloop, &rotor_speed);
      }
      else
      {
        if(current_loop_ctrl == SET)
        {
#if defined EMF_COMPENSATE
          ke_compen = (int16_t)(rotor_speed.filtered * KE_COMPENSATE >> 15);
          volt_cmd = pid_controller(&pid_is, (current.Ibus.Iref - current.Ibus.Ireal_pu)) + ke_compen;
#else

          if(const_current_ctrl != SET)
          {
#ifdef CURRENT_LP_FILTER
            volt_cmd = pid_controller(&pid_is, (current.Ibus.Iref - ibus_filterd));
#else
            volt_cmd = pid_controller(&pid_is, (current.Ibus.Iref - current.Ibus.Ireal_pu));
#endif
#if defined LOW_SPEED_VOLT_CTRL
            pid_spd_volt.integral = volt_cmd << pid_spd_volt.ki_shift;
#endif
          }
          else
          {
            volt_cmd = pid_controller(&pid_is, (current.Ibus.Istart - current.Ibus.Ireal_pu));
          }

#endif
        }

#if defined LOW_SPEED_VOLT_CTRL
        else
        {
          /* update pid integral */
          pid_is.integral = volt_cmd << pid_is.ki_shift;
        }

#endif
      }

      /* Set PWM duty */
      pwm_comp_value = (abs(volt_cmd) * (PWM_PERIOD + 1)) >> 15;
      pwm_duty_update();

      /* Set ADC sample point */
      calc_adc_sample_point(&adc_sample, pwm_comp_value);
#if defined (BLDC_SENSORLESS_COMP)
      adc_sample.ADC_TMRx->c4dt = adc_sample.current_sampling_point;
#else
      set_adc_sample_point(&adc_sample);
#endif
    }

    /* calculate motor speed */
    calc_motor_speed(&rotor_speed);

    if(detect_zcp_flag != SET && rotor_speed.speed_count > rotor_speed.val)
    {
      /* clear flags of ch1 events of comparator capture timer */
      tmr_flag_clear(COMP_OUT_CAPTURE_TIMER, COMP_OUT_FLAG);
      /* enable COMP cature timer */
      tmr_interrupt_enable(COMP_OUT_CAPTURE_TIMER, COMP_OUT_INT, TRUE);
    }

    /* UI sample */
#if defined USE_MOTOR_MONITOR

    if(++ui_count >= ui_wave_param.sample_cycle)
    {
      ui_wave_param.user_define_a = (int16_t)current.Ibus.Iref;
      ui_wave_param.user_define_b = (int16_t)current.Ibus.Ireal_pu;
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
  * @brief  Internal Comparator-Based BEMF Zero-Crossing Capture Interrupt.
  * @note   Triggered by MCU's internal comparator output.
  *         Hardware requirement: Comparator connected to timer capture input.
  * @param  none
  * @retval none
  */
void COMP_OUT_CAPTURE_IRQ(void)
{
  if (tmr_flag_get(COMP_OUT_CAPTURE_TIMER, COMP_OUT_FLAG)) /* Low Speed */
  {
    /* clear flags of ch1 events of comparator capture timer */
    tmr_flag_clear(COMP_OUT_CAPTURE_TIMER, COMP_OUT_FLAG);

    /* disable COMP cature timer */
    tmr_interrupt_enable(COMP_OUT_CAPTURE_TIMER, COMP_OUT_INT, FALSE);
    detect_zcp_flag = SET;

#if defined (BLDC_SENSORLESS_COMP)

    if(hall.state != hall.pre_state)
    {
      hall.pre_state = hall.state;
    }

    bldc_sensorless_change_phase(&hall, &adc_sample, &rotor_speed);
#endif
  }
}

/**
  * @brief  timer interrupt handler for commutation(BLDC sensor-less)
  * @param  none
  * @retval none
  */
void CHANGE_PHASE_IRQ(void)
{
  if (tmr_flag_get(CHANGE_PHASE_TIMER, TMR_OVF_FLAG))
  {
    /* clear flags of trigger/ch1 and overflow events of hall timer */
    tmr_flag_clear(CHANGE_PHASE_TIMER, TMR_OVF_FLAG);

    /* disable change phase timer */
    tmr_counter_enable(CHANGE_PHASE_TIMER, FALSE);

    if(start_state == START_STATE_FIRST_STEP)
    {
#if defined INIT_ANGLE_STARTUP
      bldc_output_config(hall.next_state);
      rotor_speed.speed_count = 0;
#endif
      start_state = START_STATE_SENSE_HALL;
    }
    else if(start_state == START_STATE_SENSE_HALL)
    {
      sense_hall_steps++;
    }

    if(closeloop_rdy != RESET)  //当闭环控制已准备好（进入稳定 ZCP 区间）
    {
      adc_sample.emf.emf_avoid_noise_times = EMF_AVOID_NOISE_TIMES;
      tmr_event_sw_trigger(PWM_ADVANCE_TIMER, TMR_HALL_SWTRIG); //向“相位提前/换相定时器”发软触发，通知它开始按照当前设定的延时去切换相位
      hall.state = hall.next_state;
      rotor_speed.val = (rotor_speed.speed_count >> 1) + rotor_speed.speed_count;
      cmp_init_struct.cmp_inverting = emf_detect_cmp_channel[hall.state];
      cmp_init_struct.cmp_polarity = emf_detect_cmp_polarity[hall.state];
      cmp_init(BEMF_COMP, &cmp_init_struct);
    }

    emf_avoid_noise_counter = 0;

    /* set flag to calculate motor speed in main function */
    calc_spd_rdy = SET;

    /* Reset Change phase flag */
    change_phase_flag = RESET;

    detect_zcp_flag = RESET;
  }
}

/**
  * @brief  adc interrupt handler for end of preempted channel conversion
  * @param  none
  * @retval none
  */
#ifdef BLDC_SENSORLESS_COMP
uint16_t emf_adc_val[3] = {0};

void ADC_SHUNT_SAMP_READY_IRQ(void)
{
  if (adc_flag_get(adc_sample.adc_x, ADC_PCCE_FLAG) != RESET)
  {
    adc_flag_clear(adc_sample.adc_x, ADC_PCCE_FLAG);

    if(esc_state == (ESC_STATE_FREE_RUN || ESC_STATE_BRAKING))
    {

      emf_adc_val[0] = adc_preempt_conversion_data_get(adc_sample.adc_x, ADC_PREEMPT_CHANNEL_1);
      emf_adc_val[1] = adc_preempt_conversion_data_get(adc_sample.adc_x, ADC_PREEMPT_CHANNEL_2);
      emf_adc_val[2] = adc_preempt_conversion_data_get(adc_sample.adc_x, ADC_PREEMPT_CHANNEL_3);

      if(emf_adc_val[0] < EMF_MIN_VALUE && emf_adc_val[1] < EMF_MIN_VALUE && emf_adc_val[2] < EMF_MIN_VALUE)
      {
        rotor_speed.speed_count = MAX_SPD_CNT;
        rotor_speed.val_temp = MAX_SPD_CNT;
        rotor_speed.filtered = 0;
        speed_LPF.output_temp = 0;
        reset_ma_buffer(interval_moving_average_fliter);
      }
    }
    else
    {
      current_read_bldc(&current);
#ifdef CURRENT_LP_FILTER
      ibus_filterd = lowpass_filtering(&current_LPF, current.Ibus.Ireal_pu);
#endif
    }
  }
}
#else
void ADC_SHUNT_SAMP_READY_IRQ(void)
{
  if (adc_flag_get(adc_sample.adc_x, ADC_PCCE_FLAG) != RESET)
  {
    adc_flag_clear(adc_sample.adc_x, ADC_PCCE_FLAG);

    if(change_phase_flag == RESET && closeloop_rdy == SET)
    {
      detect_zero_cross_point();
    }

#ifdef MOTOR_PARAM_IDENTIFY

    if (esc_state == ESC_STATE_WINDING_PARAM_ID)
    {
      current_read_1shunt_ID(&current);
    }
    else
#endif
      current_read_bldc(&current);
  }
}
#endif


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
  ui_wave_param.speed_meas_filter_pu = (int16_t)(i32_speed_filterd * RPM_TO_SPEED_PU >> 15);
  ui_wave_param.speed_reference_pu = (int16_t)(speed_ramp.command * RPM_TO_SPEED_PU >> 15);
  adc_sample.emf.emf_half_vdc_val = (int16_t)(ui_wave_param.iBusVoltage_meas * EMF_HALF_VDC_GAIN);
  zcp_highspd_fall = (adc_sample.emf.emf_half_vdc_val) + (adc_sample.emf.emf_high_spd_offset_falling);
  zcp_highspd_rise = (adc_sample.emf.emf_half_vdc_val) + (adc_sample.emf.emf_high_spd_offset_rising);

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

void ADVTMR_CH1_EMF_PULL_IRQ(void)
{
  if (tmr_flag_get(PWM_ADVANCE_TIMER, TMR_C1_FLAG))
  {
    /* clear update flag of pwm timer */
    tmr_flag_clear(PWM_ADVANCE_TIMER, TMR_C1_FLAG);

    /* PULL UP */
    if ((adc_sample.emf.emf_pull_up == SET) && (EMF_switch_sample_position == 0))
    {
      EMF_PULL_UP_PORT->clr = EMF_PULL_UP_GPIO_PIN;
    }
    else
    {
      EMF_PULL_UP_PORT->scr = EMF_PULL_UP_GPIO_PIN;
    }
  }
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

