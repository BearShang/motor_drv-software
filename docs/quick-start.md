---
title: 快速开始
---

# 快速开始

这页用于把 Tiny_ESC 从代码工程带到第一次安全转动。更细的电机库、比较器、过流保护说明见 [性能调优](performance-tuning.html)。

## 准备

- Tiny_ESC 硬件板。
- 4S，KV650的电机（出厂固件按这个电机调的，需要更换参考[性能调优](performance-tuning.html)）。
- 4S电池或可调电源。
- usb转uart调试器。
- PC电脑，及原厂[上位机软件](https://www.arterytek.com/file/download/2014)。

## 硬件连接

- 电机三相连接到开发板的 U/V/W 输出端。
- 电源正负极连接到母线输入端，先不要打开电源输出。
<img src="{{ '/docs/pics/UVW 输出端.jpg' | relative_url }}" alt="UVW 输出端">
- 使用上位机时，将串口 TX/RX/GND 连接到控制板通信接口。
<img src="{{ '/docs/pics/uart调试口.jpg' | relative_url }}" alt="uart调试口">
- 打开上位机软件
<img src="{{ '/docs/pics/监视器打开项目.png' | relative_url }}" alt="监视器打开项目">
- 打开工程
<img src="{{ '/docs/pics/监视器打开项目库2.png' | relative_url }}" alt="监视器打开项目库2">
- 打开串口
<img src="{{ '/docs/pics/监视器打开项目3.png' | relative_url }}" alt="监视器打开项目3">

## 第一次启动

1. 打开母线电源。
2. 先给很小的速度或油门指令，观察电机是否完成开环启动并进入稳定运行。
<img src="{{ '/docs/pics/启动电机.png' | relative_url }}" alt="启动电机">
3. 逐步提高指令，同时观察母线电流、转速、温升和异常告警。
4. 如果出现抖动、反转、过流或启动失败，先停止输出，再按 [性能调优](performance-tuning.html) 调整启动和检测参数。

## 输入方式

当前工程支持上位机控制、单向 DSHOT600 和双向 BDSHOT600。默认固件使用原厂上位机软件控制，如需修改见 [性能调优](performance-tuning.html)。

| 配置项 | 用途 |
| --- | --- |
| `DSHOT600_INPUT` | 启用单向 DSHOT600 输入 |
| `DSHOT600_BIDIRECTIONAL` | 启用双向 DSHOT600 |

## 安全检查

- 空载调试确认稳定后，再接入真实负载。
- 电机发热、MOS 发热或电流异常时，不要继续加大油门。
- 过流阈值、低压阈值、最高转速需要按实际电机、电池和散热条件重新确认。
