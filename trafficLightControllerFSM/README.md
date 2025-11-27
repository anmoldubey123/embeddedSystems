# Traffic Light Controller FSM 

## Overview

This project implements a finite state machine (FSM) controller for a traffic intersection with two vehicle directions (North/South and East/West) and a pedestrian walk signal. The system runs on the TI MSPM0G3507 microcontroller and demonstrates Moore FSM design principles for real-time embedded control.

## Authors

- Anmol Dubey 

## Objectives

1. Design a Moore finite state machine for traffic control
2. Implement sensor-driven state transitions
3. Create safe traffic patterns with proper sequencing
4. Include pedestrian crossing with visual feedback

## Hardware Configuration

### LED Outputs (Active High)

| Signal | Pins | Colors |
|--------|------|--------|
| South Traffic | PB0, PB1, PB2 | Green, Yellow, Red |
| West Traffic | PB6, PB7, PB8 | Green, Yellow, Red |
| Walk Signal | PB22, PB26, PB27 | Blue, Red, Green |

### Sensor Inputs (Active High with Pull-down)

| Sensor | Pin | Function |
|--------|-----|----------|
| West Car | PB15 | Detects vehicles on west road |
| South Car | PB16 | Detects vehicles on south road |
| Walk Request | PB17 | Pedestrian crossing button |

### Pin Configuration

```
Outputs: IOMUX = 0x00000081 (GPIO function)
Inputs:  IOMUX = 0x00040081 (GPIO function + input enable)
```

## FSM Design

### State Diagram

```
                    ┌─────────────────────────────────────┐
                    │                                     │
                    ▼                                     │
              ┌─────────┐                                 │
              │   goS   │◄────────────────────────────────┤
              │ S:Green │                                 │
              │ W:Red   │                                 │
              └────┬────┘                                 │
                   │ (timeout or sensor)                  │
                   ▼                                      │
              ┌─────────┐                                 │
              │  waitS  │                                 │
              │S:Yellow │                                 │
              └────┬────┘                                 │
                   │                                      │
                   ▼                                      │
              ┌─────────┐                                 │
              │  redS   │                                 │
              │ All Red │                                 │
              └────┬────┘                                 │
                   │                                      │
          ┌────────┼────────┐                             │
          ▼        │        ▼                             │
     ┌────────┐    │   ┌────────┐                         │
     │  goW   │    │   │ goWalk │                         │
     │W:Green │    │   │Walk:On │                         │
     └───┬────┘    │   └───┬────┘                         │
         │         │       │                              │
         ▼         │       ▼                              │
    ┌────────┐     │   ┌────────┐                         │
    │ waitW  │     │   │ Flash  │ (4 states)              │
    │W:Yellow│     │   │sequence│                         │
    └───┬────┘     │   └───┬────┘                         │
         │         │       │                              │
         ▼         │       ▼                              │
    ┌────────┐     │   ┌────────┐                         │
    │  redW  │     │   │redWalk │                         │
    │All Red │     │   │All Red │                         │
    └───┬────┘     │   └───┬────┘                         │
         │         │       │                              │
         └─────────┴───────┴──────────────────────────────┘
```

### State Definitions

| State | ID | South | West | Walk | Time (×10ms) |
|-------|-----|-------|------|------|--------------|
| goS | 0 | Green | Red | Red | 300 (3.0s) |
| waitS | 1 | Yellow | Red | Red | 200 (2.0s) |
| redS | 2 | Red | Red | Red | 200 (2.0s) |
| goWalk | 3 | Red | Red | Green+Blue | 300 (3.0s) |
| Fon1 | 4 | Red | Red | Red | 100 (1.0s) |
| Foff1 | 5 | Red | Red | Off | 100 (1.0s) |
| Fon2 | 6 | Red | Red | Red | 100 (1.0s) |
| Foff2 | 7 | Red | Red | Off | 100 (1.0s) |
| redWalk | 8 | Red | Red | Red | 200 (2.0s) |
| goW | 9 | Red | Green | Red | 300 (3.0s) |
| waitW | 10 | Red | Yellow | Red | 200 (2.0s) |
| redW | 11 | Red | Red | Red | 200 (2.0s) |

### Input Encoding

| Input Value | Binary | Sensors Active |
|-------------|--------|----------------|
| 0 | 000 | None |
| 1 | 001 | West car |
| 2 | 010 | South car |
| 3 | 011 | West + South |
| 4 | 100 | Walk |
| 5 | 101 | Walk + West |
| 6 | 110 | Walk + South |
| 7 | 111 | All sensors |

## Data Structure

```c
struct state {
    uint32_t Out[3];    // [west, south, walk] LED values
    uint32_t Time;      // Wait time in 10ms units
    uint32_t Next[8];   // Next state for each input combination
};
typedef struct state State_t;

State_t FSM[12] = {
    // {outputs}, time, {next states for inputs 0-7}
    {{4,1,0x04000000}, 300, {goS, waitS, goS, waitS, waitS, waitS, waitS, waitS}},
    // ... 11 more states
};
```

### Output Encoding

**Traffic Lights (3-bit each):**
- Bit 0 (value 1): Green
- Bit 1 (value 2): Yellow
- Bit 2 (value 4): Red

