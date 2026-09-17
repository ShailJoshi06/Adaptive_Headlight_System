# Automated Headlight Control System for Vehicles

## Purpose

This project develops an automated headlight control model for vehicles. The system adjusts headlight brightness automatically based on two factors: the light from an incoming vehicle and the distance from an obstacle. The goal is to reduce the risk of collision and to prevent temporary blindness to other drivers caused by high beam glare.

## Hardware Used

- STM32F39ZI development board, running the main control logic
- VEGA board, an Arduino equivalent board, used for ultrasonic sensing
- 5 x GL5528 LDR sensors, used for ambient light and incoming vehicle light detection
- 4 x HC-SR04 ultrasonic sensors, used for obstacle distance detection
- 4 x WS2812 addressable LED strips, used to simulate the vehicle headlights

The VEGA board and the STM32 board communicate over UART.

## System Overview

The VEGA board reads distance values from the 4 ultrasonic sensors and sends them to the STM32 board over UART in a comma separated format, for example dist1,dist2,dist3,dist4.

The STM32 board reads the 5 LDR sensors through its ADC channels, 3 through ADC1 and 2 through ADC3. One of the 5 LDR readings is used as an ambient light normalization reference, and the remaining 4 correspond to the 4 headlight strips.

Using both the distance values and the LDR values, the STM32 decides the brightness level for each of the 4 LED strips independently, and updates the strips using the WS2812 driver. The logic followed for each strip is as below:

1. Daytime check: If the ambient light reading is above a set threshold, the strip is set to a low brightness daytime running light mode, regardless of distance or incoming light.
2. Glare check: If nighttime, and the LDR reading at that strip is much brighter than the ambient reading, it is treated as an incoming vehicle, and the strip is switched to a low beam pattern, where the top LEDs of the strip are turned off to reduce glare for the other driver.
3. Obstacle check: If nighttime and no incoming vehicle glare is detected, but an obstacle is closer than a set distance threshold, the strip is set to a high brightness mode to improve visibility.
4. Normal night driving. If none of the above apply, the strip is set to a standard nighttime brightness.

Brightness changes are not applied instantly. The brightness of each strip is faded smoothly from its current value to its target value, so that transitions look natural instead of switching instantly.

Sensor readings and the current state of each strip are also printed over a separate UART line, so they can be logged or viewed on a connected computer.

## Diagnostic and Visualization Tool

A Python based diagnostic dashboard was built using Streamlit and Plotly to visualize the sensor data in real time. It connects to the STM32 board over a serial port, reads the printed sensor and state lines, and displays:

- A top down view of the 4 ultrasonic sensor zones, color coded by distance and danger level
- A distance chart for the 4 sensors
- A live status table with per sensor distance, state, and LDR reading
- A vehicle presence indicator, showing whether an incoming vehicle has been detected on any strip

This tool is meant for testing and debugging the system on a computer while the hardware is running, and is not required for the headlight logic itself to function.

## Repository Contents

| File | Description |
|---|---|
| Core/Src/main.c | STM32Cube IDE main application file. Contains peripheral initialization, the UART receive loop for distance data, the ADC read loop for the 5 LDR sensors, and the headlight decision and fading logic. |
| Core/Inc/main.h | STM32Cube IDE generated header file with pin and peripheral definitions for the STM32 board. |
| Core/Src/ws2812b.c | Driver for the WS2812 LED strips. Handles setting LED colors, applying brightness scaling, and sending data to the strips using timer PWM and DMA. |
| Core/Inc/ws2812b.h | Header file for the WS2812 driver, with strip identifiers and function declarations. |
| project.ioc | STM32CubeMX configuration file for the project. Defines the pin mux, clock setup, and peripheral configuration for USART3, USART6, ADC1, ADC3, TIM2, TIM3, and DMA, and is used to regenerate the initialization code in main.c if settings are changed. |
| VegaBoard_embedded/VegaBoard_embedded.ino | Code running on the VEGA board. Reads the 4 HC-SR04 ultrasonic sensors and sends the distance values to the STM32 board over UART. |
| GUI/GUI_code.txt | Python Streamlit script for the serial diagnostic dashboard described above. |

Note, file extensions will need to be corrected when placing them into the actual project structure; GUI_code.txt should be renamed to a .py file to run with Streamlit.

## Requirements

- STM32CubeIDE, for building and flashing the STM32F39ZI project
- Arduino IDE or equivalent, for building and flashing the VEGA board
- Python 3, with streamlit, plotly, pandas, numpy, and pyserial installed, for running the diagnostic dashboard
- A UART connection between the VEGA board and the STM32 board
- A serial connection between the STM32 board and a computer, for logging and for the dashboard

## Running the Diagnostic Dashboard

1. Flash the STM32 project and the VEGA project onto their respective boards.
2. Connect the STM32 board to a computer over USB or a USB to serial adapter.
3. Install the Python dependencies, then run the dashboard with:

`
streamlit run GUI_code.py
`

4. In the sidebar, select the correct serial port and baud rate (115200), matching the STM32 UART configuration, then click Connect.
5. The dashboard will begin showing live sensor data and system state as the boards run.

See the [video](https://youtube.com/shorts/JVcNRrL9pG4?feature=share) for the implementation demo. 