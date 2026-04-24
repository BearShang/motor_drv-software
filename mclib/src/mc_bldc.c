/**
  **************************************************************************
  * @file     mc_bldc.c
  * @brief    BLDC-related functions
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
extern volatile uint32_t system_time_ms;

/** @addtogroup Motor_Control_Library
  * @{
  */

/** @defgroup mc_bldc
  * @brief BLDC-related functions
  * @{
  */

/**
* @brief  configuration of pwm timer outputs
* @param  hall_states : hall state
* @retval none
*/
void bldc_output_config(uint8_t hall_states)
{
  uint16_t temp;

  temp = PWM_ADVANCE_TIMER->cm1;
  temp &= (~TMR_PWM_MODE_CM1_MASK);
  temp |= tmr_pwm_channel_mode[hall_states][0];
  PWM_ADVANCE_TIMER->cm1 = temp;

  temp = PWM_ADVANCE_TIMER->cm2;
  temp &= (~TMR_PWM_MODE_CM2_MASK);
  temp |= tmr_pwm_channel_mode[hall_states][1];
  PWM_ADVANCE_TIMER->cm2 = temp;

  temp = PWM_ADVANCE_TIMER->cctrl;
  temp &= (~TMR_PWM_OUT_MODE_MASK);
  temp |= tmr_pwm_output_mode[hall_states];
  PWM_ADVANCE_TIMER->cctrl = temp;
}

/**
  * @brief  start bldc setting
  * @param  none
  * @retval none
  */
void start_bldc(void)
{
#if defined HALL_SENSORS
  error_code |= error_code_mask & read_hall_state(&hall);

  /* clear interrupt flags of hall timer */
  tmr_flag_clear(HALL_CAPTURE_TIMER, TMR_OVF_FLAG | TMR_TRIGGER_FLAG | TMR_HALL_FLAG);

  /* set dir: CW or CCW */
  if(ctrl_source == CTRL_SOURCE_EXTERNAL)
  {
#if !defined AT32M412xx
    rotor_speed.dir = gpio_input_data_bit_read(MODE1_BUTTON_PORT, MODE1_BUTTON_PIN);
#endif
  }
  else if(ctrl_source == CTRL_SOURCE_SOFTWARE)
  {
    if(ctrl_mode == SPEED_CTRL)
    {
      if(speed_ramp.cmd_final > 0)
      {
        rotor_speed.dir = CW;
      }
      else
      {
        rotor_speed.dir = CCW;
      }
    }
    else if(ctrl_mode == TORQUE_CTRL)
    {
      if(current.Ibus.Iref > 0)
      {
        rotor_speed.dir = CW;
      }
      else
      {
        rotor_speed.dir = CCW;
      }
    }
  }

  /* set first hall state output */
  bldc_output_config(output_hall_state[rotor_speed.dir][hall.state]);

  if(rotor_speed.dir == CW)
  {
    motor_cw_init();
  }
  else
  {
    motor_ccw_init();
  }

  /* update pwm output mode from shadow buffer of timer cctrl reg. */
  tmr_event_sw_trigger(PWM_ADVANCE_TIMER, TMR_HALL_SWTRIG);

  /* set hall pre_state */
  hall.pre_state = hall.state;

  /* find next hall state */
  hall.next_state = next_hall_state[rotor_speed.dir][hall.state];

  /* set pwm output mode in shadow buffer of timer cctrl reg. */
  bldc_output_config(output_hall_state[rotor_speed.dir][hall.next_state]);

  /* reset counter value */
  PWM_ADVANCE_TIMER->cval = 0;

  /* enable overflow and trigger interrup of hall timer */
  tmr_interrupt_enable(HALL_CAPTURE_TIMER, TMR_C4_INT | TMR_TRIGGER_INT, TRUE);

  /* enable hall timer */
  tmr_counter_enable(HALL_CAPTURE_TIMER, TRUE);

  /* enable pwm timer output */
  tmr_output_enable(PWM_ADVANCE_TIMER, TRUE);

#elif defined SENSORLESS
  /* reset counter value */
  PWM_ADVANCE_TIMER->cval = 0;
  ADC_TIMER->cval = 0;
  blank_trigger.TMRx->cval = 0;
  blank.TMRx->cval = 0;

  /* set pwm output mode in shadow buffer of timer cctrl reg. */
  bldc_output_config(output_hall_state[rotor_speed.dir][hall.state]);

  /* update pwm output mode from shadow buffer of timer cctrl reg. */
  tmr_event_sw_trigger(PWM_ADVANCE_TIMER, TMR_HALL_SWTRIG);

  /* enable pwm timer output */
  tmr_output_enable(PWM_ADVANCE_TIMER, TRUE);

  /* enable blank trigger timer output */
  tmr_output_enable(blank_trigger.TMRx, TRUE);
  tmr_output_enable(blank.TMRx, TRUE);

  /* enable adc timer */
  ADC_TIMER->ctrl1_bit.tmren = TRUE;
  /* enable pwm timer */
  PWM_ADVANCE_TIMER->ctrl1_bit.tmren = TRUE;
  /* enable blank trigger timer */
  blank_trigger.TMRx->ctrl1_bit.tmren = TRUE;
  blank.TMRx->ctrl1_bit.tmren = TRUE;
  COMP_OUT_CAPTURE_TIMER->ctrl1_bit.tmren = TRUE;
#endif
}

