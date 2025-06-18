# STM32 Thermal Tracker with AMG8833 & Live USB Display

This project uses an STM32F407 and AMG8833 thermal sensor to track heat signatures and display them on a laptop via USB CDC using a Python-based live thermal grid.

## Features
- Reads 8x8 temperature grid from AMG8833
- Sends real-time data over USB CDC to laptop
- Displays thermal map using matplotlib
- Dual-servo motor support to track heat sources

## Hardware Used
- STM32F407 Discovery/Nucleo Board
- AMG8833 Thermal Camera (8x8 grid)
- 2x Servo Motors
- USB Cable (Type-B to Type-A)

## Live Display
Python script (`thermal_display.py`) shows real-time heatmap.

## Getting Started
1. Flash STM32 code from `STM32_Firmware/`
2. Connect USB and run `thermal_display.py`
3. Adjust COM port in script
