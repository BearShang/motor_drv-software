---
title: 快速开始
---

# 快速开始

这页用于把 Tiny_ESC 从代码工程带到第一次安全转动。更细的电机库、比较器、过流保护说明见 [电机控制](motor-control.html)。

## 准备

- Tiny_ESC 硬件板、三相 BLDC 电机、4S~6S 电池或限流直流电源。
- AT-Link 或兼容调试下载器。
- Keil MDK v5 / AT32 BSP 环境，打开工程 `firmware/at32m412_lv_motor_ev/at32m412/bldc_1shunt_sensorless/mdk_v5/bldc_1shunt_sensorless.uvprojx`。
- 如需上位机调试，准备 ArteryMotorMonitor 和 USB 转 TTL 串口。
- 首次上电建议使用限流电源，并让电机空载固定，避免桨叶或负载直接参与调试。

## 硬件连接

- 电机三相连接到开发板的 U/V/W 输出端。
- 电源正负极连接到母线输入端，先不要打开电源输出。
- 调试下载器连接到 SWD 调试接口。
- 使用上位机时，将串口 TX/RX 连接到控制板通信接口。
- 使用 PWM/DSHOT 外部控制时，将控制信号接入对应输入端，并保证控制器与 ESC 共地。

## 编译和烧录

1. 打开 Keil 工程。
2. 检查 `motor_control_drive_param.h` 中的输入方式、启动方式、电流限制和电压保护参数。
3. 默认工程面向无感 BLDC 六步方波控制，启动方式为 `OPENLOOP_STARTUP`。
4. 编译工程，确认没有错误。
5. 烧录程序后，先保持母线电源关闭，确认调试连接稳定。

## 第一次启动

1. 将直流电源限流设置到较小电流，电压设置在目标电池电压范围内。
2. 打开母线电源。
3. 如果使用 ArteryMotorMonitor，连接后清除低压等上电前遗留告警。
4. 先给很小的速度或油门指令，观察电机是否完成开环启动并进入稳定运行。
5. 逐步提高指令，同时观察母线电流、转速、温升和异常告警。
6. 如果出现抖动、反转、过流或启动失败，先停止输出，再按 [性能调优](performance-tuning.html) 调整启动和检测参数。

## 输入方式

当前工程支持上位机控制、单向 DSHOT600 和双向 BDSHOT600。DSHOT 相关配置集中在 `motor_control_drive_param.h`：

| 配置项 | 用途 |
| --- | --- |
| `DSHOT600_INPUT` | 启用单向 DSHOT600 输入 |
| `DSHOT600_BIDIRECTIONAL` | 启用双向 DSHOT600 |
| `DSHOT_CMD_MIN` / `DSHOT_CMD_MAX` | 有效油门命令范围 |
| `DSHOT600_SIGNAL_LOSS_COUNT` | DSHOT 信号丢失判定次数 |

## 安全检查

- 空载调试确认稳定后，再接入真实负载。
- 电机发热、MOS 发热或电流异常时，不要继续加大油门。
- 过流阈值、低压阈值、最高转速需要按实际电机、电池和散热条件重新确认。
