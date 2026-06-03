---
title: 性能调优
---

# 性能调优

Tiny_ESC 的核心调参集中在 `firmware/at32m412_lv_motor_ev/at32m412/bldc_1shunt_sensorless/inc/motor_control_drive_param.h`。调参时建议一次只改一类参数，并记录电机、电池、电流、转速和异常状态。

## 调优顺序

1. 先确认硬件连接、电源限流、相线顺序和散热条件。
2. 调整保护阈值，让过压、欠压、过流保护先符合硬件能力。
3. 调整启动参数，让电机能稳定进入闭环或稳定运行区。
4. 调整反电势检测和消隐窗口，减少误换相、丢步和高速抖动。
5. 最后调速度环和低速电压控制，让响应和稳定性达到目标。

## 保护参数

| 参数 | 作用 |
| --- | --- |
| `MAX_CURRENT` | 控制层电流限幅 |
| `OVER_CURRENT_SW` | 软件过流判断电流 |
| `OCP_CURRENT` | 比较器/DAC 过流保护目标电流 |
| `OVER_VOLT_THRESHOLD` | 母线过压保护阈值 |
| `UNDER_VOLT_THRESHOLD` | 母线欠压保护阈值 |
| `BAT_LOW_VOLT` | 允许的最低电池电压 |

先把保护阈值调到硬件和电池允许范围内，再做性能调优。若刚启动就过流，优先降低启动电压、启动电流或开环加速斜率，而不是简单抬高保护阈值。

## 启动参数

| 参数 | 作用 |
| --- | --- |
| `CONST_VOLTAGE_START` / `CONST_CURRENT_START` | 选择恒压或恒流启动 |
| `OPENLOOP_STARTUP` | 使用开环启动 |
| `START_VOLTAGE` | 初始启动电压 |
| `START_CURRENT` | 初始启动电流 |
| `OLC_INIT_VOLT` | 开环控制初始电压 |
| `OLC_VOLT_INC` | 开环阶段电压增量 |
| `OLC_STARTUP_PERIOD` | 开环启动持续时间 |
| `LOCK_VOLT` | 启动前转子对齐电压 |

启动失败通常先从 `START_VOLTAGE`、`OLC_INIT_VOLT`、`OLC_VOLT_INC` 和 `OLC_STARTUP_PERIOD` 入手。低 KV 或大惯量负载需要更温和的加速；高 KV 小电机则要避免启动电压过高导致瞬时过流。

## 反电势检测

无感 BLDC 依赖反电势过零点判断换相时机。当前工程使用内部比较器，并提供消隐窗口参数过滤 PWM 开关噪声。

| 参数 | 作用 |
| --- | --- |
| `EMF_RISE_BLANK_TIME` | 上升沿过零后的消隐时间 |
| `EMF_FALL_BLANK_TIME_HIGH_SPD` | 高速下降沿消隐时间 |
| `EMF_FALL_BLANK_TIME_LOW_SPD` | 低速下降沿消隐时间 |
| `EMF_BLANK_TIME_CHANGED_RPM` | 高低速消隐参数切换转速 |
| `BLANK_TIME_OFFSET` | 消隐窗口修正量 |
| `EMF_SAMPLE_LEAD_PWM` | EMF 采样提前量 |

低速抖动、误换相或启动后立刻失步时，优先检查消隐窗口和采样点是否太靠近 PWM 开关边缘。高速抖动时，重点检查高速下降沿消隐时间和相位提前相关参数。

## 速度和油门映射

| 参数 | 作用 |
| --- | --- |
| `MIN_SPEED_RPM` | 最低运行转速 |
| `MAX_SPEED_RPM` | 最高正转转速 |
| `MAX_CCW_SPEED_RPM` | 最高反转转速 |
| `SPEED_RPM_MIN` | DSHOT 油门映射的起始转速 |
| `SPEED_FILTER_TIMES` | 转速滤波次数 |
| `PID_SPD_KP_DEFUALT` / `PID_SPD_KI_DEFUALT` | 速度环 PI 参数 |
| `PID_SPD_VOLT_KP_DEFUALT` / `PID_SPD_VOLT_KI_DEFUALT` | 低速电压控制 PI 参数 |

响应太慢时，可以小幅提高 `PID_SPD_KP_DEFUALT`；转速来回振荡时，降低 KP 或 KI，并适当增加滤波。低速段如果电流环不稳定，可关注 `LOW_SPEED_VOLT_CTRL`、`HYSTERESIS_LOW_SPEED` 和 `HYSTERESIS_HIGH_SPEED`。

## 常见现象

| 现象 | 优先检查 |
| --- | --- |
| 启动抖动但不转 | 相线顺序、`START_VOLTAGE`、`OLC_INIT_VOLT`、`LOCK_VOLT` |
| 刚启动就过流 | `START_CURRENT`、`START_VOLTAGE`、`OCP_CURRENT`、负载是否过大 |
| 低速断续或丢步 | `EMF_FALL_BLANK_TIME_LOW_SPD`、`EMF_SAMPLE_LEAD_PWM`、低速控制 PI |
| 高速抖动 | 高速消隐时间、相位提前、`MAX_SPEED_RPM` |
| 油门不线性 | DSHOT 命令范围、`SPEED_RPM_MIN`、`MAX_SPEED_RPM` |
| 急加速掉速 | 电源限流、电池压降、`MAX_CURRENT`、速度环 KP/KI |

## 记录模板

每次调参建议记录：

- 电机型号、KV、负载和供电电压。
- 修改的宏定义和修改前后数值。
- 启动是否成功、最低稳定转速、最高稳定转速。
- 峰值电流、稳态电流、MOS 和电机温升。
- 是否出现过流、欠压、失步、抖动或 DSHOT 信号丢失。
