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
#include <stdbool.h>

// 函数声明
extern void send_telemetry_nrz(uint16_t telemetry_data);

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
			blank_trigger.sample_point[1] = pwm_comp_value-BLANK_TIME_OFFSET;//官方人员修改的
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
    //tmr_interrupt_enable(PWM_ADVANCE_TIMER, TMR_BRK_INT, FALSE);
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

volatile uint32_t system_time_ms;
/**
  * @brief  this function handles systick handler.
  * @param  none
  * @retval none
  */
void SysTick_Handler(void)
{
  static uint16_t led_blink_cnt = 0;
//		#if defined(DSHOT_SENDER) && !defined(DSHOT_RECEIVER)
//		dshot_send_packet(2047,0);
//    #endif
//		#if defined(DSHOT_RECEIVER) && defined(DSHOT_SENDER) && defined(DSHOT_BIDIRECTIONAL_INIT_SENDER)
//    {
// 
//      // 1ms启动一次dshot波形发送
//      dshot_sender.state = DSHOT_TX_STATE_SENDING_WAVEFORM;
//      dshot_send_packet(2047, 0);  // 发送DShot波形
//    }
//    #endif
 
    system_time_ms++;
 
 

  // if(ctrl_source == CTRL_SOURCE_EXTERNAL)
  // {
  //   external_input_handler();
  // }

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

#if  defined DSHOT600_INPUT || defined DSHOT600_BIDIRECTIONAL
 
extern volatile uint8_t dshot_bidir_state;
// GCR解码查找表（5位GCR -> 4位数据，255表示无效）
const uint8_t gcr_decode[32] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // 0-7
    0xFF, 0x09, 0x0A, 0x0B, 0xFF, 0x0D, 0x0E, 0x0F,  // 8-15
    0xFF, 0xFF, 0x02, 0x03, 0xFF, 0x05, 0x06, 0x07,  // 16-23
    0xFF, 0x00, 0x08, 0x01, 0xFF, 0x04, 0x0C, 0xFF   // 24-31
};
/**
 * @brief  计算双向DShot接收的CRC校验和（取反）
 * @param  data: 16油门值或命令（0-2047）
 * @param  request_telemetry: 是否请求遥测
 * @return 4位CRC校验和（已取反）
 * @note   双向DShot的CRC需要取反，与标准DShot不同
 */
static uint8_t dshot_bidir_calculate_crc(uint16_t data)
{
    uint8_t crc = 0;
    // 构建基本数据包：值左移1位，最低位为遥测请求位
    uint16_t packet = data >> 4;
    
    // 计算4位校验和
   crc = (packet ^ (packet >> 4) ^ (packet >> 8)) & 0x0F;
   crc = ~crc & 0x0F; // 飞控使用的是取反后的CRC
    
    return (uint8_t)crc;
}
 /**
  * @brief  计算DShot数据的CRC校验
  * @param  data: 16位DShot数据
  * @return 4位CRC校验值
  */
 uint8_t dshot_calculate_crc(uint16_t data)
 {
     uint8_t crc = 0;
    
     // 从原始数据中提取12位有效数据（去掉CRC位）
     uint16_t temp = data >> 4;
 		// 普通DSHOT CRC算法
 		// CRC计算公式: crc = (data ^ (data >> 4) ^ (data >> 8)) & 0x0F
 		// DShot协议使用简单的3级XOR校验
 		crc = temp ^ (temp >> 4) ^ (temp >> 8);
 		crc &= 0x0F; // 保留低4位作为CRC
     return crc;
 }
/**
 * @brief  计算双向DShot发送端的CRC校验和（取反）
 * @param  value: 11位油门值或命令（0-2047）
 * @param  request_telemetry: 是否请求遥测
 * @return 4位CRC校验和（已取反）
 * @note   双向DShot的CRC需要取反，与标准DShot不同
 */
static uint8_t dshot_bidir_calculate_crc_tx(uint16_t value, bool request_telemetry)
{
    // 构建基本数据包：值左移1位，最低位为遥测请求位
    uint16_t packet = (value << 1) | (request_telemetry ? 1 : 0);
    
    // 计算4位校验和
    unsigned csum = 0;
    unsigned csum_data = packet;
    for (int i = 0; i < 3; i++) {
        csum ^= (csum_data & 0x0F);  // 按半字节异或
        csum_data >>= 4;
    }
    
    // 双向DShot：CRC取反（关键区别！）
    csum = ~csum & 0x0F;
    
    return (uint8_t)csum;
}

/**
 * @brief  准备双向DShot数据包
 * @param  value: 油门值或命令（0-2047）
 * @param  request_telemetry: 是否请求遥测
 * @return 16位DShot数据包
 * @note   双向DShot的CRC需要取反，与标准DShot不同
 */
