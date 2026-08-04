---
title: 性能调优
---

# 性能调优

Tiny_ESC 的核心调参集中在 `firmware/at32m412_lv_motor_ev/at32m412/bldc_1shunt_sensorless/inc/motor_control_drive_param.h`。调参时建议一次只改一类参数，调完一步验证一步，避免多变量同时改动导致无法定位问题。

## 调参检查清单

<div class="checklist">
  <label><input type="checkbox"> 电池电压适配（4S 保持默认 / 6S 修改分压参数）</label>
  <label><input type="checkbox"> 上位机模式下调参，完成后切换目标协议</label>
  <label><input type="checkbox"> 电机寄生参数与实物一致</label>
  <label><input type="checkbox"> 开环启动平稳，方向正确</label>
  <label><input type="checkbox"> 电流采样点正确，PI 参数已写闪存</label>
  <label><input type="checkbox"> 消隐窗口干净，无噪声误触发</label>
  <label><input type="checkbox"> 速度 PI 响应无振荡、无过冲</label>
  <label><input type="checkbox"> DSHOT 转速范围合理</label>
</div>

---

## 第一步：6S 电池适配（可选）

如果使用 4S 电池，可跳过此步。如果使用 6S 电池，需完成以下硬件和软件修改。

### 硬件修改

| 电阻 | 原值 | 新值 |
|------|------|------|
| R231（VBUS 分压） | 10kΩ | 5.6kΩ |

<img src="{{ '/docs/pics/performance-tuning/performance-tuning36.png' | relative_url }}?v={{ site.time | date: '%s' }}" alt="R231（VBUS分压）">

| 电阻 | 原值 | 新值 |
|------|------|------|
| R213（BEMF 分压 U 相） | 43kΩ | 56kΩ |
| R217（BEMF 分压 V 相） | 43kΩ | 56kΩ |
| R220（BEMF 分压 W 相） | 43kΩ | 56kΩ |

<img src="{{ '/docs/pics/performance-tuning/performance-tuning34.png' | relative_url }}?v={{ site.time | date: '%s' }}" alt="R213R217R220分压">

### 软件修改

修改 `motor_control_drive_param.h` 中以下宏定义：

| 参数 | 说明 |
|------|------|
| `VDC_RATED` | 额定母线电压 |
| `BAT_LOW_VOLT` | 低电压保护阈值 |
| `OVER_VOLT_THRESHOLD` | 过电压保护阈值 |
| `UNDER_VOLT_THRESHOLD` | 欠电压保护阈值 |
| `V_SENSE_GAIN` | VBUS 电压检测增益 |
| `EMF_SENSE_GAIN` | 反电动势检测增益 |

<img src="{{ '/docs/pics/performance-tuning/performance-tuning35.png' | relative_url }}?v={{ site.time | date: '%s' }}" alt="VBUS 参数配置">

<img src="{{ '/docs/pics/performance-tuning/EMF_gain.png' | relative_url }}?v={{ site.time | date: '%s' }}" alt="EMF gain 参数配置">

---

## 第二步：选择调参模式

先将控制协议切换为上位机模式以便调参，调参完成后再选择单向或双向 DSHOT（目前仅支持 DSHOT600）。

1. 打开 `USE_MOTOR_MONITOR` 宏定义，启用上位机控制模式
2. 调参全部完成后，按需求切换为 `DSHOT600_INPUT`（单向）或 `DSHOT600_BIDIRECTIONAL`（双向）

<img src="{{ '/docs/pics/performance-tuning/performance-tuning3.png' | relative_url }}" alt="控制协议选择">

---

## 第三步：填写电机寄生参数

在 `motor_control_drive_param.h` 中修改以下电机参数为你的实际电机值：

| 参数 | 说明 | 单位 |
|------|------|------|
| `RS_LL` | 相电阻（线间） | Ω |
| `LS_LL` | 相电感（线间） | H |
| `POLE_PAIRS` | 极对数 | P |
| `KE` | 反电动势常数 | V·s/rad |

> **提示：** 这些参数可通过 LCR 电桥测量，或参考电机 datasheet。若使用出厂默认的 KV650 电机，可保持默认值。

---

## 第四步：六步方波开环控制

开环电压控制模式不需要位置传感器即可转动电机，用于：

- 确认电机接线正确、运转方向正确
- 调整 BLDC 无传感器开环启动参数
- 初步验证电流采样是否正常

