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

/** @addtogroup bldc_1shunt_hall_sensor bldc_1shunt_hall_sensor
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

  opa_init();

  /* initialize delay function */
  mc_delay_init();

  led_init();

  nvic_config();

  firmware_id = get_fw_id();
  param_initial_rdy = (flag_status)mc_param_init(firmware_id);

  /* COMP initialization */
  ocp_cmp_config();

  gpio_hall_init();

  tmr_hall_init();
  tmr_pwm_init();
#if defined PWM_INPUT
  pwm_in_timer_init();
#endif

  /* ADC initialization */
  adc_preempt_config();
  adc_ordinary_config();
  /* voltage calibration */
  get_int_vref_cal_ratio();
  /* enable pwm timer */
  tmr_counter_enable(PWM_ADVANCE_TIMER, TRUE);

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

  /* get system clock */
  crm_clocks_freq_get(&crm_clocks_freq_struct);

  param_init();

  while(1)
  {
    if(hall_learn.check_flag == SET)
    {
      write_flash_cmd();
      hall_learn.check_flag = RESET;
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