static uint16_t dshot_bidir_prepare_packet(uint16_t value, bool request_telemetry)
{
    uint16_t packet = 0;
    
    // 将11位值左移1位，留出遥测请求位
    packet = (value & 0x07FF) << 1;
    
    // 设置遥测请求位（位0）
    if (request_telemetry) {
        packet |= 0x01;
    }
    
    // 计算并添加4位校验和到数据包末尾
    uint8_t csum = dshot_bidir_calculate_crc_tx(value, request_telemetry);
    packet = (packet << 4) | csum;
    
    return packet;
}

/**
 * @brief  将双向DShot数据包编码为PWM脉冲（反相极性）
 * @param  packet: 16位DShot数据包
 * @param  buffer: 输出缓冲区
 * @return 实际编码长度
 * @note   双向DShot使用反相极性：空闲HIGH，1=LOW脉冲，0=HIGH脉冲
 */
static uint8_t dshot_bidir_encode_to_pwm(uint16_t packet, uint16_t *buffer)
{
    uint8_t index = 0;
    
    // 双向DShot：反相极性编码
    // 从最高位(位15)到最低位(位0)依次编码每个位
    for (int8_t i = 15; i >= 0; i--) {
        uint16_t bit = (packet >> i) & 0x01;
        
        // 反相极性：
        // - 比特1：较短的低电平脉冲（DSHOT_PULSE_0_COUNT）
        // - 比特0：较长的高电平脉冲（DSHOT_PULSE_1_COUNT）
        // 注意：这里使用PWM模式B或反转逻辑实现
        if (bit) {
            // 比特1：低电平脉冲（在PWM模式B下，设置较小的值产生低电平）
            buffer[index++] = DSHOT600_BIT_TICKS - DSHOT600_ONE_THRESHOLD;
        } else {
            // 比特0：高电平脉冲
            buffer[index++] = DSHOT600_BIT_TICKS - DSHOT_PULSE_0_COUNT;
        }
    }
    
    // 添加帧结束空闲周期（双向DShot空闲状态为HIGH）
    for (uint8_t i = 0; i < 2; i++) {
        buffer[index++] = DSHOT600_BIT_TICKS;  // 100%占空比（高电平）
    }
    
    return index;
}

/**
 * @brief  GCR解码函数（21位GCR -> 16位数据）
 * @param  gcr_data: 21位GCR编码数据
 * @return 16位解码后的数据
 * @note   电调回传的遥测数据使用GCR编码
 */
uint16_t dshot_gcr_decode(uint32_t gcr_data)
{
    uint16_t result = 0;
    uint8_t nibble;
    
    // GCR编码：每5位GCR对应4位数据
    // 21位GCR = 4个数据半字节(20位) + 1位保留
    for (int8_t i = 3; i >= 0; i--) {
        uint8_t gcr_nibble = (gcr_data >> (i * 5 + 1)) & 0x1F;  // 提取5位GCR
        nibble = gcr_decode[gcr_nibble];               // 查表解码
        
        if (nibble == 255) {
            // 无效的GCR编码
            return 0xFFFF;  // 返回错误标记
        }
        
        result |= (nibble << (i * 4));  // 组合4位数据
    }
    
    return result;
}

/**
 * @brief  GCR编码函数（16位数据 -> 21位GCR）
 * @param  data: 16位遥测数据
 * @return 21位GCR编码数据
 * @note   电调回传的遥测数据使用GCR编码
 */
static uint32_t dshot_gcr_encode(uint16_t data)
{
    uint32_t gcr_data = 0;
    
    // GCR编码：每4位数据对应5位GCR
    // 16位数据 = 4个数据半字节
    for (int8_t i = 3; i >= 0; i--) {
        uint8_t nibble = (data >> (i * 4)) & 0x0F;  // 提取4位数据
        uint8_t gcr_nibble = dshot_gcr_encode_lut[nibble];  // 查表编码
        gcr_data = (gcr_data << 5) | gcr_nibble;  // 组合5位GCR
    }
    
    // 确保第一bit是0，起始位在最高位(MSB，第20位)
    // 格式：0(起始位) + GCR(20位)
    // 注意：我们保持20位GCR数据，发送时从第20位开始，第20位发送0，然后是19-0位的GCR数据
    gcr_data = gcr_data & 0xFFFFF;  // 确保只有20位GCR数据
    
    return gcr_data;
}


/**
 * @brief  将机械 RPM 转换为电气周期（微秒）
 */
static uint32_t rpm_to_eperiod_us(uint16_t actual_rpm, uint16_t pole_pairs)
{
    uint32_t erpm = (uint32_t)actual_rpm * pole_pairs;
    if (erpm == 0) return 0xFFFF;   // 停转
    return 60000000UL / erpm;
}

