# Distributed Data Acquisition System 

## Overview

This project implements a **distributed data acquisition system** using two TI MSPM0G3507 microcontrollers communicating via UART. One board acts as a transmitter (sampling analog data), while the other acts as a receiver (displaying measurements on an LCD). The system demonstrates real-time sensor data transmission with interrupt-driven communication.

## Authors

- Anmol Dubey

## System Architecture

```
┌─────────────────────┐         UART          ┌─────────────────────┐
│   TRANSMITTER       │  ───────────────────► │     RECEIVER        │
│                     │    2000 baud          │                     │
│  Slidepot → ADC     │    PA8 → PA22         │  FIFO → LCD Display │
│  TimerG12 @ 30Hz    │                       │  Real-time plotting │
└─────────────────────┘                       └─────────────────────┘
```

## Hardware Configuration

### Transmitter Board

| Component | Pin | Function |
|-----------|-----|----------|
| Slide Pot | PB18 | ADC1 Channel 5 input |
| UART1 Tx | PA8 | Serial data output |
| Debug LED (Green) | PB27 | ISR activity indicator |

### Receiver Board

| Component | Pin | Function |
|-----------|-----|----------|
| UART2 Rx | PA22 | Serial data input |
| LCD SCK | PB9 | SPI1 Clock |
| LCD MOSI | PB8 | SPI1 Data |
| LCD CS | PB6 | SPI1 Chip Select |
| Debug LED (Red) | PB26 | Receive indicator |
| Debug LED (Blue) | PB22 | UART ISR indicator |

### Loopback Testing (Single Board)

Connect PA8 (UART1 Tx) to PA22 (UART2 Rx) on the same board.

## Communication Protocol

### Message Format

Each transmission consists of 4 bytes:

| Byte | Content | Description |
|------|---------|-------------|
| 1 | `0x3C` ('<') | Start delimiter |
| 2 | ASCII '0'-'9' | Ones digit (cm) |
| 3 | ASCII '0'-'9' | Tenths digit |
| 4 | ASCII '0'-'9' | Hundredths digit |

Example: `<154` represents 1.54 cm

### UART Configuration

- Baud rate: 2000 bps
- Frame: 8 data bits, 1 stop bit, no parity
- Synchronization: Blind transmit, receiver timeout interrupt

## Key Components

### FIFO1.c - Software FIFO Buffer

- 64-byte circular buffer for receive data
- Thread-safe put/get operations
- Decouples ISR from main loop processing

### UART1.c - Transmitter Driver

- Blind (non-blocking) character output
- 5ms per frame at 2000 baud

### UART2.c - Receiver Driver

- Interrupt-driven reception with timeout
- Automatic transfer from hardware FIFO to software FIFO

### ADC1.c - Analog Input

- 12-bit ADC on channel 5 (PB18)
- Software-triggered single conversion
- Linear conversion to distance (0-2000 representing 0.000-2.000 cm)

## Timing

| Parameter | Value |
|-----------|-------|
| Sample rate | 30 Hz |
| Timer period | 2,666,666 cycles (80 MHz / 30) |
| Message rate | ~30 messages/second |
| Frame time | 5 ms (10 bits @ 2000 baud) |
| Message time | 20 ms (4 frames) |

## Test Programs

| Main Function | Purpose |
|---------------|---------|
| `main1` | FIFO verification against reference implementation |
| `main2` | Single frame transmission test (0x3C only) |
| `main3` | 4-frame message transmission test |
| `main4` | Loopback test with UART echo |
| `main` | Full system with ADC, LCD, and plotting |

## Distance Calculation

The transmitter converts ADC readings to fixed-point distance:

```c
position = ((2001 * Data + 25) >> 12);  // 0 to 2000
```

This maps the 12-bit ADC range (0-4095) to distance (0.000-2.000 cm).

## LCD Display

The receiver displays:
- Numeric distance value at top of screen
- Real-time scrolling plot of position history
- Plot range: 0 to 2000 (representing 0.000 to 2.000 cm)

## Building and Deployment

### Two-Board Configuration

1. Program both boards with the same code
2. Connect grounds between boards
3. Connect PA8 (Board 1) → PA22 (Board 2)
4. Connect PA8 (Board 2) → PA22 (Board 1)
5. Attach slidepot to PB18 on each board

### Single-Board Testing

1. Connect PA8 to PA22 on the same board
2. Use `main4` for loopback verification
3. Use `main` (main5) for full system test

## Debugging

- Green LED (PB27): Toggles in transmit ISR
- Red LED (PB26): Toggles on valid message receipt
- Blue LED (PB22): Toggles in UART2 receive ISR
- TExaS logic analyzer available on PB27/PB26
