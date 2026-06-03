/**
  **************************************************************************
  this is my signal
  * @file     main.c
  * @brief    main program
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

/** @addtogroup bldc_1shunt_sensorless bldc_1shunt_sensorless
  * @{
  */

/** @defgroup main
  * @brief main program
  * @{
  */
extern volatile uint32_t system_time_ms;
crm_clocks_freq_type crm_clocks_freq_struct = {0};
void check_configured_core_clock(void);
/**
  * @brief  main function.
  * @param  none
  * @retval none
  */
int main(void)
{
  system_clock_config();

  /* enable clock for remap or gpio exint */
  remap_exint_clock_enable_config();

  systick_init();

  opa_init();

  /* initialize delay function */
  mc_delay_init();

  led_init();
  pull_up_config();

  nvic_config();

  firmware_id = get_fw_id();
  param_initial_rdy = (flag_status)mc_param_init(firmware_id);

  /* COMP initialization */
  ocp_cmp_config();
  cmp2_config();

  tmr_sensorless_change_phase_init();

  tmr_pwm_init();
#if  defined DSHOT600_INPUT || defined DSHOT600_BIDIRECTIONAL
  dshot_input_timer_init();
#endif

  /* ADC initialization */
  adc_preempt_config();
  adc_ordinary_config();

  /* voltage calibration */
  mc_delay_ms(100);
  /* clear error */
  error_code = MC_NO_ERROR;
  get_int_vref_cal_ratio();

  tmr_comp_capture_init();

  /* enable blank timer */
  tmr_blank_trigger_init();
  tmr_blank_init();

  /* UART initialization */
#if defined USE_MOTOR_MONITOR
  uart_init(&ui_usart);
  dma_uart_configuration();
  uart_rx_init();
#endif

  /* current offset initialization */
  I_offset_init();
 

 
  /* Reset pwm timer */
  tmr_pwm_init();
    /* enable pwm timer */
  tmr_counter_enable(PWM_ADVANCE_TIMER, TRUE);
	mc_delay_ms(100);
	motor_play_startup_tone(80);

  gpio_output_init(); //for test
    /* enable pwm timer */
  tmr_counter_enable(PWM_ADVANCE_TIMER, TRUE);


  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);

  param_init();
	/*官方人员添加的代码*/
	mc_delay_ms(500);
	vdc_ratio = calcVdcRatio(adc_sample.vref_cal_ratio);
	openloop.olc_init_volt = adjustValueByVdc(openloop.olc_init_volt,vdc_ratio);
	openloop.olc_volt_inc = adjustValueByVdc(openloop.olc_volt_inc,vdc_ratio);
  led_blink();

  
  while(1)
  {
    /* calculate motor speed */
    if(calc_spd_rdy == SET)
    {
      /* 弧度/相邻零交叉事件之间的时间间隔 * 方向矢量 */
      rotor_speed.filtered = (uint32_t)(PWM_CVAL_TO_RPM / rotor_speed.interval_filter.long_word) * (1 - 2 * rotor_speed.dir);
      i32_speed_filterd = lowpass_filtering(&speed_LPF, rotor_speed.filtered);  //对电机速度进行低通滤波
      calc_spd_rdy = RESET;
    }

    if (esc_state != esc_state_old)
    {
      ESC_State_Init(esc_state);
      esc_state_old = esc_state;
    }
  }
}

/**
  * @brief  Check the core frequency matching the required frequency
  * @param  none
  * @retval none
  */
void check_configured_core_clock(void)
{
  if (system_core_clock != SYSTEM_CORE_CLOCK)
  {
    printf("Core freq.: %d, does not match the required freq. : %d", system_core_clock, SYSTEM_CORE_CLOCK);
    while(1);
  }
}

