# Hardware Interfacing: LED & Switch Control 

## Overview

This project implements precise LED brightness control through software-generated pulse-width modulation (PWM) on the TI MSPM0G3507 microcontroller. Written entirely in ARM Cortex-M0+ assembly language, the lab demonstrates direct hardware register manipulation, timing loops, and GPIO configuration for embedded systems.

## Author

- Anmol Dubey 

## Objective

Generate four distinct LED brightness levels by cycling through different duty cycles at a fixed 50 Hz frequency (20 ms period). The duty cycles create a visible "breathing" or ramping brightness effect.

## Hardware Configuration

### GPIO Assignments

| Component | Pin | Direction | Function |
|-----------|-----|-----------|----------|
| LED | PB18 | Output | PWM-controlled brightness |
| Change Switch | PB0/PB1/PB2/PB3 | Input | Mode selection (EID-dependent) |

The specific pins are randomly assigned based on the student's EID, requiring flexible implementation.

### Pin Configuration

```
LED Output:     IOMUX = 0x00000081 (GPIO function, no pull)
Switch Input:   IOMUX = 0x00040081 (GPIO function, pull-up enabled)
DOE31_0:        Bit set for LED pin to enable output
```

## PWM Timing Specification

### Operating Parameters

- **Frequency**: 50 Hz (20 ms period)
- **Clock**: 80 MHz bus clock
- **Tolerance**: ±3% duty cycle accuracy

### Duty Cycle Pattern

The LED cycles through four brightness levels continuously:

| Cycle | High Time | Low Time | Duty Cycle |
|-------|-----------|----------|------------|
| 1 | 2 ms | 18 ms | 10% |
| 2 | 6 ms | 14 ms | 30% |
| 3 | 8 ms | 12 ms | 40% |
| 4 | 14 ms | 6 ms | 70% |

*Note: Actual duty cycles vary based on EID-generated random assignment (Mode2 parameter).*

## Timing Calculations

At 80 MHz, each clock cycle is 12.5 ns. The delay loop executes 4 cycles per iteration:

```
Delay cycles = (Time in seconds) × (80,000,000 Hz)
             = Time_ms × 80,000

Examples:
  2 ms  →  160,000 cycles
  6 ms  →  480,000 cycles
  8 ms  →  640,000 cycles
 12 ms  →  960,000 cycles
 14 ms → 1,120,000 cycles
 18 ms → 1,440,000 cycles
```

## Assembly Implementation

### Key Functions

| Function | Purpose |
|----------|---------|
| `Lab2Init` | Configure GPIO pins for LED output and switch input |
| `turnOn` | Set LED pin high using DOUT31_0 register |
| `turnOff` | Clear LED pin low using DOUT31_0 register |
| `Delay` | Software delay loop with cycle-accurate timing |
| `Dump` | Debug function to log state for analysis |

### Register Usage

| Register | Purpose |
|----------|---------|
| R0 | Delay count / Function parameters |
| R1 | GPIO register address |
| R2 | Current register value |
| R3 | Bit mask for LED pin |
| R4 | Cycle counter (0-3) |
| LR | Link register for subroutine returns |

### Main Loop Structure

```
loop:
    Cycle 1: LED ON (2ms) → LED OFF (18ms)
    Cycle 2: LED ON (6ms) → LED OFF (14ms)
    Cycle 3: LED ON (8ms) → LED OFF (12ms)
    Cycle 4: LED ON (14ms) → LED OFF (6ms)
    Repeat...
```

## Debugging Features

### Debug Modes (Phase Parameter)

| Phase | Mode | Description |
|-------|------|-------------|
| 0 | Info | Display problem assignment and exit |
| 1 | Logic Analyzer | Stream GPIO states via UART at 10 kHz |
| 2 | Oscilloscope | Stream ADC samples from PB20 at 10 kHz |
| 10 | Grader | Automated duty cycle verification |

### Dump Function

The `Dump` function is called before each LED state change to record timing data for the logic analyzer, enabling verification of PWM timing accuracy.

## Grading System

The automated grader samples the LED output at 10 kHz and measures:

1. **Period**: Must be 150-250 samples (15-25 ms at 10 kHz sampling)
2. **Duty Cycle**: Must match expected values within ±3%
3. **All Four Cycles**: Each duty cycle must be detected for full credit

### Scoring

- 4 points per correctly detected duty cycle
- 25 points maximum (perfect score)

## Building and Testing

### Hardware Setup

1. Connect LED with current-limiting resistor to assigned PB pin
2. Connect momentary switch to assigned PB pin (active low)
3. Insert jumpers J25 and J26 for UART debugging
4. Optionally connect oscilloscope probe to PB20 for analog monitoring

### Debug Procedure

1. Run with Phase=0 to see your specific pin assignments
2. Run with Phase=1 to verify timing with logic analyzer
3. Run with Phase=2 to view analog waveform
4. Run with Phase=10 for automated grading

### Expected Console Output

```
ECE319K Lab 2, Spring 2025
Student EID= AD56328
Implement the Change switch using PB2
Implement the LED on PB18
Implement duty cycles 10,30,40,70%,
Running the grader
10,30,40,70, Perfect, Score=25
End of Lab 2, Spring 2025
```

## Technical Notes

### Delay Loop Analysis

```assembly
Delay:
    SUBS R0, R0, #2     ; 1 cycle, initial offset compensation
dloop:
    SUBS R0, R0, #4     ; 1 cycle
    NOP                  ; 1 cycle
    BHS dloop           ; 2 cycles (taken) / 1 cycle (not taken)
    BX LR               ; Return
```

Each loop iteration: 4 cycles = 50 ns at 80 MHz

### GPIO Bit Manipulation

```assembly
; Turn ON using OR operation (preserves other bits)
LDR R3, =(1<<18)
ORRS R2, R2, R3

; Turn OFF using BIC operation (preserves other bits)
BICS R2, R2, R3
```

## File Structure

| File | Purpose |
|------|---------|
| `ECE319K_Lab2.s` | Main assembly implementation |
| `ECE319K_Lab2main.c` | C framework with grader and debug tools |
| `msp.s` | Include file with register definitions |
