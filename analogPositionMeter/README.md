# Analog Position Meter 

## Overview

This project implements a real-time analog position measurement system using the **TI MSPM0G3507** microcontroller. It reads position data from a slide potentiometer via ADC, converts the raw values to distance measurements, and displays results on an ST7735 LCD.

## Authors

- Anmol Dubey

## Hardware Requirements

- TI MSPM0G3507 LaunchPad
- ST7735 128x160 TFT LCD display
- Slide potentiometer (connected to PB18)
- Optional: Logic analyzer for debugging

## Pin Connections

| Component | Pin | Function |
|-----------|-----|----------|
| Slide Pot | PB18 | ADC1 Channel 5 input |
| LCD SCK | PB9 | SPI1 Clock |
| LCD MOSI | PB8 | SPI1 Data |
| LCD CS | PB6 | SPI1 Chip Select |
| LCD DC | PA13 | Data/Command |
| LCD RST | PB15 | Reset |
| Debug LED (Red) | PB26 | Status indicator |
| Debug LED (Green) | PB27 | ISR toggle |

## System Architecture

The system operates at **80 MHz** using the internal PLL and samples the ADC at **30 Hz** via TimerG12 interrupts.

### Data Flow

1. Timer interrupt triggers ADC sample
2. 12-bit ADC value (0-4095) stored in mailbox
3. Semaphore signals main loop
4. `Convert()` transforms ADC data to position (0-2000, representing 0.000 to 2.000 cm)
5. `OutFix()` formats and displays the distance on LCD
6. Position plotted on scrolling graph

## Key Functions

### ADC1.c

- `ADCinit()` - Configures ADC1 channel 5 for 12-bit single conversion
- `ADCin()` - Performs software-triggered ADC sample
- `Convert()` - Linear transformation: `Position = (2000 * Data) >> 12`

### Lab7Main.c

- `OutFix()` - Displays fixed-point distance as `X.XXX cm`
- `TIMG12_IRQHandler()` - 30 Hz interrupt service routine for sampling
- `main()` - Primary system loop with LCD updates and plotting

## Test Programs

| Main Function | Purpose |
|---------------|---------|
| `main1` | TExaS voltmeter verification |
| `main2` | ADC calibration and timing measurement |
| `main3` | Conversion function validation |
| `main4` | LCD output and timing analysis |
| `main` | Full system operation |
| `main6` | Central Limit Theorem demonstration |

## Calibration

The system uses a linear calibration model. To calibrate:

1. Run `main2` to collect ADC values at known positions
2. Record 5 data points in `Calibration.xls`
3. Determine constants k1, k2 for: `Position = (k1 * Data + k2) >> 12`
4. Update the `Convert()` function accordingly

## Performance Metrics

The code includes timing measurements for:

- `ADCtime` - ADC conversion duration
- `Converttime` - Integer conversion cycles
- `FloatConverttime` - Floating-point conversion cycles (demonstrates Cortex-M0+ FP overhead)
- `OutFixtime` - Display output duration

## Building and Flashing

This project uses TI's MSPM0 SDK with the TI-CLANG toolchain. Import into Code Composer Studio and build for the LP-MSPM0G3507 target.