/**
 * @brief  将电气周期编码为 12 位指数-尾数格式
 */
static uint16_t eperiod_to_12bit(uint32_t eperiod_us)
{
    if (eperiod_us == 0) return 0;
    uint8_t exp = 0;
    uint32_t val = eperiod_us;
    while (val >= 512 && exp < 7) {
        val >>= 1;
        exp++;
    }
    uint16_t mant = (uint16_t)val & 0x1FF;
    return ((uint16_t)exp << 9) | mant;
}

/**
 * @brief  计算 4 位 CRC（双向 DShot 取反）
 */
static uint8_t crc4_rev(uint16_t data_12bit)
{
    uint8_t crc = 0;
    uint16_t tmp = data_12bit;
    for (int i = 0; i < 3; i++) {
        crc ^= (tmp & 0x0F);
        tmp >>= 4;
    }
    return ~crc & 0x0F;
}

/**
 * @brief  构建 16 位遥测帧（替换原来的 dshot_calculate_telemetry_value）
 */
uint16_t dshot_build_telemetry_frame(uint16_t actual_rpm)
{
    uint16_t pole_pairs = POLE_PAIRS;
    uint32_t eperiod = rpm_to_eperiod_us(actual_rpm, pole_pairs);
    uint16_t data_12bit = eperiod_to_12bit(eperiod);
    uint8_t crc = crc4_rev(data_12bit);
    return (data_12bit << 4) | crc;
}

/**
 * @brief  计算双向DShot遥测数据值（兼容旧接口）
 * @param  actual_rpm: 实际转速（RPM）
 * @return 16位遥测数据值
 * @note   将实际转速转换为双向DShot遥测格式
 */
uint16_t dshot_calculate_telemetry_value(uint16_t actual_rpm)
{
    return dshot_build_telemetry_frame(actual_rpm);
}

static uint8_t dshot_checksum(uint16_t frame_value)
{
  uint8_t checksum = 0;
  uint8_t i;

  frame_value >>= 4;
  for (i = 0; i < 3; i++)
  {
    checksum ^= (uint8_t)(frame_value & 0x0F);
    frame_value >>= 4;
  }

  return (uint8_t)(checksum & 0x0F);
}

static uint16_t dshot_last_capture_value;
static uint16_t dshot_frame_accumulator;
static uint8_t dshot_frame_bit_count;
static uint8_t dshot_expect_falling_edge;
static uint8_t dshot_frame_synced;
static uint16_t dshot_no_signal_counter;
 
 void dshot_frame_decode_and_apply(uint16_t dshot_frame)
{
  uint16_t throttle_value;
  #ifdef DSHOT600_INPUT 
  if (dshot_calculate_crc(dshot_frame) == (uint8_t)(dshot_frame & 0x0F))
  #else
  if (dshot_bidir_calculate_crc(dshot_frame) == (uint8_t)(dshot_frame & 0x0F))
  #endif
  {
    //dshot_debug_last_frame = dshot_frame;
    throttle_value = (uint16_t)(dshot_frame >> 5);
    //dshot_debug_last_throttle = throttle_value;
    //dshot_debug_crc_ok_count++;
    dshot_no_signal_counter = 0;

    if (throttle_value > 0)
    {
      if (throttle_value > DSHOT_CMD_MAX)
      {
        throttle_value = DSHOT_CMD_MAX;
      }

      if (throttle_value <= 47) {
        speed_ramp.cmd_final = SPEED_RPM_MIN;
      } else {
        speed_ramp.cmd_final = SPEED_RPM_MIN + (((int32_t)(throttle_value - 47) * (MAX_SPEED_RPM - SPEED_RPM_MIN)) / (DSHOT_CMD_MAX - 47));
      }

      start_stop_btn_flag = SET;
    }
    else
    {
      speed_ramp.cmd_final = 0;
      start_stop_btn_flag = RESET;
    }
		#ifdef  DSHOT600_BIDIRECTIONAL
		   // 双向DShot：检查是否请求遥测
		if (dshot_bidir_state==0) {
				// 延迟 - 标准双向DShot要求电调在收到命令后等待30μs再回传遥测
				//dshot_delay_us(10);

				// 重置硬件
				dshot_bidir_reset_hardware();

				// 准备遥测数据：从实际转速计算
				// 这里替换为实际的转速获取函数，例如：get_motor_rpm()
				// 暂时使用模拟值2000 RPM
				// uint16_t actual_rpm = 2000;
				uint16_t telemetry_value = dshot_build_telemetry_frame(rotor_speed.filtered);
				dshot_bidir_send_telemetry_nrz(telemetry_value);
		}
		#endif
  }
  //else
  //{
  //  dshot_debug_crc_fail_count++;
  //}
}

