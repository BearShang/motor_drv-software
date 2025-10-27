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

trig_components_type trig_components;
int16_t Iq_cmd;
int8_t Iq_cmd_count;
/** @addtogroup at32m412_lv_motor_ev
  * @{
  */

/** @addtogroup pmsm_foc_sensorless pmsm_foc_sensorless
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

  if (tmr_flag_get(PWM_ADVANCE_TIMER, TMR_OVF_FLAG) != RESET)
  {
    tmr_flag_clear(PWM_ADVANCE_TIMER, TMR_OVF_FLAG);

#ifdef ONE_SHUNT
    if ((PWM_ADVANCE_TIMER->ctrl1_bit.cnt_dir & TMR_COUNT_DOWN) == RESET)  /* underflow interrupt */
    {
#endif
      monitoring_signal_adc_trigger(); /* adc orydinary software trigger */

#ifdef MOTOR_PARAM_IDENTIFY
      if (motor_param_ident.state_flag == PROCESSING)
      {
        if (motor_param_ident.id_flag == SET)
        {
          param_identify(&motor_param_ident);
        }
#ifdef ONE_SHUNT
        if (esc_state == ESC_STATE_WINDING_PARAM_ID)
        {
          tmr_channel_value_set(ADC_TIMER, ADC_TIMER_SELECT_CHANNEL, motor_param_ident.duty); /* trigger adc current sensing */
        }
#endif
      }
      else
#endif
      {
#ifdef WIND_SENSE
        wind_sense();
#elif defined VOLT_SENSE
        volt_sense();
#else
        motor_volt_calc(&motor_voltage);
        state_observer.elec_angle = rotor_angle_sensorless(&state_observer, &motor_voltage);
        /* low pass filter for speed by observer */
        state_observer.motor_speed.filtered = lowpass_filtering(&obs_speed_LPF, state_observer.motor_speed.val);
#endif
      }

      if (curr_offset_rdy & foc_rdy)
      {
#if defined TWO_ADC_CONVERTERS && defined THREE_SHUNT
        /* adc channels selection */
        select_two_adc_channels();
#endif
        if(ctrl_mode == OPEN_LOOP_CTRL)
        {
          foc_open_loop_ctrl(&volt_cmd, &openloop);

          elec_angle_val = openloop.theta;

          /* trigonometric functions transformation */
          trig_components = trig_functions(elec_angle_val);

          /* Clarke transformation */
          foc_clarke_trans(&current.Iabc, &current.Ialphabeta);

          /* Park transformation */
          foc_park_trans(&current, &trig_components);

          if (volt_cmd.Vqd.q >= 0)
          {
            startup.dir = CW;
          }
          else
          {
            startup.dir = CCW;
          }

          /* foc circle limitation */
          foc_circle_limitation(&volt_cmd);

          /* Inverse Park transformation */
          foc_inver_park_trans(&volt_cmd, &trig_components);

          /* svpwm function */
          svpwm_func(&volt_cmd, &pwm_duty);
        }
        else
        {
          if (ctrl_mode == SPEED_CTRL || ctrl_mode == TORQUE_CTRL || ctrl_mode == VF_CTRL)
          {
            if (startup.closeloop_rdy != SET)
            {
              if (ctrl_mode == SPEED_CTRL && speed_ramp.cmd_final >= 0)
              {
                startup.dir = CW;
              }
              else if (ctrl_mode == TORQUE_CTRL && current.Iqdref.q >= 0)
              {
                startup.dir = CW;
              }
              else
              {
                startup.dir = CCW;
              }

#if defined OPENLOOP_STARTUP    /* method 1: open loop start-up */
              startup.closeloop_rdy = startup_openloop(&startup);
              //startup.closeloop_rdy = startup_align_openloop(&startup);

#elif defined ALIGN_AND_GO_STARTUP    /* method 2:  fixed alpha axis and force to beta axis */
              startup.closeloop_rdy = startup_alpha_axis(&startup);

#elif defined INIT_ANGLE_STARTUP    /* method 3: Initial angle detection */
              startup.closeloop_rdy = startup_openloop(&startup);
              //startup.closeloop_rdy = startup_angle_init(&startup);
#endif
            }

            if (startup.closeloop_rdy_old != RESET)
            {
              elec_angle_val = state_observer.elec_angle;
            }
            else if (startup.closeloop_rdy != RESET)
            {
              elec_angle_val = state_observer.elec_angle;
              startup.closeloop_rdy_old = startup.closeloop_rdy;
#if defined CONST_CURRENT_START
              if (ctrl_mode == SPEED_CTRL)
              {
                if (startup.dir == CW)
                {
                  current.Iqdref.q = (startup.start_current >> 1);
                }
                else
                {
                  current.Iqdref.q = -(startup.start_current >> 1);
                }

                pid_set_integral(&pid_spd, (int32_t)(current.Iqdref.q << pid_spd.ki_shift));
              }
#elif defined CONST_VOLTAGE_START
              if (ctrl_mode == SPEED_CTRL)
              {
                pid_set_integral(&pid_spd, (int32_t)(current.Iqd.q << pid_spd.ki_shift));
              }
              else if (ctrl_mode == TORQUE_CTRL)
              {
                pid_set_integral(&pid_id, (int32_t)(volt_cmd.Vqd.d << pid_id.ki_shift));
                pid_set_integral(&pid_iq, (int32_t)(volt_cmd.Vqd.q << pid_iq.ki_shift));
              }
#endif
            }
            else
            {
              elec_angle_val = startup.elec_angle;
            }
          }

          /* trigonometric functions transformation */
          trig_components = trig_functions(elec_angle_val);

          /* Clarke transformation */
          foc_clarke_trans(&current.Iabc, &current.Ialphabeta);

          /* Park transformation */
          foc_park_trans(&current, &trig_components);

#if CURRENT_LP_FILTER
          /* low pass filter for q/d-axis current */
          current.Iqd_LPF.d = lowpass_filtering(&d_current_LPF, current.Iqd.d);
          current.Iqd_LPF.q = lowpass_filtering(&q_current_LPF, current.Iqd.q);
          Ival.d = current.Iqd_LPF.d;
          Ival.q = current.Iqd_LPF.q;
#else
          Ival.d = current.Iqd.d;
          Ival.q = current.Iqd.q;
#endif

          if (startup.closeloop_rdy == SET || ctrl_mode == ID_MANUAL_TUNE || ctrl_mode == IQ_MANUAL_TUNE)
          {
            Iref.d = current.Iqdref.d;
            Iref.q = current.Iqdref.q;
          }

          /* Torque control */
#if defined CONST_CURRENT_START
          if ((ctrl_mode == SPEED_CTRL || ctrl_mode == TORQUE_CTRL || ctrl_mode == ID_MANUAL_TUNE || ctrl_mode == IQ_MANUAL_TUNE))
#elif defined CONST_VOLTAGE_START
          if (esc_state != ESC_STATE_STARTING && (ctrl_mode == SPEED_CTRL || ctrl_mode == TORQUE_CTRL || ctrl_mode == ID_MANUAL_TUNE || ctrl_mode == IQ_MANUAL_TUNE))
#endif
          {
#ifdef CURRENT_DECOUPLE_CTRL
            voltage_comp();
#else
            volt_cmd.Vqd.d = pid_controller(&pid_id, (Iref.d - Ival.d));

            /* foc Vq limitation */
            foc_vq_limitation(&volt_cmd, &pid_iq);

            volt_cmd.Vqd.q = pid_controller(&pid_iq, (Iref.q - Ival.q));
#endif
#if defined FIELD_WEAKENING
            field_weakening.fw_err_filt = ma_filter(pid_iq.upper_limit_output - abs(pid_iq.out_start), field_weakening.fw_err_filt, FW_MA_NBR_LOG);
#endif
          }
          else if (ctrl_mode == VF_CTRL)
          {
            /* foc circle limitation */
            foc_circle_limitation(&volt_cmd);
          }

          /* Inverse Park transformation */
          foc_inver_park_trans(&volt_cmd, &trig_components);

          /* svpwm function */
          svpwm_func(&volt_cmd, &pwm_duty);
        }
        /* pwm output */
#ifndef ONE_SHUNT
        pwm_duty_update(&pwm_duty);
#else
        asym_pwm_duty_update(&pwm_duty);
#endif
      }
#ifdef BRAKING_RESISTOR
      if (adc_in_tab[ADC_BUS_VOLT_IDX] > HYSTERESIS_HIGH_VOLTAGE_d)
      {
        tmr_channel_enable(BRAKE_PWM_TIMER, BRAKE_PWM_TIMER_CH, TRUE);
      }
      else if (adc_in_tab[ADC_BUS_VOLT_IDX] < HYSTERESIS_LOW_VOLTAGE_d)
      {
        tmr_channel_enable(BRAKE_PWM_TIMER, BRAKE_PWM_TIMER_CH, FALSE);
      }
#endif
      /* UI sample */
#if defined USE_MOTOR_MONITOR
      if(++ui_count >= ui_wave_param.sample_cycle)
      {
        ui_wave_param.user_define_a = (int16_t)motor_emf.max_bemf_val_filt;
        ui_wave_param.user_define_b = (int16_t)startup.elec_angle;
        ui_save_monitor_data();
        ui_count = 0;
      }
#endif

#ifdef ONE_SHUNT
    }