/**
  * @brief  Rotor alignment (BLDC)
  * @param  none
  * @retval none
  */
void align_bldc(void)
{
  /* set pwm output mode in shadow buffer of timer cctrl reg. */
  bldc_output_config(hall.state);
  /* update pwm output mode from shadow buffer of timer cctrl reg. */
  tmr_event_sw_trigger(PWM_ADVANCE_TIMER, TMR_HALL_SWTRIG);
  /* set align volt */
  volt_cmd = (int16_t)((uint32_t)(calcValueByVref(LOCK_VOLT_CMD) * vdc_ratio) >> VBUS_GAIN_LOG);
  pwm_comp_value = (volt_cmd * (PWM_PERIOD + 1)) >> 15;

  PWM_ADVANCE_TIMER->c1dt = pwm_comp_value;
  PWM_ADVANCE_TIMER->c2dt = pwm_comp_value;
  PWM_ADVANCE_TIMER->c3dt = pwm_comp_value;

  /* enable pwm timer */
  tmr_counter_enable(PWM_ADVANCE_TIMER, TRUE);

  /* enable pwm timer output */
  tmr_output_enable(PWM_ADVANCE_TIMER, TRUE);
}

/**
  * @brief  Open-loop control function (BLDC)
  * @param  hall_handler : Hall sensor related structure variables
  * @param  openloop_handler : Open-loop related structure variables
  * @param  rotor_speed : Speed-related structure variables
  * @retval none
  */
void bldc_open_loop_ctrl(hall_sensor_type *hall_handler, olc_type *openloop_handler, speed_type *rotor_speed)
{
  volt_cmd = openloop_handler->volt_ref * current.volt_sign;

  if(openloop_handler->olc_count > openloop_handler->period_ref)
  {
    /* Set next state */
    hall_handler->next_state = next_hall_state[rotor_speed->dir][hall_handler->state];

    /* Change mode **/
    bldc_output_config(output_hall_state[rotor_speed->dir][hall_handler->next_state]);

    /* Change phase trigger **/
    tmr_event_sw_trigger(PWM_ADVANCE_TIMER, TMR_HALL_SWTRIG);

    /* Renew hall state */
    hall_handler->state = hall_handler->next_state;

    /* Speed estimation */
    rotor_speed->interval_filter.long_word = openloop_handler->period_ref * PWM_PERIOD * (1 - 2 * rotor_speed->dir);

    /* set flag to calculate motor speed in main function */
    calc_spd_rdy = SET;

    openloop_handler->olc_count = 0;
    rotor_speed->speed_count = 0;
  }
  else
  {
    openloop_handler->olc_count++;
  }
}