static void dshot_capture_decode_range(uint16_t start_index, uint16_t end_index)
{
  uint16_t index;
  uint16_t capture_value;
  uint16_t edge_ticks;

  for (index = start_index; index < end_index; index++)
  {
    capture_value = dshot_dma_capture_buffer[index];
    edge_ticks = (uint16_t)(capture_value - dshot_last_capture_value);
    dshot_last_capture_value = capture_value;

    if (edge_ticks >= DSHOT600_FRAME_GAP_TICKS)
    {
      dshot_frame_accumulator = 0;
      dshot_frame_bit_count = 0;
      dshot_expect_falling_edge = TRUE;
      dshot_frame_synced = TRUE;
      continue;
    }

    if (dshot_frame_synced == FALSE)
    {
      continue;
    }

    if (dshot_expect_falling_edge != FALSE)
    {
      dshot_frame_accumulator <<= 1;
      if (edge_ticks >= DSHOT600_ONE_THRESHOLD)
      {
        dshot_frame_accumulator |= 1U;
      }
      dshot_frame_bit_count++;
      dshot_expect_falling_edge = FALSE;

      if (dshot_frame_bit_count >= DSHOT600_FRAME_BITS)
      {
        dshot_frame_decode_and_apply(dshot_frame_accumulator);
        dshot_frame_accumulator = 0;
        dshot_frame_bit_count = 0;
      }
    }
    else
    {
      dshot_expect_falling_edge = TRUE;
    }
  }
}

void DMA_DSHOT_INPUT_IRQHandler(void)
{
	// 处理发送模式的DMA中断
	if (dshot_bidir_state == 1) { // 发送中
		// 处理发送完成中断
		if (dma_flag_get(DMA_DSHOT_INPUT_FDT_FLAG) != RESET) {
			dma_flag_clear(DMA_DSHOT_INPUT_FDT_FLAG);  // 清除中断标志
			
			// 发送完成后切换回接收模式
			dshot_bidir_state = 0;  // 接收中
			dshot_input_timer_init();
		}
	}
	// 处理接收模式的DMA中断
	else {
		// 处理半传输完成中断
		// 当DMA缓冲区的前半部分填满时触发
		// DSHOT_DMA_CAPTURE_BUFFER_SIZE=128，因此前半部分是0-63个元素
		if (dma_flag_get(DMA_DSHOT_INPUT_HDT_FLAG) != RESET) {
			dma_flag_clear(DMA_DSHOT_INPUT_HDT_FLAG);  // 清除中断标志
			
			// 解码缓冲区前半部分的数据（索引0到63）
			// 使用/2是为了将128个元素的缓冲区分为两部分处理
			// 这样可以实现数据的流式处理，提高效率
			dshot_capture_decode_range(0, DSHOT_DMA_CAPTURE_BUFFER_SIZE / 2U);
		}
		
		// 处理全传输完成中断
		// 当DMA缓冲区的后半部分填满时触发
		// DSHOT_DMA_CAPTURE_BUFFER_SIZE=128，因此后半部分是64-127个元素
		if (dma_flag_get(DMA_DSHOT_INPUT_FDT_FLAG) != RESET) {
			dma_flag_clear(DMA_DSHOT_INPUT_FDT_FLAG);  // 清除中断标志
			
			// 解码缓冲区后半部分的数据（索引64到127）
			// 使用/2是为了将128个元素的缓冲区分为两部分处理
			// 这样可以实现数据的流式处理，提高效率
			dshot_capture_decode_range(DSHOT_DMA_CAPTURE_BUFFER_SIZE / 2U, DSHOT_DMA_CAPTURE_BUFFER_SIZE);
		}
	}
}

void PWM_DUTY_INPUT_IRQ(void)
{
  if (PWM_DUTY_INPUT_TIMER->ists_bit.ovfif && PWM_DUTY_INPUT_TIMER->iden_bit.ovfien)  // 当定时器溢出时，表示在预定的时间内未接收到完整的DSHOT帧，可能是信号丢失或干扰导致的
  {
    tmr_flag_clear(PWM_DUTY_INPUT_TIMER, TMR_OVF_FLAG);
    dshot_frame_accumulator = 0;
    dshot_frame_bit_count = 0;
    dshot_expect_falling_edge = FALSE;
    dshot_frame_synced = FALSE;
    dshot_no_signal_counter++;

    if(dshot_no_signal_counter >= DSHOT600_SIGNAL_LOSS_COUNT)
    {
      //speed_ramp.cmd_final = 0;
      start_stop_btn_flag = RESET;
      dshot_no_signal_counter = DSHOT600_SIGNAL_LOSS_COUNT;
    }
  }
}

 
#endif
