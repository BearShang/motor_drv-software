/**
  **************************************************************************
  * @file     mc_hwio.c
  * @brief    Hardware peripheral configuration
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

/** @defgroup mc_hwio
  * @brief Hardware peripheral configuration
  * @{
  */

__IO uint16_t adc_in_tab[ADC_IDX_MAX] = {0};

/**
  * @brief  initialization of nested vectored interrupt controller
  * @param  none
  * @retval none
  */
void nvic_config(void)
{
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);

  /* brake_in isr configuration*/
  nvic_irq_enable(ADVTMR_PWM_BRK_IRQn, 0, 0);
  /* sensorless change phase configuration*/
  nvic_irq_enable(CHANGE_PHASE_IRQn, 1, 0);

#if defined (BLDC_SENSORLESS_COMP)
  /* comp capture isr */
  nvic_irq_enable(COMP_OUT_CAPTURE_IRQn, 2, 0);
  /* pwm isr configuration*/
  nvic_irq_enable(ADVTMR_PWM_CYCLE_IRQn, 3, 0);
  /* adc isr configuration*/
  nvic_irq_enable(ADC_SHUNT_SAMP_READY_IRQn, 4, 0);
#elif defined (BLDC_SENSORLESS_ADC)
  /* adc isr configuration*/
  nvic_irq_enable(ADC_SHUNT_SAMP_READY_IRQn, 2, 0);
  /* pwm isr configuration*/
  nvic_irq_enable(ADVTMR_PWM_CYCLE_IRQn, 3, 0);
  /* comp capture isr */
  nvic_irq_enable(COMP_OUT_CAPTURE_IRQn, 4, 0);
#if defined (EMF_PULL_UP)
  nvic_irq_enable(ADVTMR_CH1_EMF_PULL_IRQn, 5, 0);
#endif
#endif
#if  defined DSHOT600_INPUT || defined DSHOT600_BIDIRECTIONAL
  /* dshot input interrupt nvic init */
  nvic_irq_enable(PWM_DUTY_INPUT_IRQn, 6, 0);
  nvic_irq_enable(DMA_DSHOT_INPUT_IRQn, 6, 1);
#endif
  /* systick nvic init */
  nvic_irq_enable(SysTick_IRQn, 7, 0);
  /* usart1 interrupt nvic init */
  nvic_irq_enable(COMM_UART_IRQn, 8, 0);
}

/**
  * @brief  initialization of a timer for PWM
  * @param  none
  * @retval none
  */
