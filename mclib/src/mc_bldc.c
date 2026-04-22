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
  * @brief  Generate beep sound using motor
  * @param  frequency : Beep frequency in Hz (100Hz - 5kHz recommended)
  * @param  duration : Beep duration in milliseconds
  * @param  volume : Volume level (0-100%), recommended 5-20% to prevent overheating
  * @retval none
  * @note   Safe beep principle: Fixed AH-BL phase, AH PWM modulation (low duty),
  *         BL always on. Current path: VDC->AH->PhaseA->PhaseB->BL->GND
  *         Low duty (5-20%) ensures low average current, MOS won't overheat
  */
void motor_beep(uint16_t frequency, uint32_t duration, uint8_t volume)
{
  uint16_t temp;
  uint16_t pwm_on_cval;
  uint32_t half_period_us;
  uint32_t start_time;
  uint8_t toggle_state = 0;
  uint8_t phase_state = 0;

  /* Limit frequency range (200Hz - 2kHz is optimal for beeping) */
  if(frequency < 200) frequency = 200;
  if(frequency > 2000) frequency = 2000;

  /* Limit volume to 5-15% to prevent MOS overheating */
  if(volume > 15) volume = 15;
  if(volume < 5) volume = 5;

  /* Calculate PWM duty: volume% on time */
  pwm_on_cval = ((uint32_t)volume * PWM_PERIOD) / 100;

  /* pwm timer output disable */
  tmr_output_enable(PWM_ADVANCE_TIMER, FALSE);
  /* disable pwm timer */
  tmr_counter_enable(PWM_ADVANCE_TIMER, FALSE);

  /* reset counter value */
  PWM_ADVANCE_TIMER->cval = 0;

  /* Calculate half period in microseconds for better accuracy */
  half_period_us = 500000 / frequency;
  if(half_period_us < 500) half_period_us = 500; /* Minimum 0.5ms */

  /* Set start time */
  start_time = system_time_ms;

  /* Main beep loop */
  while((system_time_ms - start_time) < duration)
  {
    /* Cycle through different phase combinations */
    switch(phase_state % 3)
    {
      case 0:
        /* AH-BL (Phase A high-side, Phase B low-side) */
        temp = PWM_ADVANCE_TIMER->cm1;
        temp &= (~TMR_PWM_MODE_CM1_MASK);
        temp |= AH_BL_PWM_MODE_CM1;
        PWM_ADVANCE_TIMER->cm1 = temp;

        temp = PWM_ADVANCE_TIMER->cm2;
        temp &= (~TMR_PWM_MODE_CM2_MASK);
        temp |= AH_BL_PWM_MODE_CM2;
        PWM_ADVANCE_TIMER->cm2 = temp;

        temp = PWM_ADVANCE_TIMER->cctrl;
        temp &= (~TMR_PWM_OUT_MODE_MASK);
        temp |= AH_BL_PWM_OUT_CCTRL;
        PWM_ADVANCE_TIMER->cctrl = temp;
        break;

      case 1:
        /* BH-CL (Phase B high-side, Phase C low-side) */
        temp = PWM_ADVANCE_TIMER->cm1;
        temp &= (~TMR_PWM_MODE_CM1_MASK);
        temp |= BH_CL_PWM_MODE_CM1;
        PWM_ADVANCE_TIMER->cm1 = temp;

        temp = PWM_ADVANCE_TIMER->cm2;
        temp &= (~TMR_PWM_MODE_CM2_MASK);
        temp |= BH_CL_PWM_MODE_CM2;
        PWM_ADVANCE_TIMER->cm2 = temp;

        temp = PWM_ADVANCE_TIMER->cctrl;
        temp &= (~TMR_PWM_OUT_MODE_MASK);
        temp |= BH_CL_PWM_OUT_CCTRL;
        PWM_ADVANCE_TIMER->cctrl = temp;
        break;

      case 2:
        /* CH-AL (Phase C high-side, Phase A low-side) */
        temp = PWM_ADVANCE_TIMER->cm1;
        temp &= (~TMR_PWM_MODE_CM1_MASK);
        temp |= CH_AL_PWM_MODE_CM1;
        PWM_ADVANCE_TIMER->cm1 = temp;

        temp = PWM_ADVANCE_TIMER->cm2;
        temp &= (~TMR_PWM_MODE_CM2_MASK);
        temp |= CH_AL_PWM_MODE_CM2;
        PWM_ADVANCE_TIMER->cm2 = temp;

        temp = PWM_ADVANCE_TIMER->cctrl;
        temp &= (~TMR_PWM_OUT_MODE_MASK);
        temp |= CH_AL_PWM_OUT_CCTRL;
        PWM_ADVANCE_TIMER->cctrl = temp;
        break;
    }

    if(toggle_state)
    {
      /* On state: PWM output with duty cycle */
      switch(phase_state % 3)
      {
        case 0:
          tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_1, pwm_on_cval);
          tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_2, 0);
          tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_3, 0);
          break;
        case 1:
          tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_1, 0);
          tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_2, pwm_on_cval);
          tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_3, 0);
          break;
        case 2:
          tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_1, 0);
          tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_2, 0);
          tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_3, pwm_on_cval);
          break;
      }
    }
    else
    {
      /* Off state: all channels off */
      tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_1, 0);
      tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_2, 0);
      tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_3, 0);
    }

    /* update pwm output mode from shadow buffer */
    tmr_event_sw_trigger(PWM_ADVANCE_TIMER, TMR_HALL_SWTRIG);

    /* enable pwm timer and output */
    tmr_counter_enable(PWM_ADVANCE_TIMER, TRUE);
    tmr_output_enable(PWM_ADVANCE_TIMER, TRUE);

    /* Wait for half period using microsecond delay for better accuracy */
    mc_delay_us(half_period_us);

    /* Disable output before phase change */
    tmr_output_enable(PWM_ADVANCE_TIMER, FALSE);
    tmr_counter_enable(PWM_ADVANCE_TIMER, FALSE);
    
    /* Small delay to prevent shoot-through */
    mc_delay_us(2);

    /* Toggle state */
    toggle_state = !toggle_state;
    phase_state++;
  }

  /* Disable all outputs */
  tmr_output_enable(PWM_ADVANCE_TIMER, FALSE);
  tmr_counter_enable(PWM_ADVANCE_TIMER, FALSE);

  /* Reset PWM values to 0 */
  tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_1, 0);
  tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_2, 0);
  tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_3, 0);
}

/**
  * @brief  Generate tone sequence using motor
  * @param  frequencies : Array of frequencies in Hz
  * @param  durations : Array of durations in milliseconds
  * @param  count : Number of tones in sequence
  * @param  volume : Volume level (0-100%)
  * @retval none
  */
void motor_tone_sequence(uint16_t *frequencies, uint32_t *durations, uint8_t count, uint8_t volume)
{
  for(uint8_t i = 0; i < count; i++)
  {
    motor_beep(frequencies[i], durations[i], volume);
  }
}