### 操作步骤

**STEP-1.** 在上位机中将控制模式下拉菜单选为 **Open Loop Control**：

<img src="{{ '/docs/pics/performance-tuning/performance-tuning4.png' | relative_url }}" alt="选择 Open Loop Control 模式">

**STEP-2.** 切换到 **Tuning Parameters** 页面，调整以下参数：

| 参数 | 说明 |
|------|------|
| `OpenLoop initial voltage` | 开环初始电压 |
| `OpenLoop initial speed` | 开环初始速度 |
| `OpenLoop final speed` | 开环最终速度 |
| `OpenLoop times` | 递增次数 |
| `OpenLoop increase volt` | 每次递增电压 |


开环控制的示意图：

<img src="{{ '/docs/pics/performance-tuning/performance-tuning5.png' | relative_url }}" alt="开环控制示意图 - 1">

<img src="{{ '/docs/pics/performance-tuning/performance-tuning6.png' | relative_url }}" alt="开环控制示意图 - 2">

### 调试技巧

1. 刚开始先将 `Openloop increase volt`、`Openloop times` 调整为 0，`Openloop final speed` 与 `Openloop initial speed` 调成一样的数值，先找到适当的**起始电压及速度**
2. 找到适当起始电压后，进一步设定最终速度，并调整 `Openloop increase volt` 以及 `Openloop times`。若觉得运转无力则调大 `Openloop increase volt` 增加每次递增电压，或增大 `Openloop times` 增加递增次数（一般建议递增次数大于 50 次）

**STEP-3.** 按下 **Start Motor** 按钮，观察电流大小及电机运转情况，直到电机正常运转。

> ⚠️ 开环电压避免过大，以免造成电机过热损毁。

**STEP-4.** 将调试后的参数填入 `motor_control_drive_param.h` 文件对应宏定义，并重新编译烧录代码：

<img src="{{ '/docs/pics/performance-tuning/performance-tuning7.png' | relative_url }}" alt="开环参数写入 motor_control_drive_param.h">

---

## 第五步：电流采样点检测与 PI 调节

第四步完成后，继续使用开环模式测试电流采样。

### 5.1 波形检测

测试点颜色对应关系：

| 颜色 | 信号 | 说明 |
|------|------|------|
| 黄色 | U 相电压 | 相电压波形 |
| 绿色 | 电流 | 使用弹簧表笔触碰 `OP3_OUT_BUS` 信号 |
| 蓝色 | 电流测试点 | 采样点位置 |

<img src="{{ '/docs/pics/performance-tuning/performance-tuning8.png' | relative_url }}" alt="电流测试点波形总览">

电流测试点硬件位置 —— 用弹簧表笔触碰 `OP3_OUT_BUS`：

<img src="{{ '/docs/pics/performance-tuning/performance-tuning9.png' | relative_url }}" alt="OP3_OUT_BUS 测试点位置">

### 5.2 测试流程

<img src="{{ '/docs/pics/performance-tuning/performance-tuning10.png' | relative_url }}" alt="电流采样测试流程 - 1">

<img src="{{ '/docs/pics/performance-tuning/performance-tuning11.png' | relative_url }}" alt="电流采样测试流程 - 2">

<img src="{{ '/docs/pics/performance-tuning/performance-tuning12.png' | relative_url }}" alt="电流采样测试流程 - 3">

<img src="{{ '/docs/pics/performance-tuning/performance-tuning13.png' | relative_url }}" alt="电流采样测试流程 - 4">

### 5.3 切换 IQ 模式并调节电流 PI

采样点参数填写完成后，将模式切换为 **IQ 模式** 进行 PI 参数调节：

<img src="{{ '/docs/pics/performance-tuning/performance-tuning14.png' | relative_url }}" alt="切换 IQ 模式 - 1">

<img src="{{ '/docs/pics/performance-tuning/performance-tuning15.png' | relative_url }}" alt="切换 IQ 模式 - 2">

<img src="{{ '/docs/pics/performance-tuning/performance-tuning16.png' | relative_url }}" alt="电流 PI 参数调节 - 1">

<img src="{{ '/docs/pics/performance-tuning/performance-tuning17.png' | relative_url }}" alt="电流 PI 参数调节 - 2">

> ⚠️ 调试的时候修改好 PI 参数后**务必点击写闪存**，否则断电后仍使用之前的 PI 参数。

