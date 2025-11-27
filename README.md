# Embedded Systems Laboratory Projects

## ECE319K - Introduction to Embedded Systems
### University of Texas at Austin 

## Overview

This repository contains a comprehensive collection of embedded systems laboratory projects developed on the TI MSPM0G3507 LaunchPad platform. The coursework progresses from fundamental assembly programming and hardware interfacing through advanced topics including real-time operating concepts, communication protocols, and interactive game development.

## Authors

- **Anmol Dubey** 

## Hardware Platform

| Component | Specification |
|-----------|---------------|
| Microcontroller | TI MSPM0G3507 |
| Core | ARM Cortex-M0+ |
| Clock Speed | 80 MHz |
| Flash | 128 KB |
| SRAM | 32 KB |
| ADC | 12-bit, multiple channels |
| Timers | Multiple 16/32-bit timers |
| Communication | SPI, UART, I2C |

## Project Summary

| Lab | Title | Key Concepts |
|-----|-------|--------------|
| 1 | [Assembly Programming](./assemblyLinkedListSearch) | ARM assembly, linked data structures, AAPCS compliance |
| 2 | [LED & Switch Control](./hardwareInterfacingLED+SwitchControl) | GPIO, software PWM, timing loops |
| 3 | [Minimally Intrusive Debugging](./minimallyIntrusiveDebugging3) | Hardware timers, data logging, period measurement |
| 4 | [Traffic Light Controller](./trafficLightControllerFSM) | Finite state machines, sensor input, Moore FSM |
| 5 | [Digital Piano](./soundGenerationDAC) | DAC output, audio synthesis, interrupt-driven waveforms |
| 6 | [LCD Device Driver](./lcdDeviceDriver) | SPI protocol, graphics rendering, device drivers |
| 7 | [Analog Position Meter](./analogPositionMeter) | ADC sampling, fixed-point math, calibration |
| 8 | [Distributed Data Acquisition](./distributedDataAcquisitionSystem) | UART communication, FIFO buffers, multi-board systems |
| 9 | [Geometry Dash Game](./geometryDashGame) | Real-time systems, game physics, audio integration |

## Laboratory Details

### Lab 1: Assembly Programming

**Objective:** Implement a function in ARM Cortex-M0+ assembly to search linked student records.

**Skills Developed:**
- ARM assembly language programming
- Linked data structure traversal
- Register management and AAPCS compliance
- String comparison algorithms

**Key Functions:**
- Linked list iteration
- Character-by-character string matching
- Register preservation (R4-R7)

---

### Lab 2: Hardware Interfacing - LED & Switch Control

**Objective:** Generate precise PWM signals to control LED brightness through software timing.

**Skills Developed:**
- Direct GPIO register manipulation
- Software delay loop calibration
- Duty cycle calculation and implementation

**Technical Specifications:**
- 50 Hz PWM frequency (20 ms period)
- Four distinct duty cycles (variable brightness)
- ±3% timing accuracy requirement

---

### Lab 3: Minimally Intrusive Debugging

**Objective:** Create a software debugging instrument with minimal impact on program execution.

**Skills Developed:**
- Hardware timer configuration (Timer G12)
- Timestamped data capture
- Period and duty cycle measurement
- Non-intrusive instrumentation techniques

**Features:**
- 50-sample circular buffer
- 12.5 ns timing resolution
- Rising edge detection for period calculation

---

### Lab 4: Traffic Light Controller FSM

**Objective:** Design and implement a Moore finite state machine for traffic intersection control.

**Skills Developed:**
- FSM design methodology
- State transition table implementation
- Sensor-driven control systems
- Safe sequencing logic

**System Features:**
- 12-state FSM
- Three traffic directions (South, West, Pedestrian)
- Sensor-responsive state transitions
- Flashing walk signal warning sequence

---

### Lab 5: Digital Piano

**Objective:** Build a functional digital piano using DAC output and keyboard scanning.

**Skills Developed:**
- Binary-weighted DAC design
- Waveform synthesis
- Interrupt-driven audio generation
- Real-time input processing

**Technical Specifications:**
- 5-bit DAC (32 voltage levels)
- 32-sample sine wave lookup table
- Four musical notes (C4, D4, E4, F4)
- Chord support through period averaging

---

### Lab 6: LCD Device Driver

**Objective:** Develop a complete device driver for the ST7735 TFT LCD display.

**Skills Developed:**
- SPI protocol implementation
- Assembly-level busy-wait routines
- Graphics primitive rendering
- Integer-to-string conversion without hardware division

**Display Capabilities:**
- 128×160 pixel, 16-bit color
- Character and string rendering
- Bitmap display
- Data plotting functions

---

### Lab 7: Analog Position Meter

**Objective:** Create a real-time position measurement system using ADC and LCD display.

**Skills Developed:**
- ADC configuration and sampling
- Fixed-point arithmetic
- Sensor calibration techniques
- Real-time display updates

**Technical Specifications:**
- 12-bit ADC resolution
- 30 Hz sampling rate
- Linear conversion: Position = (2000 × ADC) >> 12
- Fixed-point output format (X.XXX cm)

---

### Lab 8: Distributed Data Acquisition System

**Objective:** Implement a two-board UART communication system for remote sensing.

**Skills Developed:**
- UART protocol configuration
- FIFO buffer implementation
- Multi-processor system design
- Real-time data visualization

