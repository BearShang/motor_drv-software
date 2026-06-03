---
title: 编译和烧录
---

# 编译和烧录

1. 打开 Keil 工程。
2. 检查 `motor_control_drive_param.h` 中的输入方式、启动方式、电流限制和电压保护参数。
3. 默认工程面向无感 BLDC 六步方波控制，启动方式为 `OPENLOOP_STARTUP`。
4. 编译工程，确认没有错误。
5. 烧录程序后，先保持母线电源关闭，确认调试连接稳定。

## 软件环境准备

1) 开启 bldc_1shunt_sensorless 范例工程

2) 电机应用 PC 软件 ArteryMotorMonitor.exe（本软件不需安装，只需直接运行可执行程序）。

3) Keil 的配置需根据各个 AT32 MCU 的闪存存储大小修改 Options 中的 Read/Only MemoryAreas，详细参照表 1，例：AT32M412CBT7 的闪存存储大小为 128 K 字节，则其 IROM1 的起始位置为 0x8000000，大小为 0x1FC00，其 IROM2 的起始位置为 0x801FC00，大小为0x400, AT32M412CBT7 的修改范例如图 2 所示；AT32IDE 的配置需根据各个 AT32 MCU 的闪存存储大小修改Id 文件如图3所示；IAR Systems的配置需根据各个AT32 MCU 的闪存存储大小修改icf 文件如图 4所示。


表 1. 对应闪存存储空间 ROM 配置表


<table><tr><td>Flash size</td><td>128K</td><td>64K</td></tr><tr><td>IROM1(address)</td><td>0x8000000</td><td>0x8000000</td></tr><tr><td>IROM1(size)</td><td>0x1FC00</td><td>0x0FC00</td></tr><tr><td>IROM2(address)</td><td>0x801FC00</td><td>0x800FC00</td></tr><tr><td>IROM2(size)</td><td>0x400</td><td>0x400</td></tr></table>

[1]: keil v5.33  AT32 BSP  V6.15  keil complier version 5版本进行编译。


图 2. AT32M412CBT7 ROM 配置(Keil)


<img src="{{ '/docs/pics/motor-control/figure-02.jpg' | relative_url }}" alt="图 2">


<img src="{{ '/docs/pics/motor-control/figure-03.jpg' | relative_url }}" alt="图 3">


图 3. AT32M412CBT7 ROM 配置(AT32IDE)


<img src="{{ '/docs/pics/motor-control/figure-04.jpg' | relative_url }}" alt="图 4">


图 4. AT32M412CBT7 之 ROM 配置(IAR)


<img src="{{ '/docs/pics/motor-control/figure-05.jpg' | relative_url }}" alt="图 5">