确认 PI 参数后，修改 `motor_control_drive_param.h` 中的对应宏定义并重新编译烧录：

| 参数 | 说明 |
|------|------|
| `PID_IS_KP_DEFUALT` | 电流环比例增益 |
| `PID_IS_KI_DEFUALT` | 电流环积分增益 |

---

## 第六步：反电动势检测与消隐窗口

这是调参中**最关键也最复杂**的一步。目的是让 MCU 在正确的时机检测反电动势过零点，避免开关噪声导致误判。

### 6.1 波形通道说明

将上位机切换为 **Speed 模式**：

<img src="{{ '/docs/pics/performance-tuning/performance-tuning19.png' | relative_url }}" alt="上位机切换 Speed 模式">

示波器各通道对应关系：

| 颜色 | 信号 | 说明 |
|------|------|------|
| 蓝色 | `EMF_NUTRAL` | 虚拟中性点 |
| 黄色 | `EMF_U` | U 相反电动势 |
| 紫色 | 消隐窗口 | 空白区域 = 检测窗口 |
| 绿色 | 测试点 | 比较器输出 / 过零点标志 |

<img src="{{ '/docs/pics/performance-tuning/performance-tuning18.png' | relative_url }}" alt="反电动势与消隐窗口波形总览">

### 6.2 消隐参数说明

<img src="{{ '/docs/pics/performance-tuning/performance-tuning20.png' | relative_url }}" alt="消隐参数示意图">

| 参数 | 作用 | 调节目标 |
|------|------|----------|
| `EMF_RISE_BLANK_TIME` | 上升沿消隐（左边空白） | 屏蔽开关噪声 |
| `EMF_FALL_BLANK_TIME_LOW_SPD` | 下降沿消隐-低速 | 低于切换转速时使用 |
| `EMF_FALL_BLANK_TIME_HIGH_SPD` | 下降沿消隐-高速 | 高于切换转速时使用 |
| `EMF_BLANK_TIME_CHANGED_RPM` | 高/低速切换转速阈值 | 区分高低速分界点 |
| `BLANK_TIME_OFFSET` | 消隐窗口偏移量 | 微调窗口相位 |

### 6.3 调节方法

上位机中对应的消隐参数设置界面：

<img src="{{ '/docs/pics/performance-tuning/performance-tuning21.png' | relative_url }}" alt="消隐参数配置界面 - 1">

<img src="{{ '/docs/pics/performance-tuning/performance-tuning22.png' | relative_url }}" alt="消隐参数配置界面 - 2">

以下以 **1000 rpm** 为例说明具体调参方向：

<img src="{{ '/docs/pics/performance-tuning/performance-tuning23.png' | relative_url }}" alt="1000rpm 消隐波形总览">

---

#### 上升沿调节（左边）

<img src="{{ '/docs/pics/performance-tuning/performance-tuning24.png' | relative_url }}" alt="上升沿消隐调节">

信号特征：
- 绿色下降对应黄色上升，绿色上升对应黄色下降
- 可通过 `BLANK_TIME_OFFSET` 修正相位

判断标准：从第一个紫色脉冲开始，若**黄色比蓝色大**，则左边不允许出现紫色脉冲波形。通过 `EMF_RISE_BLANK_TIME` 调节。

---

#### 下降沿调节（右边）

<img src="{{ '/docs/pics/performance-tuning/performance-tuning25.png' | relative_url }}" alt="下降沿消隐调节">

信号特征：
- 绿色下降对应黄色上升，绿色上升对应黄色下降
- 可通过 `BLANK_TIME_OFFSET` 修正相位

判断标准：从第一个紫色脉冲开始，若**蓝色比黄色大**，则右边不允许出现紫色脉冲波形。通过 `EMF_FALL_BLANK_TIME_LOW_SPD` 或 `EMF_FALL_BLANK_TIME_HIGH_SPD` 调节，两者由 `EMF_BLANK_TIME_CHANGED_RPM` 区分：

- 当前转速 **<** `EMF_BLANK_TIME_CHANGED_RPM` → 调节 `EMF_FALL_BLANK_TIME_LOW_SPD`
- 当前转速 **>** `EMF_BLANK_TIME_CHANGED_RPM` → 调节 `EMF_FALL_BLANK_TIME_HIGH_SPD`

---

### 6.4 高速消隐波形

高速时通过 `EMF_FALL_BLANK_TIME_HIGH_SPD` 调节消隐：

