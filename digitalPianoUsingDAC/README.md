# Digital Piano Using DAC

## Overview

This project implements a **digital piano** on the TI MSPM0G3507 microcontroller. It uses a 5-bit binary-weighted DAC to generate audio waveforms and a 4-key keyboard interface to select musical notes.

## Authors

- Anmol Dubey

## Hardware Configuration

### 5-Bit DAC (PB4-PB0)

| Pin | Bit Weight | Resistor Value |
|-----|------------|----------------|
| PB4 | MSB (16) | R |
| PB3 | 8 | 2R |
| PB2 | 4 | 4R |
| PB1 | 2 | 8R |
| PB0 | LSB (1) | 16R |

Output voltage range: 0V to 3.3V in 32 steps (approximately 103mV per step)

### 4-Key Keyboard (PB19-PB16)

| Pin | Key | Note Frequency | Period (cycles) |
|-----|-----|----------------|-----------------|
| PB16 | Key 0 | C4 (262 Hz) | 10726 |
| PB17 | Key 1 | D4 (294 Hz) | 8513 |
| PB18 | Key 2 | E4 (330 Hz) | 7159 |
| PB19 | Key 3 | F4 (349 Hz) | 6378 |

## System Architecture

### Signal Flow

1. Keyboard input scanned via GPIO (PB19-PB16)
2. Key press triggers SysTick timer with note-specific period
3. SysTick ISR outputs sine wave samples to DAC
4. DAC converts digital samples to analog voltage
5. Analog output drives speaker/audio circuit

### Waveform Generation

The system uses a 32-sample sine wave lookup table:

```c
const uint8_t SinWave[32] = {
    16,19,22,24,27,28,30,31,31,31,30,28,27,24,22,19,
    16,13,10,8,5,4,2,1,1,1,2,4,5,8,10,13
};
```

Output frequency = 80 MHz / (period × 32 samples)

## Key Functions

### DAC5.c

- `DAC5_Init()` - Configures PB4-PB0 as GPIO outputs
- `DAC5_Out(data)` - Outputs 5-bit value using friendly read-modify-write

### Key.c

- `Key_Init()` - Configures PB19-PB16 as inputs
- `Key_In()` - Returns 4-bit key state

### Sound Generation

- `Sound_Init(period, priority)` - Initializes SysTick timer
- `Sound_Start(period)` - Begins waveform output at specified frequency
- `Sound_Stop()` - Halts audio output
- `SysTick_Handler()` - ISR that outputs next sine sample

## Chord Support

The system supports simultaneous key presses by averaging periods:

| Keys Pressed | Resulting Period |
|--------------|------------------|
| Single key | Direct period value |
| Two keys | Average of both periods |
| Three keys | Average of three periods |
| All four keys | Average of all four periods |

## Test Programs

| Main Function | Purpose |
|---------------|---------|
| `main1` | Display lab assignment |
| `main2a` | Static DAC testing with voltmeter |
| `main2b` | Static DAC testing with TExaS scope |
| `main3` | Dynamic DAC ramp test (32ms period) |
| `main4` | Keyboard switch debugging via UART |
| `main5` | Continuous tone test |
| `main` | Full piano system |

## Building and Testing

1. Connect DAC output (summing junction) to PB20 for scope verification
2. Connect speaker or audio amplifier to DAC output
3. Build with TI-CLANG in Code Composer Studio
4. Use TExaSdisplay for waveform visualization
5. UART output (115200 baud) available for debugging

## Timing Calculations

At 80 MHz bus clock with 32 samples per period:

- Note frequency = 80,000,000 / (SysTick_period × 32)
- For middle C (262 Hz): period = 80,000,000 / (262 × 32) ≈ 9542
