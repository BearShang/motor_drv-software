# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

AT32M412-based BLDC motor driver firmware for drone applications. Implements sensorless 6-step commutation control using back-EMF detection via internal comparators, with single-shunt current sensing.

**Target MCU:** AT32M412KBU7-4 (Cortex-M4 @ 180MHz, 128KB Flash, 16KB RAM)

## Build System

**Toolchain:** Keil MDK-ARM v5 with ARM Clang v6.19

**Project file:** `at32m412_lv_motor_ev/at32m412/bldc_1shunt_sensorless/mdk_v5/bldc_1shunt_sensorless.uvprojx`

Build via Keil µVision IDE: Project → Build Target (F7)

**Output:** `Objects/bldc_1shunt_sensorless.axf` and `.hex`

## Architecture

```
Application (main.c)
    │
    ▼
Motor Control Library (mclib/)
    ├── mc_bldc.c/h              - 6-step commutation
    ├── mc_bldc_sensorless.c/h   - EMF zero-crossing detection
    ├── mc_hwio.c/h              - Hardware I/O abstraction
    ├── motor_control_bldc.c     - Control mode & speed ramping
    ├── mc_esc_state.c           - ESC state machine
    ├── mc_pid_controller.c      - Speed/current PID loops
    ├── mc_curr_fdbk.c           - Current feedback & filtering
    └── mc_bldc_kernal           - Pre-compiled motor core (binary)
    │
    ▼
Vendor HAL (libraries/AT32M412_416_Firmware_Library/)
    └── Peripheral drivers (TMR, ADC, GPIO, CMP, USART, etc.)
```

## Key Configuration Files

- **`motor_control_drive_param.h`** - Motor parameters, drive settings, PID gains, protection thresholds
- **`mc_hwio_m412_lv_v1_0.h`** - Hardware pin mappings, timer/ADC/comparator configuration
- **`mc_bldc_globals.h`** - Global control variables and motor calculations

## Hardware Configuration

| Resource | Function |
|----------|----------|
| TMR1 | PWM generation (30kHz), 3-phase with complementary outputs |
| TMR2 | Phase commutation timing |
| ADC1 | Single-shunt current sampling |
| CMP1/CMP2 | Back-EMF zero-crossing detection |
| UART1 | Motor monitor interface (1.5 Mbaud) |

**Phase outputs:** PA8/PB7 (A), PA9/PA12 (B), PA10/PB6 (C) - high/low side pairs

## Motor Control Modes

Configured via `#define` in `motor_control_drive_param.h`:

- **Sensor mode:** `SENSORLESS` with `BLDC_SENSORLESS_COMP` (comparator-based EMF)
- **Startup:** `OPENLOOP_STARTUP` (open-loop ramp 100→600 RPM)
- **Current control:** `LOW_SPEED_VOLT_CTRL` for low-speed stability
- **PWM mode:** `COMPLEMENT` (high-side PWM, low-side complementary)

## Protection Thresholds

| Protection | Value |
|------------|-------|
| Over-current (software) | 20 A |
| Over-current (hardware/comparator) | 30 A |
| Over-voltage | 17 V |
| Under-voltage | 12 V |
| Over-temperature | 70°C |

## Key Parameters

```c
POLE_PAIRS        = 7 (14-pole motor)
PWM_FREQ          = 30000 Hz
DEADTIME_NS       = 50 ns
MIN_SPEED_RPM     = 100
MAX_SPEED_RPM     = 11900
ACC_SPD_SLOPE     = 5 rpm/ms
R_SHUNT           = 0.002 Ω
OP_GAIN           = 27.708
```

## External Tools

**Motor Monitor:** UART-based tuning interface for real-time parameter adjustment and monitoring. Enable with `#define USE_MOTOR_MONITOR`.

Documentation available in `document/` folder (AN0063 for motor monitor usage).
