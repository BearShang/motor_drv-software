# Repository Guidelines

## Project Structure & Module Organization
This repository contains AT32M412 BLDC motor-control firmware and its vendor dependencies. The main application lives in `at32m412_lv_motor_ev/at32m412/bldc_1shunt_sensorless/`, with board headers under `inc/`, application sources under `src/`, and the Keil MDK project under `mdk_v5/`. Shared motor-control logic is in `mclib/inc` and `mclib/src`. Vendor MCU support and CMSIS files are under `libraries/AT32M412_416_Firmware_Library/`, while `libraries/dsp/` contains DSP support code. Reference manuals and application notes are stored in `document/`.

## Build, Test, and Development Commands
Use Keil MDK/uVision for normal development.

- Open `at32m412_lv_motor_ev/at32m412/bldc_1shunt_sensorless/mdk_v5/bldc_1shunt_sensorless.uvprojx` to build and flash the target.
- Rebuild in uVision before committing parameter changes so `Objects/bldc_1shunt_sensorless.hex` and the map file reflect the current source.
- Review hardware-facing parameters in `inc/motor_control_drive_param.h` before testing on a new motor, battery, or propeller setup.

There is no checked-in CLI build script or CI pipeline in this branch; local validation is done through the Keil project and bench testing.

## Coding Style & Naming Conventions
Follow the existing C style: 2-space indentation, braces on their own lines for functions, and lower_snake_case for functions and variables such as `system_clock_config()` and `calc_spd_rdy`. Keep macro names uppercase with underscores, for example `SYSTEM_CORE_CLOCK` and `OVER_CURRENT_SW`. Preserve the vendor file header blocks and use short inline comments only where hardware intent is not obvious.

## Testing Guidelines
There is no automated unit-test framework in this repository. Validate changes by rebuilding the MDK project, checking for new warnings, and testing on the target board with safe current limits first. For control-loop or startup changes, record the motor, supply voltage, and key parameter values changed in `motor_control_drive_param.h`. Treat `mdk_v5/Objects/` and `mdk_v5/Listings/` outputs as verification artifacts, not hand-edited source.

## Commit & Pull Request Guidelines
Recent commits are short, imperative, and parameter-focused, for example `Update nTsel value in bldc_1shunt_sensorless configuration to 6`. Keep commit subjects concise and specific to the tuning or firmware change. For pull requests, include:

- What hardware was used: board, motor, battery, propeller
- Which files or parameters changed
- Build result from Keil MDK
- Bench-test evidence for risky motor-control changes, such as startup, commutation, or current-limit updates

## Configuration & Safety Notes
Double-check clock source, sensor mode, voltage thresholds, and over-current settings before flashing hardware. Small constant changes in `motor_control_drive_param.h` can materially affect startup behavior and MOSFET stress.
