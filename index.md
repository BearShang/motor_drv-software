---
title: 简介
---

## 概述

- 泛用低压三相电机驱动器，无感BLDC，适用于无人机ESC，RC小车驱动等。
- 上升沿10ns，极小的米勒平台。
- AT32M412KBU7-4 (Cortex-M4F)是一颗180MHz时脉带有浮点运算器的微控制器，双ADC转换器、 2个比较、4个运放，几乎不需要外设。
- 适合4s~6s电池。
- MCU的ADC(CMP INP)引脚连接端电压检测，CMP INM连接虚拟中性点，通过比较器检测反向电动势的零交越点。
- 1个直流地母线电流检测电阻，搭配MCU内建OPA可实现单电阻电流电测。
- MCU内建的OPA放大母线电流信号，可通过配置，直连内建的CMP，构成过流保护单元，过流点可由DAC配置。
- 支持雅特丽原厂上位机控制、单向DSHOT600、双向BDSHOT600。

<img src="{{ '/docs/pics/length.jpg' | relative_url }}" alt="长度尺寸图">

<img src="{{ '/docs/pics/width.jpg' | relative_url }}" alt="宽度尺寸图">

## 测试台

<video controls preload="metadata" playsinline>
  <source src="{{ '/docs/pics/测试台.mp4' | relative_url }}" type="video/mp4">
  当前浏览器不支持视频播放。
</video>

## 上飞机

<video controls preload="metadata" playsinline>
  <source src="{{ '/docs/pics/上飞机.mp4' | relative_url }}" type="video/mp4">
  当前浏览器不支持视频播放。
</video>

## 感谢

特别感谢雅特丽原厂工程师们的支持！！