**Walk Signal (32-bit packed):**
- Bit 22: Blue LED
- Bit 26: Red LED
- Bit 27: Green LED
- `0x0C400000`: Walk enabled (Green + Blue)
- `0x04000000`: Don't walk (Red only)
- `0x00000000`: Walk off (flashing)

## Key Functions

### Traffic_Init

Configures all GPIO pins for traffic light operation.

```c
void Traffic_Init(void);
```

**Operations:**
- Configure PB0-PB2 as outputs (South LEDs)
- Configure PB6-PB8 as outputs (West LEDs)
- Configure PB22, PB26, PB27 as outputs (Walk LEDs)
- Configure PB15-PB17 as inputs with pull-down (Sensors)
- Enable output drivers (DOE31_0)

### Traffic_Out

Sets all traffic light outputs simultaneously.

```c
void Traffic_Out(uint32_t west, uint32_t south, uint32_t walk);
```

**Parameters:**
- `west`: 3-bit value for west traffic LEDs (1=green, 2=yellow, 4=red)
- `south`: 3-bit value for south traffic LEDs
- `walk`: 32-bit value with walk LED bits already positioned

**Implementation (Friendly):**
```c
GPIOB->DOUT31_0 = (GPIOB->DOUT31_0 & 0xF3BFFE38);  // Clear LED bits
GPIOB->DOUT31_0 = (GPIOB->DOUT31_0 | south);       // Set south
GPIOB->DOUT31_0 = (GPIOB->DOUT31_0 | (west << 6)); // Set west
GPIOB->DOUT31_0 = (GPIOB->DOUT31_0 | walk);        // Set walk
```

### Traffic_In

Reads all sensor inputs.

```c
uint32_t Traffic_In(void);
```

**Returns:** 3-bit packed value
- Bit 0: West car sensor (PB15)
- Bit 1: South car sensor (PB16)
- Bit 2: Walk button (PB17)

## FSM Engine

```c
currentState = 0;  // Start in goS state
Traffic_Out(FSM[currentState].Out[0], 
            FSM[currentState].Out[1], 
            FSM[currentState].Out[2]);

while(1) {
    // 1. Output based on current state
    Traffic_Out(FSM[currentState].Out[0], 
                FSM[currentState].Out[1], 
                FSM[currentState].Out[2]);
    
    // 2. Wait for state duration
    SysTick_Wait10ms(FSM[currentState].Time);
    
    // 3. Read sensor inputs
    uint32_t input = Traffic_In();
    
    // 4. Transition to next state
    currentState = FSM[currentState].Next[input];
}
```

## Test Programs

### main1 - Assignment Display

Displays lab assignment information via grader.

### main2 - LED Output Test

Tests all traffic light combinations:

```
Sequence:
1. South Green, West Red
2. South Yellow, West Red
3. South Red, West Green
4. South Red, West Yellow
5. Walk Red On
6. Walk Off
```

### main3 - Switch Input Test

Monitors sensor inputs and displays changes via UART:

```
Switch= 0x1
West switch pressed

Switch= 0x3
West switch pressed
South switch pressed
```

### main4 - FSM Cycle Test

Runs FSM with all inputs forced high (input = 7) to verify state transitions.

### main5 - Full System

Complete traffic controller with live sensor input.

## Debug Output Format

The debug dump encodes state information:

```
Bits 31-24: Current state number
Bits 23-16: West LED status (shifted)
Bits 15-8:  South LED status (shifted)
Bits 2-0:   Walk LED status (compressed)
```

## Safety Features

1. **All-Red States**: Transition states (redS, redWalk, redW) ensure both directions are red before changing
2. **Minimum Times**: Each state has enforced minimum duration
3. **Walk Flash Warning**: 4-state flashing sequence warns pedestrians before ending walk phase
4. **Priority Logic**: Walk requests processed safely with proper vehicle stopping sequence

## Timing Summary

| Phase | Duration |
|-------|----------|
| Green light | 3.0 seconds |
| Yellow light | 2.0 seconds |
| All-red transition | 2.0 seconds |
| Walk signal | 3.0 seconds |
| Flash on/off | 1.0 second each |
| Total flash sequence | 4.0 seconds |

## File Structure

| File | Purpose |
|------|---------|
| `ECE319K_Lab4main.c` | FSM implementation and test programs |
| `Dump.c` | Debug instrumentation (from Lab 3) |
| `Timer.c` | SysTick timing functions |
| `LaunchPad.c` | Board initialization |

## Building and Testing

1. **Run main1**: Verify assignment details
2. **Run main2**: Test all LED combinations visually
3. **Run main3**: Test sensor inputs with UART feedback
4. **Run main4**: Verify FSM transitions (forced input)
5. **Run main5**: Full system test with real sensors

## Hardware Notes

### Friendly Register Access

All GPIO operations use read-modify-write to preserve other bits:

```c
// Correct (friendly):
GPIOB->DOE31_0 = GPIOB->DOE31_0 | (1<<0);

// Incorrect (not friendly):
GPIOB->DOE31_0 = (1<<0);  // Overwrites other bits
```

### Bit Mask Reference

```
South LEDs:  0x00000007 (PB2-PB0)
West LEDs:   0x000001C0 (PB8-PB6)
Walk LEDs:   0x0C400000 (PB27, PB26, PB22)
Clear Mask:  0xF3BFFE38 (preserves non-LED bits)
```