#endif
  }
}

/**
  * @brief  timer interrupt handler for break event
  * @param  none
  * @retval none
  */
void ADVTMR_PWM_BRK_IRQ(void)
{
  if (tmr_flag_get(PWM_ADVANCE_TIMER, TMR_BRK_FLAG) != RESET)
  {
    tmr_flag_clear(PWM_ADVANCE_TIMER, TMR_BRK_FLAG);

    /* Over current protection */
    error_code |= error_code_mask & MC_OVER_CURRENT_ERROR;
  }
}

/**
  * @brief  adc interrupt handler for end of preempted channel conversion
  * @param  none
  * @retval none
  */
void ADC_SHUNT_SAMP_READY_IRQ(void)
{
#ifndef MOS_RDS_SHUNT
  if(adc_interrupt_flag_get(ADC_CONVERTER, ADC_VMOR_FLAG) != RESET)
  {
    adc_flag_clear(ADC_CONVERTER, ADC_VMOR_FLAG);

    /* Over current protection */
    error_code |= error_code_mask & MC_OVER_CURRENT_ERROR;
  }
#if defined TWO_ADC_CONVERTERS
  if(adc_interrupt_flag_get(ADC_SIMULTANE_CONVERTER, ADC_VMOR_FLAG) != RESET)
  {
    adc_flag_clear(ADC_SIMULTANE_CONVERTER, ADC_VMOR_FLAG);

    /* Over current protection */
    error_code |= error_code_mask & MC_OVER_CURRENT_ERROR;
  }
#endif
#endif
  if (adc_flag_get(ADC_CONVERTER, ADC_PCCE_FLAG) != RESET)
  {
    adc_flag_clear(ADC_CONVERTER, ADC_PCCE_FLAG);

    if (esc_state != ESC_STATE_ANGLE_INIT)
    {
#if defined TWO_ADC_CONVERTERS
#if defined THREE_SHUNT
      current_read_foc_3shunt_2adc(&current);
#elif defined TWO_SHUNT
      current_read_foc_2shunt_2adc(&current);
#endif

#else
#if defined THREE_SHUNT
      current_read_foc_3shunt(&current, &pwm_duty);
#elif defined TWO_SHUNT
      current_read_foc_2shunt(&current);
#elif defined ONE_SHUNT
#ifdef MOTOR_PARAM_IDENTIFY
      if (esc_state == ESC_STATE_WINDING_PARAM_ID)
      {
        current_read_1shunt_ID(&current);
      }
      else
#endif
        current_read_foc_1shunt(&current, volt_cmd.sector);
#endif
#endif

#if AC_CURRENT_LP_FILTER
      current.Iabc.a = lowpass_filtering(&ia_current_LPF, current.Iabc.a);
      current.Iabc.b = lowpass_filtering(&ib_current_LPF, current.Iabc.b);
      current.Iabc.c = -(current.Iabc.a + current.Iabc.b);
#endif
    }
    else
    {
#if defined THREE_SHUNT
      current_angle_init_3shunt(&angle_detector, &current);
#elif defined TWO_SHUNT || defined ONE_SHUNT
      current_angle_init_2_1shunt(&angle_detector, &current);
#endif
    }
  }
}