/**
  * @brief  bldc hall auto learning function (BLDC)
  * @param  hall_handler : Hall sensor related structure variables
  * @param  hall_learn : Hall Sensor Auto-Learning related structure variables
  * @retval none
  */
void bldc_hall_learning(hall_sensor_type *hall_handler, hall_learn_type *hall_learn)
{
  if(hall_learn->count > hall_learn->learn_period)
  {
    /* Set next state */
    hall_handler->next_state = next_hall_state[hall_learn->dir][hall_handler->next_state];

    /* Change mode **/
    bldc_output_config(output_hall_state[hall_learn->dir][hall_handler->next_state]);

    /* update pwm output mode from shadow buffer of timer cctrl reg. */
    tmr_event_sw_trigger(PWM_ADVANCE_TIMER, TMR_HALL_SWTRIG);

    hall_learn->count = 0;
  }
  else
  {
    hall_learn->count++;
  }
}

/**
  * @brief  Configures PWM braking for motor control
  * @param  brake_force_duty : Braking force duty cycle (0-100%)
  * @note   0% = no braking, 100% = full braking force
  * @retval none
  */
void brake_config(uint16_t brake_force_duty)
{
  uint16_t temp, brake_pwm_cval;
  gpio_init_type gpio_init_struct = {0};

  /* pwm timer output disable */
  tmr_output_enable(PWM_ADVANCE_TIMER, FALSE);
  /* disable pwm timer */
  tmr_counter_enable(PWM_ADVANCE_TIMER, FALSE);

  /* timer pwm output pin Configuration */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;

  /* High-side, Phase A,B,C Config */
  gpio_init_struct.gpio_pins = PWM_PHASE_A_HI_GPIO_PIN;
  gpio_init(PWM_PHASE_A_HI_PORT, &gpio_init_struct);
#if !defined AT32F403Axx && !defined AT32F407xx && !defined AT32F413xx && !defined AT32F415xx
  gpio_pin_mux_config(PWM_PHASE_A_HI_PORT, PWM_PHASE_A_HI_PIN_SOURCE, PWM_PHASE_A_HI_IOMUX);
#endif

  gpio_init_struct.gpio_pins = PWM_PHASE_B_HI_GPIO_PIN;
  gpio_init(PWM_PHASE_B_HI_PORT, &gpio_init_struct);
#if !defined AT32F403Axx && !defined AT32F407xx && !defined AT32F413xx && !defined AT32F415xx
  gpio_pin_mux_config(PWM_PHASE_B_HI_PORT, PWM_PHASE_B_HI_PIN_SOURCE, PWM_PHASE_B_HI_IOMUX);
#endif

  gpio_init_struct.gpio_pins = PWM_PHASE_C_HI_GPIO_PIN;
  gpio_init(PWM_PHASE_C_HI_PORT, &gpio_init_struct);
#if !defined AT32F403Axx && !defined AT32F407xx && !defined AT32F413xx && !defined AT32F415xx
  gpio_pin_mux_config(PWM_PHASE_C_HI_PORT, PWM_PHASE_C_HI_PIN_SOURCE, PWM_PHASE_C_HI_IOMUX);
#endif

  /* close AH/BH/CH */
  gpio_bits_write(PWM_PHASE_A_HI_PORT, PWM_PHASE_A_HI_GPIO_PIN, FALSE);
  gpio_bits_write(PWM_PHASE_B_HI_PORT, PWM_PHASE_B_HI_GPIO_PIN, FALSE);
  gpio_bits_write(PWM_PHASE_C_HI_PORT, PWM_PHASE_C_HI_GPIO_PIN, FALSE);

  temp = PWM_ADVANCE_TIMER->cm1;
  temp &= (~TMR_PWM_MODE_CM1_MASK);
  temp |= BRAKE_PWM_MODE_CM1;
  PWM_ADVANCE_TIMER->cm1 = temp;

  temp = PWM_ADVANCE_TIMER->cm2;
  temp &= (~TMR_PWM_MODE_CM2_MASK);
  temp |= BRAKE_PWM_MODE_CM2;
  PWM_ADVANCE_TIMER->cm2 = temp;

  temp = PWM_ADVANCE_TIMER->cctrl;
  temp &= (~TMR_PWM_OUT_MODE_MASK);
  temp |= BRAKE_PWM_OUT_CCTRL;
  PWM_ADVANCE_TIMER->cctrl = temp;

  /* calculate brake force */
#if defined GATE_DRIVER_LOW_SIDE_INVERT
  brake_pwm_cval = (100 - brake_force_duty) * PWM_PERIOD_DIV_100;
#else
  brake_pwm_cval = brake_force_duty * PWM_PERIOD_DIV_100;
#endif

  /* set brake duty */
  tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_1, brake_pwm_cval);
  tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_2, brake_pwm_cval);
  tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_3, brake_pwm_cval);
  tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_4, brake_pwm_cval);

  /* update pwm output mode from shadow buffer of timer cctrl reg. */
  tmr_event_sw_trigger(PWM_ADVANCE_TIMER, TMR_HALL_SWTRIG);

  /* reset counter value */
  PWM_ADVANCE_TIMER->cval = 0;

  /* pwm timer output enable */
  tmr_output_enable(PWM_ADVANCE_TIMER, TRUE);

  /* enable pwm timer */
  tmr_counter_enable(PWM_ADVANCE_TIMER, TRUE);
}