**System Architecture:**
- Transmitter: Slidepot → ADC → UART
- Receiver: UART → FIFO → LCD
- 2000 baud communication rate
- 4-byte message protocol

---

### Lab 9: Geometry Dash Game (Capstone)

**Objective:** Develop a complete side-scrolling rhythm game with multiple player modes.

**Skills Developed:**
- Game engine architecture
- Sprite-based graphics
- Collision detection systems
- Audio synchronization
- State machine game logic

**Game Features:**
- Three player modes (Cube, Ship, Wave)
- Mode transformation portals
- Bilingual support (English/Spanish)
- Background music with DAC synthesis
- 30 Hz game loop at 4 pixels/frame scroll speed

## Skills Progression

```
Lab 1-2: Foundation
├── Assembly language fundamentals
├── GPIO and register-level programming
└── Software timing and delays

Lab 3-4: Control Systems
├── Hardware timer utilization
├── Finite state machine design
└── Debugging instrumentation

Lab 5-6: Peripherals & Communication
├── DAC and ADC interfacing
├── SPI protocol implementation
└── Device driver development

Lab 7-8: Data Acquisition
├── Sensor integration
├── UART communication
└── Multi-processor systems

Lab 9: Integration
├── Real-time game development
├── Audio-visual synchronization
└── Complete system integration
```

## Common Hardware Connections

### UART Debug Interface (All Labs)

| Signal | Pin | Function |
|--------|-----|----------|
| UART0 Tx | PA10 | Debug output |
| UART0 Rx | PA11 | Debug input |

*Requires jumpers J25 and J26 for XDS110 connection*

### ST7735 LCD (Labs 6-9)

| Signal | Pin | Function |
|--------|-----|----------|
| SCK | PB9 | SPI clock |
| MOSI | PB8 | SPI data |
| CS | PB6 | Chip select |
| DC | PA13 | Data/Command |
| RST | PB15 | Reset |

### DAC Output (Labs 5, 9)

| Bit | Pin | Resistor Weight |
|-----|-----|-----------------|
| 4 (MSB) | PB4 | R |
| 3 | PB3 | 2R |
| 2 | PB2 | 4R |
| 1 | PB1 | 8R |
| 0 (LSB) | PB0 | 16R |

## Development Environment

| Tool | Version/Details |
|------|-----------------|
| IDE | Code Composer Studio |
| Compiler | TI-CLANG |
| Debugger | XDS110 On-board |
| Target | MSPM0G3507 LaunchPad |

## Building Projects

Each lab is contained in its own directory with independent build configuration:

```bash
# Navigate to specific lab
cd <lab_directory>

# Import project into Code Composer Studio
# File → Import → CCS Projects

# Build and debug
# Project → Build Project
# Run → Debug
```

## Repository Structure

```
embeddedSystemsLabs/
├── README.md                              # This file
├── assemblyLinkedListSearch/              # Lab 1
│   ├── ECE319K_Lab1.s
│   └── README.md
├── hardwareInterfacingLED+SwitchControl/  # Lab 2
│   ├── ECE319K_Lab2.s
│   ├── ECE319K_Lab2main.c
│   └── README.md
├── minimallyIntrusiveDebugging3/          # Lab 3
│   ├── Dump.c
│   ├── ECE319K_Lab3main.c
│   └── README.md
├── trafficLightControllerFSM/             # Lab 4
│   ├── ECE319K_Lab4main.c
│   └── README.md
├── soundGenerationDAC/                    # Lab 5
│   ├── Lab5.c
│   ├── Sound.c
│   └── README.md
├── lcdDeviceDriver/                       # Lab 6
│   ├── BusyWait.s
│   ├── StringConversion.s
│   ├── ST7735.c
│   └── README.md
├── analogPositionMeter/                   # Lab 7
│   ├── ADC1.c
│   ├── Lab7.c
│   └── README.md
├── distributedDataAcquisitionSystem/      # Lab 8
│   ├── FIFO1.c
│   ├── Lab8Transmitter.c
│   ├── Lab8Receiver.c
│   └── README.md
├── geometryDashGame/                      # Lab 9
│   ├── Lab9Main.c
│   ├── Sound.c
│   ├── Switch.c
│   └── README.md
└── inc/                                   # Shared includes
    ├── Clock.h
    ├── LaunchPad.h
    ├── Timer.h
    ├── UART.h
    ├── ST7735.h
    └── ...
```

## Learning Outcomes

Upon completing this laboratory sequence, students will be able to:

1. **Program ARM microcontrollers** at both assembly and C levels
2. **Interface with analog and digital peripherals** including ADC, DAC, GPIO
3. **Implement communication protocols** such as SPI and UART
4. **Design finite state machines** for embedded control applications
5. **Develop device drivers** for displays and sensors
6. **Create real-time systems** with timing constraints
7. **Debug embedded systems** using software instrumentation
8. **Integrate multiple subsystems** into complete applications

## Acknowledgments

- **Course Instructor:** Professor Jonathan Valvano
- **Institution:** University of Texas at Austin, Department of Electrical and Computer Engineering
- **Course:** ECE319K - Introduction to Embedded Systems

## References

- [MSPM0G3507 Datasheet](https://www.ti.com/product/MSPM0G3507)
- [ARM Cortex-M0+ Technical Reference Manual](https://developer.arm.com/documentation/ddi0484/latest/)
- [TI MSPM0 SDK Documentation](https://www.ti.com/tool/MSPM0-SDK)
- Course textbook and lecture materials

