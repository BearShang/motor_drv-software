/**
  **************************************************************************
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

/** @addtogroup pmsm_foc_sensorless pmsm_foc_sensorless
  * @{
  */

/** @defgroup main
  * @brief main program
  * @{
  */

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

  mc_delay_init();

  gpio_pins_init();

  led_config();

  opa_init();

  ocp_cmp_config();

  nvic_config();

  adc_ordinary_config();

  adc_preempt_config();

  get_int_vref_cal_ratio();

  tmr_pwm_init();

  speed_timer_init();

#if defined PWM_INPUT
  pwm_in_timer_init();
#endif

#ifdef BRAKING_RESISTOR
  /* pwm init for brake resistor */
  brake_pwm_init();
#endif

  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);

  /* uart initialization */
#if defined USE_MOTOR_MONITOR
  uart_init(&ui_usart);
  dma_uart_configuration();
  uart_rx_init();
#endif

  /*delay for hardware stable */
  mc_delay_ms(500);

#ifdef OP_INP_MODE_SWITCH
  sw_op_inp_mode_timer_init();
#endif

  /* enable pwm timer */
  enable_pwm_timer(&pwm_duty);

#ifdef VOLT_SENSE
  voltage_offset_init(&(motor_emf.emf_abc_offset), &motor_voltage);
#endif

  /* current offset initialization */
  curr_offset_rdy = I_offset_init(&current);

  firmware_id = get_fw_id();
  param_initial_rdy = mc_param_init(firmware_id);

  param_init();

  led_blink();

  while(1)
  {
    if (ctrl_mode_cmd != ctrl_mode_old)
    {
      param_clear();
      start_stop_btn_flag = RESET;

      ctrl_mode = ctrl_mode_cmd;
      ctrl_mode_old = ctrl_mode;
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

/**
  * @}
  */