/**
  * @brief  Motor beep function - generates audible tone by switching motor coils
  * @param  frequency : Tone frequency in Hz (20~20000 Hz)
  * @param  duration  : Duration in milliseconds
  * @param  volume    : Volume level (0-100), controls PWM duty cycle
  * @retval none
  * @note   Uses two-phase switching to avoid MOS overheating
  */
void motor_beep(uint16_t frequency, uint32_t duration, uint8_t volume)
{
  uint32_t toggle_period_us;
  uint32_t elapsed_us = 0;
  uint32_t total_duration_us = duration * 1000;
  uint16_t pwm_duty;
  uint8_t phase_step = 0;
  
  /* Save current PWM mode registers, error state, and BRK configuration */
  uint32_t saved_cm1 = PWM_ADVANCE_TIMER->cm1;
  uint32_t saved_cm2 = PWM_ADVANCE_TIMER->cm2;
  uint32_t saved_cctrl = PWM_ADVANCE_TIMER->cctrl;
  uint32_t saved_brk = PWM_ADVANCE_TIMER->brk;
  err_code_type saved_error_code = error_code;

  /* Temporarily lock OCP threshold to a fixed value during beeping to ensure 
     consistent volume regardless of the global OCP_CURRENT setting.
     Fixed at 10A equivalent DAC value for beeping. */
  #define BEEP_FIXED_OCP_DAC_VAL  ((uint16_t)(63.0 * (10.0 * R_SHUNT * OP_GAIN + CURR_OFFSET_VOLT) / ADC_REFERENCE_VOLT))
  dac_1_data_set(BEEP_FIXED_OCP_DAC_VAL);

  /* Keep hardware BRK function enabled to protect MOSFETs and limit peak current.
     This prevents the motor from rotating due to excessive current. */
  PWM_ADVANCE_TIMER->brk_bit.brken = TRUE;

  /* Temporarily disable break interrupt during beeping to avoid entering error state.
     The hardware will still "chop" the pulse if OCP is reached, but software won't lock out. */
  tmr_interrupt_enable(PWM_ADVANCE_TIMER, TMR_BRK_INT, FALSE);

  /* Limit frequency to audible range */
  if(frequency < 20) frequency = 20;
  if(frequency > 20000) frequency = 20000;

  /* Limit volume - for static motor beep, even low duty can cause high current */
  if(volume > 100) volume = 100; 

  /* Calculate half period in us (toggle period) */
  toggle_period_us = 500000 / frequency;
  if(toggle_period_us < 25) toggle_period_us = 25; /* Max frequency 20kHz */

  /* Calculate PWM duty based on volume (percentage of PWM_PERIOD).
     Using / 1000 or / 2000 to balance sound and current. 
     Hardware BRK (OCP) will act as the final current limiter. */
  pwm_duty = (uint16_t)((uint32_t)volume * PWM_PERIOD / 1000); 
  if(pwm_duty < 5) pwm_duty = 5; /* Minimum duty to ensure sound */

  /* Disable normal PWM output only, keep timer running */
  tmr_output_enable(PWM_ADVANCE_TIMER, FALSE);

  /* Generate tone by toggling between phases */
  while(elapsed_us < total_duration_us)
  {
    /* CRITICAL: Clear break flag at every step. 
       If a previous pulse was "chopped" by hardware OCP, the flag must be cleared
       to allow the next pulse to be output. */
    if(tmr_flag_get(PWM_ADVANCE_TIMER, TMR_BRK_FLAG))
    {
      tmr_flag_clear(PWM_ADVANCE_TIMER, TMR_BRK_FLAG);
    }

    /* Alternate between four phase combinations to reduce MOS heating */
    switch(phase_step)
    {
      case 0:
        /* AH-BL pattern */
        PWM_ADVANCE_TIMER->cm1 = AH_BL_PWM_MODE_CM1;
        PWM_ADVANCE_TIMER->cm2 = AH_BL_PWM_MODE_CM2;
        PWM_ADVANCE_TIMER->cctrl = AH_BL_PWM_OUT_CCTRL;
        break;

      case 1:
        /* BH-AL pattern */
        PWM_ADVANCE_TIMER->cm1 = BH_AL_PWM_MODE_CM1;
        PWM_ADVANCE_TIMER->cm2 = BH_AL_PWM_MODE_CM2;
        PWM_ADVANCE_TIMER->cctrl = BH_AL_PWM_OUT_CCTRL;
        break;

      case 2:
        /* AH-CL pattern */
        PWM_ADVANCE_TIMER->cm1 = AH_CL_PWM_MODE_CM1;
        PWM_ADVANCE_TIMER->cm2 = AH_CL_PWM_MODE_CM2;
        PWM_ADVANCE_TIMER->cctrl = AH_CL_PWM_OUT_CCTRL;
        break;

      case 3:
        /* CH-AL pattern */
        PWM_ADVANCE_TIMER->cm1 = CH_AL_PWM_MODE_CM1;
        PWM_ADVANCE_TIMER->cm2 = CH_AL_PWM_MODE_CM2;
        PWM_ADVANCE_TIMER->cctrl = CH_AL_PWM_OUT_CCTRL;
        break;

      default:
        phase_step = 0;
        break;
    }

    /* Set PWM duty for all channels */
    PWM_ADVANCE_TIMER->c1dt = pwm_duty;
    PWM_ADVANCE_TIMER->c2dt = pwm_duty;
    PWM_ADVANCE_TIMER->c3dt = pwm_duty;

    /* Update output mode */
    tmr_event_sw_trigger(PWM_ADVANCE_TIMER, TMR_HALL_SWTRIG);

    /* Enable output */
    tmr_output_enable(PWM_ADVANCE_TIMER, TRUE);

    /* Delay for toggle period - safely handle us delay > 1ms */
    uint32_t delay_cnt = toggle_period_us;
    while(delay_cnt >= 1000)
    {
      mc_delay_ms(1);
      delay_cnt -= 1000;
    }
    if(delay_cnt > 0)
    {
      mc_delay_us(delay_cnt);
    }
    
    elapsed_us += toggle_period_us;

    /* Toggle phase */
    phase_step++;
    if(phase_step > 3) phase_step = 0;
  }

  /* Disable PWM output */
  tmr_output_enable(PWM_ADVANCE_TIMER, FALSE);

  /* Reset timer channels to safe state */
  PWM_ADVANCE_TIMER->c1dt = 0;
  PWM_ADVANCE_TIMER->c2dt = 0;
  PWM_ADVANCE_TIMER->c3dt = 0;
  
  /* Restore original PWM mode registers */
  PWM_ADVANCE_TIMER->cm1 = saved_cm1;
  PWM_ADVANCE_TIMER->cm2 = saved_cm2;
  PWM_ADVANCE_TIMER->cctrl = saved_cctrl;
  
  /* Update output mode from shadow registers to active registers */
  tmr_event_sw_trigger(PWM_ADVANCE_TIMER, TMR_HALL_SWTRIG);

  /* Clear any overcurrent error caused by the beep itself to prevent ESC lock-out */
  tmr_flag_clear(PWM_ADVANCE_TIMER, TMR_BRK_FLAG);
  if (!(saved_error_code & MC_OVER_CURRENT_ERROR))
  {
    error_code &= ~MC_OVER_CURRENT_ERROR;
  }

  /* Restore BRK configuration and re-enable break interrupt for normal operation */
  PWM_ADVANCE_TIMER->brk = saved_brk;
  tmr_interrupt_enable(PWM_ADVANCE_TIMER, TMR_BRK_INT, TRUE);
  
  /* Restore original OCP DAC threshold */
  dac_1_data_set(DAC_OCP_REF);
}
/**
  * @brief  Play "Castle in the Sky" (Laputa) melody
  * @param  volume : Volume level (0-100)
  * @retval none
  */