/**
  * @brief  timer interrupt handler for speed control loop
  * @param  none
  * @retval none
  */
void SPEED_LOOP_TIMER_IRQ(void)
{
  if (tmr_flag_get(SPEED_LOOP_TIMER, TMR_OVF_FLAG) != RESET)
  {
    /* clear flags of overflow events */
    tmr_flag_clear(SPEED_LOOP_TIMER, TMR_OVF_FLAG);

    if (esc_state == ESC_STATE_RUNNING)
    {
      if (ctrl_mode == SPEED_CTRL)
      {
        spd_err = speed_ramp.command - rotor_speed_val;
        current.Iqdref.q = pid_controller(&pid_spd, spd_err);

#ifdef FIELD_WEAKENING
        current.Iqdref.d = pid_controller(&(field_weakening.pid_fw), field_weakening.fw_err_filt);
#endif

#ifdef CURRENT_DECOUPLE_CTRL
        /* current decoupling control */
        curr_decoupling_ctrl(&current_decoupling, current.Iqd);
#endif
      }

#ifdef CURRENT_DECOUPLE_CTRL
      else if(ctrl_mode == TORQUE_CTRL)
      {
        /* current decoupling control */
        curr_decoupling_ctrl(&current_decoupling, current.Iqd);
      }
#endif
    }
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

  rotor_speed_val = state_observer.motor_speed.filtered;

  rotor_speed_val_filt = moving_average_shift(speed_ma_fliter, rotor_speed_val);

  if(ctrl_source == CTRL_SOURCE_EXTERNAL)
  {
    external_input_handler();
  }

  ESC_State_Task(esc_state_old);

  iMosTemperature = (int16_t)(((uint32_t)(adc_in_tab[ADC_MOS_TEMP_IDX] * TEMPER_A + TEMPER_B + TEMPER_C)*100)>>16); /* Celsius degrees = iMosTemperature/100 */
  ui_wave_param.iMosTemperature_meas = (int16_t)(iMosTemperature);
  ui_wave_param.iBusVoltage_meas = (int16_t)(adc_in_tab[ADC_BUS_VOLT_IDX]*vref_cal_ratio>>14);
  ui_wave_param.speed_meas_filter_pu = (int16_t)((rotor_speed_val_filt * RPM_TO_SPEED_PU)>>15);
  ui_wave_param.speed_reference_pu = (int16_t)((speed_ramp.command * RPM_TO_SPEED_PU)>>15);
  ui_wave_param.position_meas_pu = (int16_t)((int32_t)(angle.val * DEGREE_TO_POS_PU)>>15);
  ui_wave_param.position_reference_pu = (int16_t)((int32_t)(angle.command * DEGREE_TO_POS_PU)>>15);

  /* Over/under voltage protection */
  if (ui_wave_param.iBusVoltage_meas < UNDERVOLTAGE_THRESHOLD_d)
  {
    error_code |= error_code_mask & MC_UNDER_VOLT_ERROR;
  }
  else if (ui_wave_param.iBusVoltage_meas > OVERVOLTAGE_THRESHOLD_d)
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

  /* step Iq current test */
#if 0

  if(ctrl_mode == TORQUE_CTRL)
  {
    if (Iq_cmd_count >= 0)
    {
      current.Iqdref.q = Iq_cmd;
    }
    else
    {
      current.Iqdref.q = -Iq_cmd;
    }

    Iq_cmd_count++;
  }

#endif
  normal_state_led_blink(&led_blink_cnt,STATUS1_LED_PORT,STATUS1_LED_GPIO_PIN);
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

        if(ctrl_mode == SPEED_CTRL)
        {
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

