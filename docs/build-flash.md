---
title: 编译和烧录
---

# 编译和烧录

本项目使用keil开发，故解析来只讲keil，如需使用IAR等其它IDE，请参考官方文档。

## 软件环境准备

- 下载并安装芯片[pack包](https://www.arterytek.com/file/download/1691)。
- 打开keil工程。
<img src="{{ '/docs/pics/build-flash/打开keil工程.png' | relative_url }}" alt="打开keil工程">
- Keil 的配置需根据各个 AT32 MCU 的闪存存储大小修改 Options 中的 Read/Only MemoryAreas，详细参照表 1，例：AT32M412CBT7 的闪存存储大小为 128 K 字节，则其 IROM1 的起始位置为 0x8000000，大小为 0x1FC00，其 IROM2 的起始位置为 0x801FC00，大小为0x400, AT32M412CBT7 的修改范例如图 2 所示；AT32IDE 的配置需根据各个 AT32 MCU 的闪存存储大小修改Id 文件如图3所示；


表 1. 对应闪存存储空间 ROM 配置表


<table><tr><td>Flash size</td><td>128K</td><td>64K</td></tr><tr><td>IROM1(address)</td><td>0x8000000</td><td>0x8000000</td></tr><tr><td>IROM1(size)</td><td>0x1FC00</td><td>0x0FC00</td></tr><tr><td>IROM2(address)</td><td>0x801FC00</td><td>0x800FC00</td></tr><tr><td>IROM2(size)</td><td>0x400</td><td>0x400</td></tr></table>

[1]: keil v5.33  AT32 BSP  V6.15  keil complier version 5版本进行编译。


图 1. AT32M412CBT7 ROM 配置(Keil)


<img src="{{ '/docs/pics/motor-control/figure-02.jpg' | relative_url }}" alt="图 2">


<img src="{{ '/docs/pics/motor-control/figure-03.jpg' | relative_url }}" alt="图 3">