void motor_play_laputa(uint8_t volume)
{
  /* Musical Note Frequencies (Hz) */
  #define NOTE_G4  392
  #define NOTE_A4  440
  #define NOTE_B4  494
  #define NOTE_C5  523
  #define NOTE_D5  587
  #define NOTE_E5  659
  #define NOTE_F5  698
  #define NOTE_G5  784

  /* Tempo: Quarter note = 400ms */
  uint32_t q = 400; 
  uint32_t h = q / 2; // Half note (eighth)
  
  /* Melody: 6 7 | 1' 7 1' 3' | 7 - - 3 | 6 5 6 1' | 5 - - */
  
  // 6 7
  motor_beep(NOTE_A4, h, volume); mc_delay_ms(50);
  motor_beep(NOTE_B4, h, volume); mc_delay_ms(50);

  // 1' 7 1' 3'
  motor_beep(NOTE_C5, q + h, volume); mc_delay_ms(50);
  motor_beep(NOTE_B4, h, volume);     mc_delay_ms(50);
  motor_beep(NOTE_C5, q, volume);     mc_delay_ms(50);
  motor_beep(NOTE_E5, q, volume);     mc_delay_ms(50);

  // 7 - - 3
  motor_beep(NOTE_B4, q * 2, volume); mc_delay_ms(50);
  motor_beep(NOTE_E5, h, volume);     mc_delay_ms(50); // Using E5 as high 3

  // 6 5 6 1'
  motor_beep(NOTE_A4, q + h, volume); mc_delay_ms(50);
  motor_beep(NOTE_G4, h, volume);     mc_delay_ms(50);
  motor_beep(NOTE_A4, q, volume);     mc_delay_ms(50);
  motor_beep(NOTE_C5, q, volume);     mc_delay_ms(50);

  // 5 - -
  motor_beep(NOTE_G4, q * 2, volume); mc_delay_ms(50);
}
 
