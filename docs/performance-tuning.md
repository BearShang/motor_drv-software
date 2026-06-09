---
title: 性能调优
---

# 性能调优

Tiny_ESC 的核心调参集中在 `firmware/at32m412_lv_motor_ev/at32m412/bldc_1shunt_sensorless/inc/motor_control_drive_param.h`。调参时建议一次只改一类参数，并记录电机、电池、电流、转速和异常状态。

## 调优顺序

- 切换控制协议：单向DSHOT600,双向BDSHOT600。
- 如要更换6S电机，调整反向电动势的分压电阻，更换VBUS_REF分压电阻。
- 测量并调整motor parameters
- 调整basic，注意分压要调整
- 调整EMF，分压
- 调整消隐重要！！！配合调试引脚，见……配打开gpio测试引脚，配软硬件截图
- 根据测量情况调整ACC_SPD_SLOPE加速度，DEC_SPD_SLOPE减速度
- 调整dshot最大转速MAX_SPEED_RPM、MAX_CCW_SPEED_RPM最小转速SPEED_RPM_MIN.
- 调整open loop control,(抄文档）
- 测量并调整电流采样点
- 调整电流PID
- 进入速度闭环，调中性点延迟BLANK_TIME_OFFSET，速度消隐，速度PID


