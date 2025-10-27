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
int32_t Is;
/** @addtogroup at32m412_lv_motor_ev
  * @{
  */

/** @addtogroup pmsm_foc_incremental_encoder pmsm_foc_incremental_encoder
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
#endif

      if (curr_offset_rdy & foc_rdy)
      {
        if(ctrl_mode == OPEN_LOOP_CTRL)
        {
          enc_rotor_angle_get(&encoder, &rotor_angle_encoder);

          foc_open_loop_ctrl(&volt_cmd, &openloop);

          elec_angle_val = openloop.theta;

          /* trigonometric functions transformation */
          trig_components = trig_functions(elec_angle_val);

          /* Clarke transformation */
          foc_clarke_trans(&current.Iabc, &current.Ialphabeta);

          /* Park transformation */
          foc_park_trans(&current, &trig_components);

          /* foc circle limitation */
          foc_circle_limitation(&volt_cmd);

          /* Inverse Park transformation */
          foc_inver_park_trans(&volt_cmd, &trig_components);

          /* svpwm function */
          svpwm_func(&volt_cmd, &pwm_duty);
        }
        else
        {
          if (encoder.align == PROCESSING)
          {
#if defined ABZ
            error_code |= error_code_mask & encoder_alignment_index(&volt_cmd, &encoder);
#elif defined AB
            encoder_alignment(&volt_cmd, &encoder);
#endif
            /* svpwm function */
            svpwm_func(&volt_cmd, &pwm_duty);
          }
          else if (encoder.align == SUCCEED || esc_state == ESC_STATE_I_TUNE)
          {
            elec_angle_val = enc_rotor_angle_get(&encoder, &rotor_angle_encoder);

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

#if defined IPM_MTPA_MTPV_CTRL && !defined TORQUE_CTRL_WITH_SPEED_LIMIT
            if (ctrl_mode == TORQUE_CTRL)
            {
              ipmsm_mtpa_control();
            }
#endif
            Iref.d = current.Iqdref.d;
            Iref.q = current.Iqdref.q;

            /* Torque control */
            if (ctrl_mode == POSITION_CTRL || ctrl_mode == SPEED_CTRL || ctrl_mode == TORQUE_CTRL || ctrl_mode == ID_MANUAL_TUNE || ctrl_mode == IQ_MANUAL_TUNE)
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
        ui_wave_param.user_define_a = (int16_t)encoder.count;
        ui_wave_param.user_define_b = (int16_t)(Is>>16);
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
  }
}

/**
  * @brief  exint interrupt handler for incremental encoder with index event
  * @param  none
  * @retval none
  */
#if defined ABZ
void EXINT_ENCODER_IDX_IRQ(void)
{
  if (exint_flag_get(ENCODER_Z_EXINT_LINE) != RESET)
  {
    exint_flag_clear(ENCODER_Z_EXINT_LINE);

    if (gpio_input_data_bit_read(ENCODER_Z_PORT, ENCODER_Z_GPIO_PIN) != RESET)
    {
      encoder_count_reset(&encoder, &rotor_speed_encoder);
    }
  }
}
#endif


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

    rotor_speed_encoder.val = enc_rotor_speed_get(&encoder, &rotor_speed_encoder);
    pos.val += encoder.delta;
    angle.val = (int32_t)(pos.val * PULSE_TO_ANGLE);
    rotor_speed_val = rotor_speed_encoder.val;

    if (esc_state == ESC_STATE_RUNNING)
    {
      if (ctrl_mode == SPEED_CTRL || ctrl_mode == POSITION_CTRL)
      {
        /* position control loop */
        if (ctrl_mode == POSITION_CTRL)
        {
          if (pos.count < POSITION_LOOP_COUNT)
            pos.count++;
          else
          {
            pos.count = 0;
            position_control_handler();
          }
        }
        else
          pos.count = 0;

        //speed_pid_param();   /* fine-tune speed pi parameters at low speed control if need */
        spd_err = speed_ramp.command - rotor_speed_val;
#if defined IPM_MTPA_MTPV_CTRL
        current.Tref = pid_controller(&pid_spd, spd_err);
        ipmsm_mtpa_control();
#else
        current.Iqdref.q = pid_controller(&pid_spd, spd_err);
#ifdef FIELD_WEAKENING
        current.Iqdref.d = pid_controller(&(field_weakening.pid_fw), field_weakening.fw_err_filt);
#endif
#endif

#ifdef CURRENT_DECOUPLE_CTRL
        /* current decoupling control */
        curr_decoupling_ctrl(&current_decoupling, current.Iqd);
#endif
#ifdef IPM_MTPA_MTPV_TABLE
        mtpa.mtpa_table.I_filt_val.d = lowpass_filtering(&mtpa.Id_LPF, mtpa.I_val->d);
        mtpa.mtpa_table.I_filt_val.q = lowpass_filtering(&mtpa.Iq_LPF, mtpa.I_val->q);
        Is = mtpa.mtpa_table.I_filt_val.d * mtpa.mtpa_table.I_filt_val.d + mtpa.mtpa_table.I_filt_val.q * mtpa.mtpa_table.I_filt_val.q;
        foc_iq_limitation(&current, &pid_spd);
#endif

      }
#if defined TORQUE_CTRL_WITH_SPEED_LIMIT || defined CURRENT_DECOUPLE_CTRL
      else if (ctrl_mode == TORQUE_CTRL)
      {
#if defined TORQUE_CTRL_WITH_SPEED_LIMIT
        spd_err = speed_ramp.command - rotor_speed_val;

#if defined IPM_MTPA_MTPV_CTRL
        current.Tref = pid_controller(&pid_spd_trq, spd_err);
        ipmsm_mtpa_control();
#else
        current.Iqdref.q = pid_controller(&pid_spd_trq, spd_err);
#ifdef FIELD_WEAKENING
        current.Iqdref.d = pid_controller(&(field_weakening.pid_fw), field_weakening.fw_err_filt);
#endif
#endif
#endif
#if defined CURRENT_DECOUPLE_CTRL
        /* current decoupling control */
        curr_decoupling_ctrl(&current_decoupling, current.Iqd);
#endif
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
  /* Encoder error protection */
  if (curr_offset_rdy & foc_rdy)
  {
    error_code |= error_code_mask & enc_error_check(&encoder, &speed_ramp, &current);
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