void tmr_pwm_init()
{
  gpio_init_type gpio_init_struct = {0};
  tmr_output_config_type tmr_output_struct;
  tmr_brkdt_config_type tmr_brkdt_config_struct = {0};
  tmr_brkin_config_type tmr_brkin_config_struct;

  /* enable pwm timer/high side gpio/low side gpio clock */
  crm_periph_clock_enable(PWM_ADVANCE_TIMER_CRM_CLK, TRUE);
  crm_periph_clock_enable(PWM_PHASE_A_HI_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(PWM_PHASE_B_HI_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(PWM_PHASE_C_HI_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(PWM_PHASE_A_LOW_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(PWM_PHASE_B_LOW_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(PWM_PHASE_C_LOW_GPIO_CRM_CLK, TRUE);

  /* timer pwm output pin Configuration */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;

  /* High-side, Phase A,B,C Config */
  gpio_init_struct.gpio_pins = PWM_PHASE_A_HI_GPIO_PIN;
  gpio_init(PWM_PHASE_A_HI_PORT, &gpio_init_struct);
  gpio_pin_mux_config(PWM_PHASE_A_HI_PORT, PWM_PHASE_A_HI_PIN_SOURCE, PWM_PHASE_A_HI_IOMUX);

  gpio_init_struct.gpio_pins = PWM_PHASE_B_HI_GPIO_PIN;
  gpio_init(PWM_PHASE_B_HI_PORT, &gpio_init_struct);
  gpio_pin_mux_config(PWM_PHASE_B_HI_PORT, PWM_PHASE_B_HI_PIN_SOURCE, PWM_PHASE_B_HI_IOMUX);

  gpio_init_struct.gpio_pins = PWM_PHASE_C_HI_GPIO_PIN;
  gpio_init(PWM_PHASE_C_HI_PORT, &gpio_init_struct);
  gpio_pin_mux_config(PWM_PHASE_C_HI_PORT, PWM_PHASE_C_HI_PIN_SOURCE, PWM_PHASE_C_HI_IOMUX);

  /* Low-side, Phase A,B,C Config */
#if defined GATE_DRIVER_LOW_SIDE_INVERT
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
#else
  gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
#endif

  gpio_init_struct.gpio_pins = PWM_PHASE_A_LOW_GPIO_PIN;
  gpio_init(PWM_PHASE_A_LOW_PORT, &gpio_init_struct);
  gpio_pin_mux_config(PWM_PHASE_A_LOW_PORT, PWM_PHASE_A_LOW_PIN_SOURCE, PWM_PHASE_A_LOW_IOMUX);

  gpio_init_struct.gpio_pins = PWM_PHASE_B_LOW_GPIO_PIN;
  gpio_init(PWM_PHASE_B_LOW_PORT, &gpio_init_struct);
  gpio_pin_mux_config(PWM_PHASE_B_LOW_PORT, PWM_PHASE_B_LOW_PIN_SOURCE, PWM_PHASE_B_LOW_IOMUX);

  gpio_init_struct.gpio_pins = PWM_PHASE_C_LOW_GPIO_PIN;
  gpio_init(PWM_PHASE_C_LOW_PORT, &gpio_init_struct);
  gpio_pin_mux_config(PWM_PHASE_C_LOW_PORT, PWM_PHASE_C_LOW_PIN_SOURCE, PWM_PHASE_C_LOW_IOMUX);

  /* disable brkin */
  tmr_brkdt_config_struct.brk_enable = FALSE;
  tmr_brkdt_config(PWM_ADVANCE_TIMER, &tmr_brkdt_config_struct);

  /* clear interupt flag */
  tmr_flag_clear(PWM_ADVANCE_TIMER, TMR_BRK_FLAG);

  /* disable brkin ISR */
  tmr_interrupt_enable(PWM_ADVANCE_TIMER, TMR_BRK_INT, FALSE);

  /* disable pwm timer */
  tmr_counter_enable(PWM_ADVANCE_TIMER, FALSE);

  /* reset repetition counter */
  tmr_repetition_counter_set(PWM_ADVANCE_TIMER, 0);

  /* set pwm timer period */
  tmr_base_init(PWM_ADVANCE_TIMER, (PWM_PERIOD - 1), 0);

  /* enable pwm timer period buffer */
  tmr_period_buffer_enable(PWM_ADVANCE_TIMER, TRUE);

  /* set pwm timer count up */
  tmr_cnt_dir_set(PWM_ADVANCE_TIMER, TMR_COUNT_UP);

  /* channel 1, 2, and 3 configuration in low state */
  tmr_output_default_para_init(&tmr_output_struct);
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_OFF;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = FALSE;
#if defined GATE_DRIVER_LOW_SIDE_INVERT
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_LOW;
  tmr_output_struct.occ_idle_state = TRUE;
#else
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.occ_idle_state = FALSE;
#endif

  /* channel 1 */
  tmr_output_channel_config(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_1, &tmr_output_struct);
  tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_1, 0);
  tmr_output_channel_buffer_enable(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_1, TRUE);

  /* channel 2 */
  tmr_output_channel_config(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_2, &tmr_output_struct);
  tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_2, 0);
  tmr_output_channel_buffer_enable(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_2, TRUE);

  /* channel 3 */
  tmr_output_channel_config(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_3, &tmr_output_struct);
  tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_3, 0);
  tmr_output_channel_buffer_enable(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_3, TRUE);

  /* channel 4 */
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_B;
  tmr_output_struct.occ_output_state = FALSE;
  tmr_output_channel_config(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_4, &tmr_output_struct);
  tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_4, 0);
  tmr_output_channel_buffer_enable(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_4, FALSE);

  /* set dead time clock */
  tmr_clock_source_div_set(PWM_ADVANCE_TIMER, DEADTIME_CLK_SFT_BITS);

  /* automatic output enable, break, dead time and lock configuration */
  tmr_brkdt_default_para_init(&tmr_brkdt_config_struct);
  tmr_brkdt_config_struct.brk_enable = FALSE;
  tmr_brkdt_config_struct.auto_output_enable = FALSE;
  tmr_brkdt_config_struct.deadtime = DEADTIME;
  tmr_brkdt_config_struct.fcsodis_state = TRUE;
  tmr_brkdt_config_struct.fcsoen_state = TRUE;
  tmr_brkdt_config_struct.brk_polarity = TMR_BRK_INPUT_ACTIVE_HIGH;
  tmr_brkdt_config_struct.wp_level = TMR_WP_OFF;
  tmr_brkdt_config(PWM_ADVANCE_TIMER, &tmr_brkdt_config_struct);

  tmr_brk_filter_value_set(PWM_ADVANCE_TIMER, TMR_BRK_FILTER_COUNT);

  tmr_brkin_config_struct.enable = TRUE;
  tmr_brkin_config_struct.polarity = TMR_BRKIN_SRC_POL_HIGH;
  tmr_brkin_config_struct.source = TMR_BRKIN_SOURCE;
  tmr_brkin_config_struct.brk_sel = TMR_BRK_SELECT_1;
  tmr_brk_input_config(PWM_ADVANCE_TIMER, &tmr_brkin_config_struct);

  /* disable use hall state to change pwm output mode(auto change phase mode) */
  tmr_hall_select(PWM_ADVANCE_TIMER, FALSE);

  /* enable pwm output mode buffer */
  tmr_channel_buffer_enable(PWM_ADVANCE_TIMER, TRUE);

  /* clear interupt flag */
  tmr_flag_clear(PWM_ADVANCE_TIMER, TMR_OVF_FLAG);
  tmr_flag_clear(PWM_ADVANCE_TIMER, TMR_BRK_FLAG);

  /* enable Break in interrupt*/
  tmr_interrupt_enable(PWM_ADVANCE_TIMER, TMR_BRK_INT, TRUE);
  /* enable update interrupts of pwm timer */
  tmr_interrupt_enable(PWM_ADVANCE_TIMER, TMR_OVF_INT, TRUE);

  /* disable single pulse mode */
  tmr_one_cycle_mode_enable(PWM_ADVANCE_TIMER, FALSE);

  /* pwm timer output disable */
  tmr_output_enable(PWM_ADVANCE_TIMER, FALSE);

  /* reset counter value */
  PWM_ADVANCE_TIMER->cval = 0;

  /* primary mode selection: TMR1 */
  tmr_sub_sync_mode_set(PWM_ADVANCE_TIMER, TRUE);
  tmr_primary_mode_select(PWM_ADVANCE_TIMER, TMR_PRIMARY_SEL_OVERFLOW);
}

/**
  * @brief Initializes the timer for comparator output capture.
  *
  * This function configures a timer to capture events from a comparator output.
  * It enables the necessary clocks, initializes the timer base, sets the counter
  * direction, and configures the input channel for capture. It also clears
  * any pending flags and ensures the timer is initially disabled.
  *
  * @param None
  * @return None
  */
void tmr_comp_capture_init()
{
  tmr_input_config_type tmr_input_config_struct;

  /* Enable the clock for the comparator output capture timer. */
  crm_periph_clock_enable(COMP_OUT_CAPTURE_CRM_CLK, TRUE);
  tmr_clock_source_div_set(COMP_OUT_CAPTURE_TIMER, COMP_OUT_CAPTURE_FILTER_CLK_DIV);

  /* Initialize the timer base with a maximum count and clock division. */
  tmr_base_init(COMP_OUT_CAPTURE_TIMER, MAX_CAP_COUNT, TIM_CAP_CLK_DIV);
  /* Set the timer to count upwards. */
  tmr_cnt_dir_set(COMP_OUT_CAPTURE_TIMER, TMR_COUNT_UP);

  /* Configure the timer input channel for capture. */
  tmr_input_config_struct.input_channel_select = COMP_OUT_SELECT_CHANNEL;
  tmr_input_config_struct.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
  tmr_input_config_struct.input_polarity_select = TMR_INPUT_RISING_EDGE;
  tmr_input_config_struct.input_filter_value = TMR_COMP_OUT_CAPTURE_FILTER;
  tmr_input_channel_init(COMP_OUT_CAPTURE_TIMER, &tmr_input_config_struct, TMR_CHANNEL_INPUT_DIV_1);

  tmr_input_ch_select(COMP_OUT_CAPTURE_TIMER, COMP_OUT_CAPTURE_INPUT);

  /* clear flags of ch1 events of comparator capture timer */
  tmr_flag_clear(COMP_OUT_CAPTURE_TIMER, COMP_OUT_FLAG);
  tmr_interrupt_enable(COMP_OUT_CAPTURE_TIMER, COMP_OUT_INT, FALSE);

  /* disable COMP cature timer */
  tmr_counter_enable(COMP_OUT_CAPTURE_TIMER, FALSE);
}

/**
  * @brief  Initialize the timer for DSHOT600 input measurement.
  * @param  none
  * @retval none
  */
#if  defined DSHOT600_INPUT || defined DSHOT600_BIDIRECTIONAL
volatile uint8_t dshot_bidir_state = 0;
uint16_t dshot_bidir_rx_buffer[DSHOT_DMA_CAPTURE_BUFFER_SIZE]; 
//重置硬件函数
void dshot_bidir_reset_hardware(void)
{
    // 1. 禁用定时器和DMA
    tmr_counter_enable(PWM_DUTY_INPUT_TIMER, FALSE);
    dma_channel_enable(DMA_CHANNEL_DSHOT_INPUT, FALSE);
    
    // 2. 禁用所有定时器DMA请求
    tmr_dma_request_enable(PWM_DUTY_INPUT_TIMER, TMR_C3_DMA_REQUEST, FALSE);
    tmr_dma_request_enable(PWM_DUTY_INPUT_TIMER, TMR_OVERFLOW_DMA_REQUEST, FALSE);
    
    // 3. 完全重置硬件
    tmr_reset(PWM_DUTY_INPUT_TIMER);
    dma_reset(DMA_CHANNEL_DSHOT_INPUT);
}

void dshot_input_timer_init(void)
{
    //重置硬件
    dshot_bidir_reset_hardware();
    
    // 4. 重新配置GPIO为输入模式
  gpio_init_type gpio_init_struct = {0};
  tmr_input_config_type tmr_ic_init_structure;
  dma_init_type dma_init_struct;

  crm_periph_clock_enable(PWM_DUTY_INPUT_CRM_CLK, TRUE);
  crm_periph_clock_enable(PWM_DUTY_INPUT_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(DMA_DSHOT_INPUT_CRM_CLK, TRUE);

  /* dshot input pin Configuration */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_pins = PWM_DUTY_INPUT_GPIO_PIN;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  #if defined DSHOT600_INPUT
    gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
  #else
    gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  #endif
 
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(PWM_DUTY_INPUT_PORT, &gpio_init_struct);

  /* remap pwm input pin */
  gpio_pin_mux_config(PWM_DUTY_INPUT_PORT, PWM_DUTY_INPUT_GPIO_PIN_SOURCE, PWM_DUTY_INPUT_IOMUX);

  /* dshot input timer configuration */
  tmr_base_init(PWM_DUTY_INPUT_TIMER, MAX_CAP_COUNT, DSHOT_INPUT_TIMER_DIV);
  tmr_cnt_dir_set(PWM_DUTY_INPUT_TIMER, TMR_COUNT_UP);

  /* capture both edges into the same timer channel and stream timestamps through DMA */
  tmr_input_default_para_init(&tmr_ic_init_structure);
  tmr_ic_init_structure.input_channel_select = PWM_DUTY_INPUT_SELECT_CHANNEL;
  tmr_ic_init_structure.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
  tmr_ic_init_structure.input_polarity_select = TMR_INPUT_BOTH_EDGE;
  tmr_ic_init_structure.input_filter_value = TMR_PWM_DUTY_INPUT_FILTER;
  tmr_input_channel_init(PWM_DUTY_INPUT_TIMER, &tmr_ic_init_structure, TMR_CHANNEL_INPUT_DIV_1);

  dma_reset(DMA_CHANNEL_DSHOT_INPUT);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = DSHOT_DMA_CAPTURE_BUFFER_SIZE;
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
  dma_init_struct.memory_base_addr = (uint32_t)dshot_dma_capture_buffer;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(PWM_DUTY_INPUT_TIMER->c3dt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_VERY_HIGH;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(DMA_CHANNEL_DSHOT_INPUT, &dma_init_struct);
  dma_interrupt_enable(DMA_CHANNEL_DSHOT_INPUT, DMA_HDT_INT | DMA_FDT_INT | DMA_DTERR_INT, TRUE);

  dmamux_enable(DMA_DSHOT_INPUT, TRUE);
  dmamux_init(DMA_DSHOT_INPUT_FLEX_CH, DMA_DSHOT_INPUT_FLEX);

  tmr_channel_dma_select(PWM_DUTY_INPUT_TIMER, TMR_DMA_REQUEST_BY_CHANNEL);
  tmr_dma_request_enable(PWM_DUTY_INPUT_TIMER, TMR_C3_DMA_REQUEST, TRUE);

  /* clear interrupt flag of dshot input timer */
  tmr_flag_clear(PWM_DUTY_INPUT_TIMER, PWM_DUTY_INPUT_FLAG);
  tmr_flag_clear(PWM_DUTY_INPUT_TIMER, TMR_OVF_FLAG);

  /* keep only overflow interrupt for signal-loss detection */
  tmr_interrupt_enable(PWM_DUTY_INPUT_TIMER, PWM_DUTY_INPUT_INT, FALSE);
  tmr_interrupt_enable(PWM_DUTY_INPUT_TIMER, TMR_OVF_INT, TRUE);

  dma_channel_enable(DMA_CHANNEL_DSHOT_INPUT, TRUE);
  /* enable dshot input timer */
  tmr_counter_enable(PWM_DUTY_INPUT_TIMER, TRUE);
		
    // 初始化状态
    dshot_bidir_state = 0;  // 0: 空闲
}
// GCR编码查找表（4位数据 -> 5位GCR）
const uint8_t dshot_gcr_encode_lut[16] = {
    0x19, 0x1B, 0x12, 0x13, 0x1D, 0x15, 0x16, 0x17,
    0x1A, 0x09, 0x0A, 0x0B, 0x1E, 0x0D, 0x0E, 0x0F
};
/**
 * @brief  构建21位遥测波形的CCR数组（PWM模式B）
 * @param  telemetry_data: 16位遥测数据（已包含CRC）
 * @param  ccr_buf: 输出CCR数组（长度至少21）
 * @param  period: 定时器周期值（对应GCR周期）
 * @return 实际位数（固定21）
 */
static uint8_t dshot_build_ccr_buffer(uint16_t telemetry_data, uint16_t *ccr_buf, uint16_t period)
{
    // 1. 编码为20位GCR
    uint32_t gcr_20bit = 0;
    for (int i = 3; i >= 0; i--) {
        uint8_t nibble = (telemetry_data >> (i * 4)) & 0x0F;
        gcr_20bit = (gcr_20bit << 5) | dshot_gcr_encode_lut[nibble];
    }
    
    // 起始位：强制低电平
    ccr_buf[0] = period;
    uint8_t current_level = 0;  // 起始位结束后电平为低
    
    // 编码剩余20位（GCR数据，从最高位到最低位）
    for (int i = 19; i >= 0; i--) {
        uint8_t bit = (gcr_20bit >> i) & 0x01;
        if (bit) {
            current_level = !current_level;  // 翻转电平
        }
        ccr_buf[20 - i] = (current_level == 1) ? 0 : period;
    }
    
    return 21;
}
/**
 * @brief  双向DShot发送遥测数据（方案3实现 - PWM模式 + DMA）
 * @param  telemetry_data: 16位遥测数据
 * @note   使用PWM模式B + DMA更新占空比生成NRZ波形，符合标准双向DShot回传格式
 */
void dshot_bidir_send_telemetry_nrz(uint16_t telemetry_data)
{
    // 检查DMA是否空闲
    if (dma_data_number_get(DMA_CHANNEL_DSHOT_INPUT) != 0) return;

    // 1. 禁用当前定时器和DMA（接收模式）
    tmr_counter_enable(PWM_DUTY_INPUT_TIMER, FALSE);
    dma_channel_enable(DMA_CHANNEL_DSHOT_INPUT, FALSE);
    tmr_dma_request_enable(PWM_DUTY_INPUT_TIMER, TMR_OVERFLOW_DMA_REQUEST, FALSE);
    tmr_dma_request_enable(PWM_DUTY_INPUT_TIMER, TMR_C3_DMA_REQUEST, FALSE);

    // 2. 配置GPIO为复用输出（PWM）
    gpio_init_type gpio_struct;
    gpio_default_para_init(&gpio_struct);
    gpio_struct.gpio_pins = PWM_DUTY_INPUT_GPIO_PIN;
    gpio_struct.gpio_mode = GPIO_MODE_MUX;
    gpio_struct.gpio_pull = GPIO_PULL_UP;
    gpio_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
    gpio_init(PWM_DUTY_INPUT_PORT, &gpio_struct);
    gpio_pin_mux_config(PWM_DUTY_INPUT_PORT, PWM_DUTY_INPUT_GPIO_PIN_SOURCE, PWM_DUTY_INPUT_IOMUX);

    // 3. 计算GCR比特率和周期
    uint32_t gcr_bitrate = DSHOT600_BITRATE * 5 / 4;
    uint32_t period = (DSHOT_INPUT_TIMER_CLK + gcr_bitrate/2) / gcr_bitrate; // 四舍五入
    if (period < 2) period = 2;
    period--;  // ARR = period-1

    // 4. 配置定时器为PWM模式B
    tmr_base_init(PWM_DUTY_INPUT_TIMER, period, DSHOT_INPUT_TIMER_DIV);
    tmr_cnt_dir_set(PWM_DUTY_INPUT_TIMER, TMR_COUNT_UP);
    tmr_clock_source_div_set(PWM_DUTY_INPUT_TIMER, TMR_CLOCK_DIV1);

    tmr_output_config_type out_cfg;
    tmr_output_default_para_init(&out_cfg);
    out_cfg.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_B;
    out_cfg.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
    out_cfg.oc_output_state = TRUE;
    tmr_output_channel_config(PWM_DUTY_INPUT_TIMER, PWM_DUTY_INPUT_SELECT_CHANNEL, &out_cfg);
    tmr_output_channel_buffer_enable(PWM_DUTY_INPUT_TIMER, PWM_DUTY_INPUT_SELECT_CHANNEL, TRUE);
    tmr_channel_enable(PWM_DUTY_INPUT_TIMER, PWM_DUTY_INPUT_SELECT_CHANNEL, TRUE);
    tmr_output_enable(PWM_DUTY_INPUT_TIMER, TRUE);   // 使能输出

    // 5. 构建CCR缓冲区（21位 + 2空闲）
    uint16_t ccr_buf[23];
    dshot_build_ccr_buffer(telemetry_data, ccr_buf, period + 1);
    ccr_buf[21] = 0;
    ccr_buf[22] = 0;

    // 6. 配置DMA
    dma_reset(DMA_CHANNEL_DSHOT_INPUT);
    dma_init_type dma_cfg;
    dma_default_para_init(&dma_cfg);
    dma_cfg.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
    dma_cfg.memory_base_addr = (uint32_t)ccr_buf;
    dma_cfg.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
    dma_cfg.memory_inc_enable = TRUE;
    dma_cfg.peripheral_base_addr = (uint32_t)&PWM_DUTY_INPUT_TIMER->c3dt;
    dma_cfg.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
    dma_cfg.peripheral_inc_enable = FALSE;
    dma_cfg.priority = DMA_PRIORITY_HIGH;
    dma_cfg.buffer_size = 23;
    dma_cfg.loop_mode_enable = FALSE;
    dma_init(DMA_CHANNEL_DSHOT_INPUT, &dma_cfg);
    dmamux_init(DMA1MUX_CHANNEL7, DMAMUX_DMAREQ_ID_TMR4_OVERFLOW);
    dmamux_enable(DMA1, TRUE);
    dma_interrupt_enable(DMA_CHANNEL_DSHOT_INPUT, DMA_FDT_INT, TRUE);
    // nvic_irq_enable(DMA_DSHOT_INPUT_IRQn, 6, 0);

    // 7. 启动定时器和DMA
    tmr_counter_value_set(PWM_DUTY_INPUT_TIMER, 0);
    tmr_dma_request_enable(PWM_DUTY_INPUT_TIMER, TMR_OVERFLOW_DMA_REQUEST, TRUE);
    dma_channel_enable(DMA_CHANNEL_DSHOT_INPUT, TRUE);
    tmr_counter_enable(PWM_DUTY_INPUT_TIMER, TRUE);

    // 8. 标记发送状态（DMA中断中会切换回接收）
    dshot_bidir_state = 1;
}
#endif

/**
  * @brief  initialization of a timer for sensorless change phase
  * @param  none
  * @retval none
  */
void tmr_sensorless_change_phase_init()
{
  /* enable timer and gpio clock */
  crm_periph_clock_enable(CHANGE_PHASE_CRM_CLK, TRUE);

  tmr_base_init(CHANGE_PHASE_TIMER, MAX_CAP_COUNT, CHANGE_PHASE_TMR_DIV);
  tmr_cnt_dir_set(CHANGE_PHASE_TIMER, TMR_COUNT_UP);

  /* clear flag */
  tmr_flag_clear(CHANGE_PHASE_TIMER, TMR_OVF_FLAG);

  /* enable overflow interrupt of change phase timer */
  tmr_interrupt_enable(CHANGE_PHASE_TIMER, TMR_OVF_INT, TRUE);

  /* disable change phase timer */
  tmr_counter_enable(CHANGE_PHASE_TIMER, FALSE);
}

/**
 * @brief Initializes a timer for blanking signal generation.
 * @param None
 * @return None
 */
void tmr_blank_init(void)
{
  tmr_output_config_type tmr_output_struct;
  dma_init_type dma_init_struct;

  /* enable timer and gpio clock */
  crm_periph_clock_enable(BLANK_CRM_CLK, TRUE);

  tmr_base_init(blank.TMRx, (PWM_PERIOD - 1), 0);
  tmr_cnt_dir_set(blank.TMRx, TMR_COUNT_UP);

  tmr_output_default_para_init(&tmr_output_struct);
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = FALSE;
  tmr_output_struct.occ_output_state = TRUE;
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_LOW;
  tmr_output_struct.occ_idle_state = TRUE;

  /* channel 1 */
  tmr_output_channel_config(blank.TMRx, blank.TMR_CH, &tmr_output_struct);
  tmr_channel_value_set(blank.TMRx, blank.TMR_CH, 500);
  tmr_output_channel_buffer_enable(blank.TMRx, blank.TMR_CH, TRUE);

  /* subordinate mode selection */
  tmr_sub_mode_select(blank.TMRx, TMR_SUB_RESET_MODE);
  tmr_trigger_input_select(blank.TMRx, BLANK_TMR_SYNC_INPUT_SEL);
  
  /* dma configuration */
  dma_reset(DMA_CHANNEL_BLANK_WINDOW);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = 2;
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_base_addr = (uint32_t)&blank.blank_window_dt;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t) & (blank.TMRx->c1dt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(DMA_CHANNEL_BLANK_WINDOW, &dma_init_struct);
  
  /* dmamux function enable */
  dmamux_enable(DMA_BLANK_WINDOW, TRUE);
  dmamux_init(DMA_BLANK_WINDOW_FLEX_CH, DMA_BLANK_WINDOW_FLEX);
  
  /* enable dma channe4 */
  dma_channel_enable(DMA_CHANNEL_BLANK_WINDOW, TRUE);
}

/**
 * @brief Initializes a timer to generate a blanking trigger signal.
 * @param None
 * @return None
 */
void tmr_blank_trigger_init(void)
{
  tmr_output_config_type tmr_output_struct;
  dma_init_type dma_init_struct;
  dmamux_gen_init_type  dmamux_gen_init_struct;

  /* enable timer and gpio clock */
  crm_periph_clock_enable(BLANK_TRIGGER_CRM_CLK, TRUE);

  tmr_base_init(blank_trigger.TMRx, (PWM_PERIOD - 1), 0);
  tmr_cnt_dir_set(blank_trigger.TMRx, TMR_COUNT_UP);

  tmr_output_default_para_init(&tmr_output_struct);
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_B;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = FALSE;
  tmr_output_struct.occ_output_state = TRUE;
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_LOW;
  tmr_output_struct.occ_idle_state = TRUE;

  /* channel 1 */
  tmr_output_channel_config(blank_trigger.TMRx, BLANK_TRIGGER_SELECT_CHANNEL, &tmr_output_struct);
  tmr_channel_value_set(blank_trigger.TMRx, BLANK_TRIGGER_SELECT_CHANNEL, 10);
  tmr_output_channel_buffer_enable(blank_trigger.TMRx, BLANK_TRIGGER_SELECT_CHANNEL, FALSE);

  /* enable timer dma request : TMR16CH1 */
  tmr_dma_request_enable(blank_trigger.TMRx, DMA_BLANK_TRIGGER_REQUEST, TRUE);

  /* disable change phase timer */
  tmr_counter_enable(blank_trigger.TMRx, FALSE);
  
  /* primary mode selection: TMR3 */
  tmr_sub_sync_mode_set(blank_trigger.TMRx, TRUE);
  tmr_primary_mode_select(blank_trigger.TMRx, TMR_PRIMARY_SEL_C1ORAW);
  
  /* subordinate mode selection */
  tmr_sub_mode_select(blank_trigger.TMRx, TMR_SUB_RESET_MODE);
  tmr_trigger_input_select(blank_trigger.TMRx, BLANK_TRIGGER_SYNC_INPUT_SEL);

  /* dma configuration */
  dma_reset(DMA_CHANNEL_BLANK_TRIGGER);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = 2;
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_base_addr = (uint32_t)&blank_trigger.sample_point;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t) & (blank_trigger.TMRx->c1dt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(DMA_CHANNEL_BLANK_TRIGGER, &dma_init_struct);
  
  /* genertor1 configuration */
  dmamux_generator_default_para_init(&dmamux_gen_init_struct);
  dmamux_gen_init_struct.gen_polarity = DMAMUX_GEN_POLARITY_RISING;
  dmamux_gen_init_struct.gen_request_number = 1;
  dmamux_gen_init_struct.gen_signal_sel = DMA_BLANK_TRIGGER_GEN_SIGNAL;
  dmamux_gen_init_struct.gen_enable = TRUE;
  dmamux_generator_config(DMA1MUX_GENERATOR1, &dmamux_gen_init_struct);
  
  DMA_BLANK_TRIGGER_FLEX_CH->muxctrl_bit.evtgen = TRUE;

  /* dmamux function enable */
  dmamux_enable(DMA_BLANK_TRIGGER, TRUE);
  dmamux_init(DMA_BLANK_TRIGGER_FLEX_CH, DMA_BLANK_TRIGGER_FLEX);

  dma_channel_enable(DMA_CHANNEL_BLANK_TRIGGER, FALSE);
}

/**
 * @brief Configures the Analog-to-Digital Converter (ADC) for ordinary conversions.
 * @param None
 * @return None
 */
void adc_ordinary_config(void)
{
  gpio_init_type gpio_init_struct = {0};
  adc_common_config_type adc_common_struct;
  adc_base_config_type adc_base_struct;
  dma_init_type dma_init_struct;

  /* Disable the ADC initially before configuration. */
  adc_enable(ADC_NORMAL_CONVERTER, FALSE);

  /* Initialize ADC common parameters. */
  adc_common_default_para_init(&adc_common_struct);
  /* Configure ADC clock division from HCLK.
   * Example: HCLK Max. CLK = 180M Hz, ADC_CLK = 180/6 = 30M Hz. */
  adc_common_struct.div = ADC_CONVERTER_CRM_CLK_DIV;

  /* Configure ADC combine mode to independent mode (not synchronized with other ADCs). */
  adc_common_struct.combine_mode = ADC_INDEPENDENT_MODE;

  /* Disable inner temperature sensor and VREFINT (internal voltage reference). */
  adc_common_struct.tempervintrv_state = FALSE;
  adc_common_config(&adc_common_struct);

  /* Enable clocks for the ADC module and the GPIO ports used for analog inputs. */
  crm_periph_clock_enable(ADC_NORMAL_CRM_CLK, TRUE);
  crm_periph_clock_enable(VOLT_BUS_ADC_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(MOS_TEMP_ADC_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(POTENTIO_ADC_GPIO_CRM_CLK, TRUE);

  /* Enable the clock for the DMA module used with the ADC. */
  crm_periph_clock_enable(DMA_ADC_CRM_CLK, TRUE);

  /* GPIO configuration for analog input pins. */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

  gpio_init_struct.gpio_pins = VOLT_BUS_ADC_GPIO_PIN;
  gpio_init(VOLT_BUS_ADC_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = MOS_TEMP_ADC_GPIO_PIN;
  gpio_init(MOS_TEMP_ADC_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = POTENTIO_ADC_GPIO_PIN;
  gpio_init(POTENTIO_ADC_PORT, &gpio_init_struct);

  /* DMA configuration for ADC data transfer. */
  dma_reset(DMA_CHANNEL_ADC_CONVERT);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = ADC_ORDINARY_CH_LEN;
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
  dma_init_struct.memory_base_addr = (uint32_t)adc_in_tab;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t) & (ADC_NORMAL_CONVERTER->odt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(DMA_CHANNEL_ADC_CONVERT, &dma_init_struct);

  /* Configure ADC base parameters. */
  adc_base_default_para_init(&adc_base_struct);
  adc_base_struct.sequence_mode = TRUE;
  adc_base_struct.repeat_mode = FALSE;
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
  adc_base_struct.ordinary_channel_length = ADC_ORDINARY_CH_LEN;
  adc_base_config(ADC_NORMAL_CONVERTER, &adc_base_struct);

  /* Configure the ordinary conversion channels. */
  adc_ordinary_channel_set(ADC_NORMAL_CONVERTER, VOLT_BUS_ADC_CH, 1, ADC_SAMPLETIME_1_5);
  adc_ordinary_channel_set(ADC_NORMAL_CONVERTER, MOS_TEMP_ADC_CH, 2, ADC_SAMPLETIME_1_5);
  adc_ordinary_channel_set(ADC_NORMAL_CONVERTER, POTENTIO_ADC_CH, 3, ADC_SAMPLETIME_1_5);
  adc_ordinary_conversion_trigger_set(ADC_NORMAL_CONVERTER, ADC_NORMAL_TRIG_SOURCE, ADC_NORMAL_TRIG_EDGE);
  adc_dma_mode_enable(ADC_NORMAL_CONVERTER, TRUE);

  /* Enable common DMA request repeat (DMA requests will be generated for each conversion in the sequence). */
  adc_dma_request_repeat_enable(ADC_NORMAL_CONVERTER, TRUE);

  /* ADC enable and calibration procedure. */
  if(ADC_NORMAL_CONVERTER->ctrl2_bit.adcen != TRUE)
  {
    adc_enable(ADC_NORMAL_CONVERTER, TRUE);
    adc_calibration_init(ADC_NORMAL_CONVERTER);

    while(adc_calibration_init_status_get(ADC_NORMAL_CONVERTER));

    adc_calibration_start(ADC_NORMAL_CONVERTER);

    while(adc_calibration_status_get(ADC_NORMAL_CONVERTER));
  }

  /* DMAMUX configuration and enable. */
  dma_flexible_config(DMA1, ADC_ORDINARY_DMA_FLEX_CH, ADC_ORDINARY_DMA_FLEX);
  dma_channel_enable(DMA_CHANNEL_ADC_CONVERT, TRUE);
}

/**
 * @brief Configures the Analog-to-Digital Converter (ADC) for preemptive conversions.
 * @param None
 * @return None
 */
void adc_preempt_config(void)
{
  gpio_init_type gpio_init_struct = {0};
  adc_base_config_type adc_base_struct;
  adc_common_config_type adc_common_struct;

  /* Disable ADC before configuration. */
  adc_enable(ADC_INSTANT_CONVERTER, FALSE);

  /* Configure common ADC settings: independent mode, clock division, disable common DMA. */
  adc_common_default_para_init(&adc_common_struct);
  /* Configure ADC combine mode to independent mode (not synchronized with other ADCs). */
  adc_common_struct.combine_mode = ADC_INDEPENDENT_MODE;
  /* Configure ADC clock division from HCLK.
   * Example: HCLK Max. CLK = 180M Hz, ADC_CLK = 180/6 = 30M Hz. */
  adc_common_struct.div = ADC_CONVERTER_CRM_CLK_DIV;
  /* config common dma mode,it's not useful in independent mode */
  adc_common_struct.common_dma_mode = ADC_COMMON_DMAMODE_DISABLE;
  adc_common_struct.common_dma_request_repeat_state = FALSE;
  /* Disable internal temp/VREF sensor. */
  adc_common_struct.tempervintrv_state = FALSE;
  adc_common_config(&adc_common_struct);

  /* Enable clocks for ADC module, associated GPIOs (current, BEMF, comparator), and DMA. */
  crm_periph_clock_enable(ADC_INSTANT_CRM_CLK, TRUE);
  crm_periph_clock_enable(CURR_BUS_ADC_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(BEMF_A_ADC_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(BEMF_B_ADC_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(BEMF_C_ADC_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(DMA_ADC_CRM_CLK, TRUE);

  /* Configure GPIOs for analog input: current, BEMF phases, and comparator negative input. */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

  gpio_init_struct.gpio_pins = CURR_BUS_ADC_GPIO_PIN;
  gpio_init(CURR_BUS_ADC_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = BEMF_A_ADC_GPIO_PIN;
  gpio_init(BEMF_A_ADC_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = BEMF_B_ADC_GPIO_PIN;
  gpio_init(BEMF_B_ADC_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = BEMF_C_ADC_GPIO_PIN;
  gpio_init(BEMF_C_ADC_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = COMP_N_GPIO_PIN;
  gpio_init(COMP_N_PORT, &gpio_init_struct);

  adc_base_default_para_init(&adc_base_struct);
#if defined (BLDC_SENSORLESS_COMP)
  adc_base_struct.sequence_mode = FALSE;
#else
  adc_base_struct.sequence_mode = TRUE;
#endif
  adc_base_struct.repeat_mode = FALSE;
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
  adc_base_struct.ordinary_channel_length = ADC_ORDINARY_CH_LEN;
  adc_base_config(ADC_INSTANT_CONVERTER, &adc_base_struct);
  adc_resolution_set(ADC_INSTANT_CONVERTER, ADC_RESOLUTION_12B);
#if defined (BLDC_SENSORLESS_COMP)
  adc_preempt_channel_length_set(ADC_INSTANT_CONVERTER, 3);
  adc_preempt_channel_set(ADC_INSTANT_CONVERTER, CURR_BUS_ADC_CH, 1, ADC_SAMPLETIME_1_5);
  adc_preempt_channel_set(ADC_INSTANT_CONVERTER, CURR_BUS_ADC_CH, 2, ADC_SAMPLETIME_13_5);
  adc_preempt_channel_set(ADC_INSTANT_CONVERTER, CURR_BUS_ADC_CH, 3, ADC_SAMPLETIME_1_5);

  adc_preempt_part_mode_enable(ADC_INSTANT_CONVERTER, FALSE);
#else
  adc_preempt_channel_length_set(ADC_INSTANT_CONVERTER, ADC_PREEMPT_CH_LEN);
  adc_preempt_channel_set(ADC_INSTANT_CONVERTER, CURR_BUS_ADC_CH, 1, ADC_SAMPLETIME_7_5);
  adc_preempt_channel_set(ADC_INSTANT_CONVERTER, CURR_BUS_ADC_CH, 2, ADC_SAMPLETIME_7_5);

  adc_preempt_part_mode_enable(ADC_INSTANT_CONVERTER, FALSE);
#endif
  /* Set preemptive conversion trigger source and edge. */
  adc_preempt_conversion_trigger_set(ADC_INSTANT_CONVERTER, ADC_INSTANT_TRIG_SOURCE, ADC_INSTANT_TRIG_EDGE);

  /* adc voltage monitoring for over current dectector  */
  adc_voltage_monitor_enable(ADC_INSTANT_CONVERTER, ADC_VMONITOR_ALL_PREEMPT);
  adc_voltage_monitor_threshold_value_set(ADC_INSTANT_CONVERTER, OVERCURRENT_THRESHOLD_d, UNDERCURRENT_THRESHOLD_d);

#if !(defined EMF_CONTINOUS_SAMPLE || defined HALL_SENSORS || defined BLDC_SENSORLESS_COMP)
  dma_init_type dma_init_struct;
  dma_reset(DMA_CHANNEL_EMF_TRIG);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = 3;
  dma_init_struct.direction = DMA_DIR_MEMORY_TO_PERIPHERAL;
  dma_init_struct.memory_base_addr = (uint32_t)&adc_sample.adc_sample_point;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t) & (ADC_TIMER->c4dt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(DMA_CHANNEL_EMF_TRIG, &dma_init_struct);

  dma_flexible_config(DMA1, ADC_PREEMPT_DMA_FLEX_CH, ADC_PREEMPT_DMA_FLEX);
  dma_channel_enable(DMA_CHANNEL_EMF_TRIG, FALSE);
#endif

  /* ADC enable and calibration procedure. */
  if(ADC_INSTANT_CONVERTER->ctrl2_bit.adcen != TRUE)
  {
    adc_enable(ADC_INSTANT_CONVERTER, TRUE);
    adc_calibration_init(ADC_INSTANT_CONVERTER);

    while(adc_calibration_init_status_get(ADC_INSTANT_CONVERTER));

    adc_calibration_start(ADC_INSTANT_CONVERTER);

    while(adc_calibration_status_get(ADC_INSTANT_CONVERTER));
  }
}

/**
 * @brief Configures the comparator for overcurrent protection (OCP).
 * @param None
 * @return None
 */
void ocp_cmp_config(void)
{
  gpio_init_type gpio_init_struct = {0};
  cmp_init_type cmp_init_struct;

  /* gpiob peripheral clock enable */
  crm_periph_clock_enable(BUS_OUT_CRM_CLK, TRUE);

  /* configure pb0: pb0 is used as cmp1 non inveting input */
  gpio_init_struct.gpio_pins = BUS_OUT_PIN;
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(BUS_OUT_PORT, &gpio_init_struct);

  /* configure dac1: dac1 is used as cmp1 inveting input */
  crm_periph_clock_enable(CRM_DAC_PERIPH_CLOCK, TRUE);
  dac_1_data_set(DAC_OCP_REF);
  dac_reference_voltage_select(DAC1_SELECT, DAC_VDDA);
  dac_enable(DAC1_SELECT, TRUE);

  /* cmp peripheral clock enable */
  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);

  /* cmp1 init: pb0 is used cmp1 inverting input */
  cmp_default_para_init(&cmp_init_struct);
  cmp_init_struct.cmp_non_inverting = COMP_BUS_CH;
  cmp_init_struct.cmp_inverting = CMP_INVERTING_INM2;         //INM2:DAC1_OUT  INM3:DAC2_OUT
  cmp_init_struct.cmp_polarity = CMP_POL_NON_INVERTING;
  cmp_init_struct.cmp_speed = CMP_SPEED_FAST;
  cmp_init_struct.cmp_hysteresis = CMP_HYSTERESIS_LOW;
  cmp_init(OCP_COMP, &cmp_init_struct);

  cmp_filter_config(OCP_COMP, 5, 0, TRUE);

  /* enable cmp1 */
  cmp_enable(OCP_COMP, TRUE);
}
/**
 * @brief Configures Comparator 2 (BEMF_COMP) for Back-EMF (BEMF) detection.
 * @param None
 * @return None
 */
void cmp2_config(void)
{
  cmp_init_type cmp_init_struct;

  gpio_init_type gpio_init_struct = {0};

  /* Enable peripheral clocks for GPIO ports connected to comparator inputs. */
  crm_periph_clock_enable(COMP_A_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(COMP_B_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(COMP_C_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(COMP_N_GPIO_CRM_CLK, TRUE);

  /* Configure GPIO pins for analog input for BEMF phases and virtual neutral point. */
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;

  gpio_init_struct.gpio_pins = COMP_A_GPIO_PIN;    /* PA4(BEMF_U) */
  gpio_init(COMP_A_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = COMP_B_GPIO_PIN;    /* PA5(BEMF_V) */
  gpio_init(COMP_B_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = COMP_C_GPIO_PIN;    /* PA6(BEMF_W) */
  gpio_init(COMP_C_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = COMP_N_GPIO_PIN;    /* PA7(virtual neutral point)*/
  gpio_init(COMP_N_PORT, &gpio_init_struct);

  /* Initialize Comparator 2 (BEMF_COMP) for BEMF detection. */
  cmp_default_para_init(&cmp_init_struct);
  cmp_init_struct.cmp_inverting = CMP_INVERTING_INM4;
  cmp_init_struct.cmp_non_inverting = CMP_NON_INVERTING_INP0;
  cmp_init_struct.cmp_polarity = CMP_POL_NON_INVERTING;
  cmp_init_struct.cmp_speed = CMP_SPEED_FAST;
  cmp_init_struct.cmp_hysteresis = CMP_HYSTERESIS_HIGH;
  cmp_init(BEMF_COMP, &cmp_init_struct);

  /* Configure and enable a digital filter for the comparator output. */
  cmp_filter_config(BEMF_COMP, 0x3f, 0x3f, TRUE);

  /* Enable the BEMF comparator. */
  cmp_enable(BEMF_COMP, TRUE);
  cmp_blanking_config(BEMF_COMP, CMP_BLANKING_SOURCE);
}

/**
 * @brief GPIO output configuration(debug-only).
 * @param None
 * @return None
 */
 void gpio_output_init(void)
{
  gpio_init_type gpio_init_struct = {0};

  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_DOWN;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;

  crm_periph_clock_enable(TMR_ADC_TRIG_GPIO_CRM_CLK, TRUE);
  gpio_init_struct.gpio_pins = TMR_ADC_TRIG_GPIO_PIN;    /* T1C4 for ADC*/
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;

  gpio_init(TMR_ADC_TRIG_PORT, &gpio_init_struct);
  gpio_pin_mux_config(TMR_ADC_TRIG_PORT, TMR_ADC_TRIG_GPIO_PIN_SOURCE, TMR_ADC_TRIG_IOMUX);
  debug_apb2_periph_mode_set(DEBUG_TMR1_PAUSE, TRUE);
  
  gpio_default_para_init(&gpio_init_struct);

  gpio_init_struct.gpio_mode             = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_out_type         = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull             = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength   = GPIO_DRIVE_STRENGTH_STRONGER;

  crm_periph_clock_enable(COMP_OUT_GPIO_CRM_CLK, TRUE);
  gpio_init_struct.gpio_pins = COMP_OUT_GPIO_PIN;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init(COMP_OUT_PORT, &gpio_init_struct);
  gpio_pin_mux_config(COMP_OUT_PORT, COMP_OUT_GPIO_PIN_SOURCE, COMP_OUT_IOMUX);

  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_15;
  gpio_init(GPIOA, &gpio_init_struct);

  /* PB4 configuration using macros */
  crm_periph_clock_enable(BLANK_TRIGGER_GPIO_CRM_CLK, TRUE);
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_pins = BLANK_TRIGGER_GPIO_PIN;
  gpio_init(BLANK_TRIGGER_PORT, &gpio_init_struct);
	gpio_pin_mux_config(BLANK_TRIGGER_PORT, BLANK_TRIGGER_GPIO_PIN_SOURCE, BLANK_TRIGGER_IOMUX);
}

/**
 * @brief Initializes the internal Operational Amplifier (OPA) for motor bus current sensing.
 * @param None
 * @return None
 */
void opa_init(void)
{
  gpio_init_type gpio_init_struct;

  /* Enable peripheral clocks for System Configuration Controller and OPA input pins. */
  crm_periph_clock_enable(CRM_SCFG_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(BUS_INP_CRM_CLK, TRUE);
  crm_periph_clock_enable(BUS_INM_CRM_CLK, TRUE);
  /* Initialize GPIO structure with default parameters. */
  gpio_default_para_init(&gpio_init_struct);

  /* Configure OPA input pins as analog mode. */
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_pins = BUS_INP_PIN;
  gpio_init(BUS_INP_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = BUS_INM_PIN;
  gpio_init(BUS_INM_PORT, &gpio_init_struct);

  /* Connect OPA output to MCU pins */
  op_internal_output_select(BUS_OPA, OP_INTERNAL_OUTPUT_GPIO);

  op_set_highspeed_mode_enable(BUS_OPA, FALSE);
  /* Calibrate the OPA for improved measurement accuracy. */
  opa_calibration(BUS_OPA);

  /* Initialize OPA in standalone functional mode, setting its specific inputs. */
  op_set_mode(BUS_OPA, OP_FUNCTIONAL_MODE);
  op_set_functional_mode(BUS_OPA, OP_STANDALONE_MODE);
  op_set_input_non_inverting(BUS_OPA, OP_NON_INVERTING_INPUT_INP0);
  op_set_input_inverting(BUS_OPA, OP_INVERTING_INPUT_INM0);
  op_enable(BUS_OPA, TRUE);

  /* Clear any pending error flags related to the motor control system. */
  error_code = MC_NO_ERROR;
}
/**
  * @brief  offset calibration of opa
  * @param  number of opa
  * @retval none
  */
void opa_calibration(op_type* op_num)
{
  int16_t cal_index;

  /* initial op1 in calibration mode */
  op_set_mode(op_num, OP_CALIBRATION_MODE);
  op_set_trimming_mode(op_num, OP_TRIMMING_USER);
  op_calibration_select(op_num, OP_TRIMMING_PMOS);
  op_enable(op_num, TRUE);

  mc_delay_ms(1);

  /* calibration pmos */
  for(cal_index = 0; cal_index <= 0x1F; cal_index++)
  {
    op_set_triming_value(op_num, OP_TRIMMING_PMOS, cal_index);
    mc_delay_ms(1);

    /* calibration is successful when calout switches from 1 to 0 */
    if(op_calout_status_get(op_num) == RESET)
    {
      break;
    }
  }

  op_calibration_select(op_num, OP_TRIMMING_NMOS);
  mc_delay_ms(1);

  /* calibration nmos */
  for(cal_index = 0; cal_index <= 0x1F; cal_index++)
  {
    op_set_triming_value(op_num, OP_TRIMMING_NMOS, cal_index);
    mc_delay_ms(1);

    /* calibration  is successful when calout switches from 1 to 0 */
    if(op_calout_status_get(op_num) == RESET)
    {
      break;
    }
  }
}
/**
  * @brief  initialize uart
  * @param
  * @retval none
  */
void uart_init(usart_config_type *usart_config)
{
  gpio_init_type gpio_init_struct;

  /* enable the uart and gpio clock */
  crm_periph_clock_enable(usart_config->usart_clock, TRUE);
  crm_periph_clock_enable(usart_config->usart_tx_gpio_clock, TRUE);
  crm_periph_clock_enable(usart_config->usart_rx_gpio_clock, TRUE);

  gpio_default_para_init(&gpio_init_struct);

  /* configure the uart tx pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = usart_config->usart_tx_gpio_pin;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(usart_config->usart_tx_gpio_port, &gpio_init_struct);

  /* configure the uart rx pin */
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_OPEN_DRAIN;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = usart_config->usart_rx_gpio_pin;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init(usart_config->usart_rx_gpio_port, &gpio_init_struct);

  /* remap usart1 tx and rx pins */
  gpio_pin_mux_config(usart_config->usart_tx_gpio_port, usart_config->gpio_tx_pin_source, usart_config->gpio_tx_pin_mux);
  gpio_pin_mux_config(usart_config->usart_rx_gpio_port, usart_config->gpio_rx_pin_source, usart_config->gpio_rx_pin_mux);

  /* configure uart param */
  usart_init(usart_config->usart_x, usart_config->baudrate, USART_DATA_8BITS, USART_STOP_1_BIT);
  usart_transmitter_enable(usart_config->usart_x, TRUE);
  usart_receiver_enable(usart_config->usart_x, TRUE);
  usart_dma_transmitter_enable(usart_config->usart_x, TRUE);
  usart_dma_receiver_enable(usart_config->usart_x, TRUE);

  usart_interrupt_enable(usart_config->usart_x, USART_IDLE_INT, TRUE);

  usart_enable(usart_config->usart_x, TRUE);
}

/**
  * @brief  Led gpio configuration
  * @param  none
  * @retval none
  */
void led_config(void)
{
  gpio_init_type gpio_init_struct;

  /* enable the led clock */
  crm_periph_clock_enable(LED_R_GPIO_CRM_CLK, TRUE);
  crm_periph_clock_enable(LED_G_GPIO_CRM_CLK, TRUE);

  /* set default parameter */
  gpio_default_para_init(&gpio_init_struct);

  /* configure the led gpio */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  // gpio_init_struct.gpio_pins = ERROR_LED_GPIO_PIN;
  // gpio_init(ERROR_LED_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = LED_R_GPIO_PIN;
  gpio_init(LED_R_PORT, &gpio_init_struct);

  gpio_init_struct.gpio_pins = LED_G_GPIO_PIN;
  gpio_init(LED_G_PORT, &gpio_init_struct);
}

/**
  * @brief  Pull up gpio configuration
  * @param  none
  * @retval none
  */
void pull_up_config(void)
{
  gpio_init_type gpio_init_struct;

  /* enable the led clock */
  crm_periph_clock_enable(EMF_PULL_UP_GPIO_CRM_CLK, TRUE);

  /* set default parameter */
  gpio_default_para_init(&gpio_init_struct);

  /* configure the led gpio */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;

  gpio_init_struct.gpio_pins = EMF_PULL_UP_GPIO_PIN;
  gpio_init(EMF_PULL_UP_PORT, &gpio_init_struct);

#if defined (BLDC_SENSORLESS_ADC) && defined (EMF_PULL_UP)
  /* PULL UP */
  EMF_PULL_UP_PORT->clr = EMF_PULL_UP_GPIO_PIN;
#else
  /* PULL DOWN */
  EMF_PULL_UP_PORT->scr = EMF_PULL_UP_GPIO_PIN;
#endif
}

/**
  * @brief  turns selected led on.
  * @param  led_gpio_port : to select the led gpio peripheral.
  * @param  led_gpio_pin: led gpio pin number
  * @retval none
  */
void led_on(gpio_type *led_gpio_port, uint16_t led_gpio_pin)
{
  led_gpio_port->clr = led_gpio_pin;
}

/**
  * @brief  turns selected led off.
  * @param  led_gpio_port : to select the led gpio peripheral.
  * @param  led_gpio_pin: led gpio pin number
  * @retval none
  */
void led_off(gpio_type *led_gpio_port, uint16_t led_gpio_pin)
{
  led_gpio_port->scr = led_gpio_pin;
}

/**
  * @brief  turns selected led tooggle.
  * @param  led_gpio_port : to select the led gpio peripheral.
  * @param  led_gpio_pin: led gpio pin number
  * @retval none
  */
void led_toggle(gpio_type *led_gpio_port, uint16_t led_gpio_pin)
{
  led_gpio_port->odt ^= led_gpio_pin;
}

/**
  * @brief  initialization of led.
  * @param  none
  * @retval none
  */
void led_init(void)
{
  led_config();
  led_off(LED_R_PORT, LED_R_GPIO_PIN);
  led_off(LED_G_PORT, LED_G_GPIO_PIN);
}

/**
  * @brief  turns selected led blink.
  * @param  none
  * @retval none
  */
void led_blink(void)
{
  uint8_t count;

  for(count = 0; count < 3; count++)
  {
    led_on(LED_R_PORT, LED_R_GPIO_PIN);
    led_on(LED_G_PORT, LED_G_GPIO_PIN);
    mc_delay_ms(500);
    led_off(LED_R_PORT, LED_R_GPIO_PIN);
    led_off(LED_G_PORT, LED_G_GPIO_PIN);
    mc_delay_ms(500);
  }
}

/**
  * @brief  configure current offset timer
  * @param  tmr_x: select the tmr peripheral.
  * @param  adc_sample Pointer to a structure containing ADC and Timer information relevant for sampling.
  * @retval none
  */
void bldc_angle_init_config(angle_init_type *angle_init, adc_sample_type *adc_sample)
{
  tmr_output_config_type tmr_output_struct;

  /* disable pwm timer */
  tmr_counter_enable(angle_init->ADVTMRx, FALSE);
  /* set pwm timer single pulse mode */
  tmr_one_cycle_mode_enable(angle_init->ADVTMRx, TRUE);
  /* set pwm timer initial peried */
  tmr_base_init(angle_init->ADVTMRx, angle_init->tmr_period, 0);
  /* disable pwm timer buffer */
  tmr_channel_buffer_enable(angle_init->ADVTMRx, FALSE);
  /* disable pwm timer channel buffer */
  tmr_output_channel_buffer_enable(angle_init->ADVTMRx, TMR_SELECT_CHANNEL_1, FALSE);
  tmr_output_channel_buffer_enable(angle_init->ADVTMRx, TMR_SELECT_CHANNEL_2, FALSE);
  tmr_output_channel_buffer_enable(angle_init->ADVTMRx, TMR_SELECT_CHANNEL_3, FALSE);
  tmr_output_channel_buffer_enable(angle_init->ADVTMRx, TMR_SELECT_CHANNEL_4, FALSE);

  /* disable ordinary DMA */
  dma_channel_enable(DMA_CHANNEL_ADC_CONVERT, FALSE);
  /* disable preempt DMA */
  dma_channel_enable(DMA_CHANNEL_EMF_TRIG, FALSE);
  /* ADC disable */
  adc_enable(ADC_INSTANT_CONVERTER, FALSE);
  adc_flag_clear(ADC_INSTANT_CONVERTER, ADC_PCCE_FLAG);
  /* ADC preempt channel setting */
  adc_preempt_channel_length_set(ADC_INSTANT_CONVERTER, 2);
  adc_preempt_channel_set(ADC_INSTANT_CONVERTER, CURR_BUS_ADC_CH, 1, ADC_SAMPLETIME_1_5);
  adc_preempt_channel_set(ADC_INSTANT_CONVERTER, CURR_BUS_ADC_CH, 2, ADC_SAMPLETIME_1_5);

  /* adc prempt trigger source */
  adc_preempt_conversion_trigger_set(ADC_INSTANT_CONVERTER, ADC_INSTANT_TRIG_SOURCE, ADC_INSTANT_TRIG_EDGE);
  adc_preempt_part_mode_enable(ADC_INSTANT_CONVERTER, FALSE);

  /* reset the counter of adc ordinary DMA channel */
  dma_data_number_set(DMA_CHANNEL_ADC_CONVERT, ADC_ORDINARY_CH_LEN);

  /* TMR1 setting */
  tmr_output_default_para_init(&tmr_output_struct);
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = FALSE;
  tmr_output_struct.occ_output_state = FALSE;
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_LOW;
  tmr_output_struct.occ_idle_state = FALSE;
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_B;
  tmr_output_channel_config(angle_init->ADC_TMRx, ADC_TIMER_SELECT_CHANNEL, &tmr_output_struct);
  /* clear timer counter */
  tmr_counter_value_set(angle_init->ADVTMRx, 0);
  /* disable pwm timer overflow interrupt */
  tmr_interrupt_enable(angle_init->ADVTMRx, TMR_OVF_INT, FALSE);
  /* enable DMA */
  dma_channel_enable(DMA_CHANNEL_ADC_CONVERT, TRUE);
  /* enable timer */
  tmr_counter_enable(angle_init->ADVTMRx, TRUE);
  /* enable ADC */
  adc_enable(ADC_INSTANT_CONVERTER, TRUE);

  angle_init->step_count = 1;
}
/**
 * @brief Configures the ADC and timers for Back-EMF (BEMF) detection in sensorless BLDC control.
 * @param adc_sample Pointer to a structure containing ADC and Timer information relevant for sampling.
 * @return None
 */
void bldc_sensorless_detectEMF_config(adc_sample_type *adc_sample)
{
  /* Clear interrupt flags for PWM timer overflow and ADC timer. */
  tmr_flag_clear(adc_sample->ADVTMRx, TMR_OVF_FLAG);
  tmr_flag_clear(adc_sample->ADC_TMRx, TMR_C4_FLAG);

  /* Disable PWM timer and ADC timer. */
  tmr_counter_enable(adc_sample->ADVTMRx, FALSE);
  tmr_counter_enable(adc_sample->ADC_TMRx, FALSE);

  /* Reset counter values for PWM, ADC, blank trigger, and blanking timers. */
  tmr_counter_value_set(adc_sample->ADVTMRx, 0);
  tmr_counter_value_set(adc_sample->ADC_TMRx, 0);
  tmr_counter_value_set(blank_trigger.TMRx, 0);
  tmr_counter_value_set(blank.TMRx, 0);

  /* Disable ADC and clear its preemptive conversion complete flag. */
  adc_enable(ADC_INSTANT_CONVERTER, FALSE);
  adc_flag_clear(ADC_INSTANT_CONVERTER, ADC_PCCE_FLAG);

  /* Configure ADC timer for DMA requests by channel. */
  tmr_channel_dma_select(adc_sample->ADC_TMRx, TMR_DMA_REQUEST_BY_CHANNEL);
  tmr_dma_request_enable(adc_sample->ADC_TMRx, TMR_C4_DMA_REQUEST, TRUE);

  /* Disable ordinary and preemptive ADC DMA channels. */
  dma_channel_enable(DMA_CHANNEL_ADC_CONVERT, FALSE);
  dma_channel_enable(DMA_CHANNEL_EMF_TRIG, FALSE);

#ifdef BLDC_SENSORLESS_COMP
  /* Set DMA blank trigger channel length. */
  dma_data_number_set(DMA_CHANNEL_BLANK_TRIGGER, 2);

  /* Configure ADC preemptive channel length. */
  adc_preempt_channel_length_set(ADC_INSTANT_CONVERTER, 3);
  adc_preempt_channel_set(ADC_INSTANT_CONVERTER, CURR_BUS_ADC_CH, 1, ADC_SAMPLETIME_1_5);
  adc_preempt_channel_set(ADC_INSTANT_CONVERTER, CURR_BUS_ADC_CH, 2, ADC_SAMPLETIME_28_5);
  adc_preempt_part_mode_enable(ADC_INSTANT_CONVERTER, FALSE);
#else
  /* Set DMA preemptive channel length for EMF trigger. */
  dma_data_number_set(DMA_CHANNEL_EMF_TRIG, 6);
  /* Set DMA ordinary channel length. */
  dma_data_number_set(DMA_CHANNEL_ADC_CONVERT, ADC_ORDINARY_CH_LEN);

  /* Configure ADC preemptive channel length */
  adc_preempt_channel_length_set(ADC_INSTANT_CONVERTER, 3);
  adc_preempt_channel_set(ADC_INSTANT_CONVERTER, ADC_CHANNEL_17, 1, ADC_SAMPLETIME_1_5);
  adc_preempt_channel_set(ADC_INSTANT_CONVERTER, CURR_BUS_ADC_CH, 2, ADC_SAMPLETIME_7_5);
  adc_preempt_part_mode_enable(ADC_INSTANT_CONVERTER, TRUE);
#endif

  switch(firmware_id)
  {
  case BLDC_SENSOR_LESS_ADC:
    adc_preempt_channel_set(ADC_INSTANT_CONVERTER, BEMF_A_ADC_CH, 3,  ADC_EMF_SAMPLETIME);
    break;

  case BLDC_SENSOR_LESS_COMP:
    adc_preempt_channel_set(ADC_INSTANT_CONVERTER, CURR_BUS_ADC_CH, 3, ADC_EMF_SAMPLETIME);
    break;

  default:
    adc_preempt_channel_set(ADC_INSTANT_CONVERTER, BEMF_A_ADC_CH, 3, ADC_EMF_SAMPLETIME);
    break;
  }

#if !defined BLDC_SENSORLESS_COMP
  /* enable preempt DMA */
  dma_channel_enable(DMA_CHANNEL_EMF_TRIG, TRUE);
#endif

  /* Enable ordinary ADC DMA. */
  dma_channel_enable(DMA_CHANNEL_ADC_CONVERT, TRUE);

  /* Enable blank trigger DMA. */
  dma_channel_enable(DMA_CHANNEL_BLANK_TRIGGER, TRUE);

  /* Enable ADC */
  adc_enable(ADC_INSTANT_CONVERTER, TRUE);
}

/**
  * @brief  Get Vref to calibrate voltage
  * @param  none
  * @retval none
  */
void get_int_vref_cal_ratio(void)
{
  int32_t vref_adc;
  adc_common_config_type adc_common_struct;

  /* adc clock configuration */
  crm_periph_clock_enable(ADC_NORMAL_CRM_CLK, TRUE);

  /* disable ADC */
  adc_enable(ADC_NORMAL_CONVERTER, FALSE);
  /* disable ordinary DMA */
  dma_channel_enable(DMA_CHANNEL_ADC_CONVERT, FALSE);
  /* DMA ordinary channel length setting */
  dma_data_number_set(DMA_CHANNEL_ADC_CONVERT, ADC_ORDINARY_CH_LEN);

  adc_common_default_para_init(&adc_common_struct);
  /* config division,adcclk is division by hclk */
  adc_common_struct.div = ADC_CONVERTER_CRM_CLK_DIV;        /* HCLK Max. CLK = 180M Hz, ADC_CLK = 180/6 = 30M Hz */

  /* config combine mode */
  adc_common_struct.combine_mode = ADC_INDEPENDENT_MODE;

  /* config inner temperature sensor and vintrv */
  adc_common_struct.tempervintrv_state = TRUE;
  adc_common_config(&adc_common_struct);

  /* preempt channel setting */
  adc_preempt_channel_length_set(ADC_NORMAL_CONVERTER, 2);
  adc_preempt_channel_set(ADC_NORMAL_CONVERTER, ADC_CHANNEL_17, 1, ADC_SAMPLETIME_239_5);
  adc_preempt_channel_set(ADC_NORMAL_CONVERTER, ADC_CHANNEL_17, 2, ADC_SAMPLETIME_239_5);

  /* change adc prempt trigger source */
  adc_preempt_conversion_trigger_set(ADC_NORMAL_CONVERTER, ADC_INSTANT_TRIG_SOURCE, ADC_PREEMPT_TRIG_EDGE_NONE);
  /* disable part mode */
  adc_preempt_part_mode_enable(ADC_NORMAL_CONVERTER, FALSE);

  if(ADC_NORMAL_CONVERTER->ctrl2_bit.adcen != TRUE)
  {
    adc_enable(ADC_NORMAL_CONVERTER, TRUE);

    while(adc_flag_get(ADC_NORMAL_CONVERTER, ADC_RDY_FLAG) == RESET);

    adc_calibration_init(ADC_NORMAL_CONVERTER);

    while(adc_calibration_init_status_get(ADC_NORMAL_CONVERTER));

    adc_calibration_start(ADC_NORMAL_CONVERTER);

    while(adc_calibration_status_get(ADC_NORMAL_CONVERTER));
  }

  adc_flag_clear(ADC_NORMAL_CONVERTER, ADC_PCCE_FLAG);
  adc_preempt_software_trigger_enable(ADC_NORMAL_CONVERTER, TRUE);

  while(adc_flag_get(ADC_NORMAL_CONVERTER, ADC_PCCE_FLAG) == RESET);

  vref_adc = adc_preempt_conversion_data_get(ADC_NORMAL_CONVERTER, ADC_PREEMPT_CHANNEL_2);

  adc_flag_clear(ADC_NORMAL_CONVERTER, ADC_PCCE_FLAG);
  adc_preempt_software_trigger_enable(ADC_NORMAL_CONVERTER, TRUE);

  while(adc_flag_get(ADC_NORMAL_CONVERTER, ADC_PCCE_FLAG) == RESET);

  vref_adc += adc_preempt_conversion_data_get(ADC_NORMAL_CONVERTER, ADC_PREEMPT_CHANNEL_2);
  adc_flag_clear(ADC_NORMAL_CONVERTER, ADC_PCCE_FLAG);

  adc_enable(ADC_NORMAL_CONVERTER, FALSE);

  if(ADC_NORMAL_CONVERTER->ctrl2_bit.adcen != TRUE)
  {
    adc_enable(ADC_NORMAL_CONVERTER, TRUE);

    while(adc_flag_get(ADC_NORMAL_CONVERTER, ADC_RDY_FLAG) == RESET);

    adc_calibration_init(ADC_NORMAL_CONVERTER);

    while(adc_calibration_init_status_get(ADC_NORMAL_CONVERTER));

    adc_calibration_start(ADC_NORMAL_CONVERTER);

    while(adc_calibration_status_get(ADC_NORMAL_CONVERTER));
  }

  /* enable ordinary DMA */
  dma_channel_enable(DMA_CHANNEL_ADC_CONVERT, TRUE);

  vref_adc >>= 1;

  adc_sample.vref_cal_ratio = (int16_t) ((IDEAL_1V2_ADC_VALUE << 14) / vref_adc);
}

/**
  * @brief  initialization of adc and timer for motor parameter ID
  * @param  none
  * @retval none
  */
void motor_parameter_ID_config(void)
{
  tmr_output_config_type tmr_output_struct;

  /* disable pwm timer */
  tmr_counter_enable(PWM_ADVANCE_TIMER, FALSE);
  /* disable timer dma*/
  tmr_dma_request_enable(PWM_ADVANCE_TIMER, TMR_C4_DMA_REQUEST, FALSE);
  dma_channel_enable(DMA_CHANNEL_EMF_TRIG, FALSE);
  /* disable pwm timer period buffer */
  tmr_period_buffer_enable(PWM_ADVANCE_TIMER, FALSE);
  /* set single pulse mode */
  tmr_one_cycle_mode_enable(PWM_ADVANCE_TIMER, FALSE);
  /* set tmr peried */
  tmr_repetition_counter_set(PWM_ADVANCE_TIMER, 1);               /* the pwm cycle isr in underflow (high-side pwm on) */
  tmr_base_init(PWM_ADVANCE_TIMER, PWM_PERIOD_ID, 0);
  tmr_cnt_dir_set(PWM_ADVANCE_TIMER, TMR_COUNT_TWO_WAY_1);
  /* set dead time clock */
  tmr_clock_source_div_set(PWM_ADVANCE_TIMER, DEADTIME_CLK_SFT_BITS);
  /* disable tmr buffer */
  tmr_channel_buffer_enable(PWM_ADVANCE_TIMER, FALSE);

  tmr_output_channel_buffer_enable(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_1, FALSE);
  tmr_output_channel_buffer_enable(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_2, FALSE);
  tmr_output_channel_buffer_enable(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_3, FALSE);
  tmr_output_channel_buffer_enable(ADC_TIMER, ADC_TIMER_SELECT_CHANNEL, FALSE);

  /* set pwm compare value */
  tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_1, 0);
  tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_2, 0);
  tmr_channel_value_set(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_3, 0);
  tmr_channel_value_set(ADC_TIMER, ADC_TIMER_SELECT_CHANNEL, ADC_TRIG_POS_ID);

  /* output compare toggle mode configuration: channel1 */
  tmr_output_default_para_init(&tmr_output_struct);
  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_output_struct.oc_output_state = TRUE;
  tmr_output_struct.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.oc_idle_state = FALSE;
  tmr_output_struct.occ_output_state = TRUE;
#if defined GATE_DRIVER_LOW_SIDE_INVERT
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_LOW;
  tmr_output_struct.occ_idle_state = TRUE;
#else
  tmr_output_struct.occ_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_output_struct.occ_idle_state = FALSE;
#endif

  tmr_output_channel_config(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_1, &tmr_output_struct);
  tmr_output_channel_config(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_2, &tmr_output_struct);
  tmr_output_channel_config(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_3, &tmr_output_struct);

  tmr_output_struct.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_B;
  adc_preempt_channel_length_set(ADC_INSTANT_CONVERTER, 2);
  adc_preempt_channel_set(ADC_INSTANT_CONVERTER, CURR_BUS_ADC_CH, 1, ADC_SAMPLETIME_7_5);
  adc_preempt_channel_set(ADC_INSTANT_CONVERTER, CURR_BUS_ADC_CH, 2, ADC_SAMPLETIME_7_5);
  adc_preempt_part_mode_enable(ADC_INSTANT_CONVERTER, FALSE);
  tmr_output_struct.occ_output_state = FALSE;
  tmr_output_channel_config(ADC_TIMER, ADC_TIMER_SELECT_CHANNEL, &tmr_output_struct);

  /* clear tmr counter */
  tmr_counter_value_set(PWM_ADVANCE_TIMER, 0);

  tmr_channel_enable(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_3, FALSE);
  tmr_channel_enable(PWM_ADVANCE_TIMER, TMR_SELECT_CHANNEL_3C, FALSE);

  /* enable pwm timer */
  tmr_counter_enable(PWM_ADVANCE_TIMER, TRUE);
  /* pwm timer output enable */
  tmr_output_enable(PWM_ADVANCE_TIMER, TRUE);
}