<img src="{{ '/docs/pics/performance-tuning/performance-tuning26.png' | relative_url }}" alt="高速消隐波形 - 1">

<img src="{{ '/docs/pics/performance-tuning/performance-tuning27.png' | relative_url }}" alt="高速消隐波形 - 2">

<img src="{{ '/docs/pics/performance-tuning/performance-tuning28.png' | relative_url }}" alt="高速消隐波形 - 3">

消隐调节完成后，将参数写入 `motor_control_drive_param.h` 并重新编译烧录代码。

### 6.5 极限电机转速

当电机需要运行在极高速（如 18000 rpm 以上）时，需要进行相位提前配置，否则换相滞后会导致失步或效率大幅下降。

#### 开启相位提前

在 `motor_control_drive_param.h` 中开启 `PHASE_ADVANCE` 宏定义：

```c
#define PHASE_ADVANCE
```

#### 调整相位提前角度

| 参数 | 说明 |
|------|------|
| `EMF_PHASE_ADV_SPD` | 相位提前使能转速阈值（rpm）|

该值**越小，相位提前角度越大**（即相位越提前）。建议从较大的值开始逐步下调，观察电机高速运转的稳定性和电流，直到找到最佳值。

```c
#define EMF_PHASE_ADV_SPD               (40000)     /*!< rpm */
```

#### 进一步压缩高速消隐窗口

极限高速运行时，换相周期极短，消隐窗口会占用过零点检测的有效时间窗口。`EMF_FALL_BLANK_TIME_HIGH_SPD` 越小，留给 MCU 检测过零点的时间就越多，换相判断也就越及时，从而避免高速失步。

在 6.4 节高速消隐调好的基础上，进入极限转速测试后**进一步减小** `EMF_FALL_BLANK_TIME_HIGH_SPD`：

```c
#define EMF_FALL_BLANK_TIME_HIGH_SPD  (1.5f)          /*!< usec，越小高速检测越快 */
```

逐步下调，直至电机在目标极限转速下稳定运行。

#### 最高转速限制

| 参数 | 说明 |
|------|------|
| `MAX_SPEED_RPM` | 正向最高转速（rpm） |
| `MAX_CCW_SPEED_RPM` | 反向最高转速（rpm） |

> ⚠️ 如果实际转速超过 `MAX_SPEED_RPM` / `MAX_CCW_SPEED_RPM` 的设定值，上位机会出现**数据乱码**。务必确保这两个值大于等于系统的实际最高运行转速。

```c
#define MAX_SPEED_RPM              (35000)     /*!< rpm */
#define MAX_CCW_SPEED_RPM          (35000)     /*!< rpm */
```

---

## 第七步：速度闭环 PI 调节

消隐窗口调好后，进入速度闭环 PI 调节。

| 参数 | 说明 |
|------|------|
| `ACC_SPD_SLOPE` | 加速斜率（转速上升速率） |
| `DEC_SPD_SLOPE` | 减速斜率（转速下降速率） |
| `PID_SPD_KP_DEFAULT` | 速度环比例增益 |
| `PID_SPD_KI_DEFAULT` | 速度环积分增益 |

<img src="{{ '/docs/pics/performance-tuning/performance-tuning29.png' | relative_url }}" alt="速度 PI 调节 - 1">

<img src="{{ '/docs/pics/performance-tuning/performance-tuning30.png' | relative_url }}" alt="速度 PI 调节 - 2">

<img src="{{ '/docs/pics/performance-tuning/performance-tuning31.png' | relative_url }}" alt="速度 PI 调节 - 3">

<img src="{{ '/docs/pics/performance-tuning/performance-tuning32.png' | relative_url }}" alt="速度 PI 调节 - 4">

调节好 PI 参数后，将参数写入 `motor_control_drive_param.h` 并重新编译烧录代码。

---

## 第八步：DSHOT 转速配置

| 参数 | 说明 |
|------|------|
| `SPEED_RPM_MIN` | DSHOT 启动转速（低于此值不响应） |
| `MAX_SPEED_RPM` | 正向最高转速 |
| `MAX_CCW_SPEED_RPM` | 反向最高转速 |

<img src="{{ '/docs/pics/performance-tuning/performance-tuning33.png' | relative_url }}" alt="DSHOT 转速配置">

修改完成后重新编译烧录，至此调参全部完成。
