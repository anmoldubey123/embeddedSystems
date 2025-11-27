# Minimally Intrusive Debugging 

## Overview

This project implements a software-based debugging instrument for the TI MSPM0G3507 microcontroller. The debugging system captures timestamped data samples with minimal impact on program execution, enabling period measurement and waveform analysis without expensive logic analyzer hardware.

## Author

- Anmol Dubey 

## Objectives

1. Create a non-intrusive data recording system using circular buffers
2. Implement high-resolution timing using hardware Timer G12
3. Calculate signal period from recorded edge transitions
4. Demonstrate debugging techniques for embedded systems

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Application Code                          │
│              (Main program being debugged)                   │
└─────────────────────────────────────────────────────────────┘
                              │
                        Debug_Dump() calls
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    Debug Instrument                          │
│         DataBuffer[50]     │     TimeBuffer[50]             │
│         [data values]      │     [timestamps]               │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                     Timer G12                                │
│            32-bit free-running counter                       │
│            Resolution: 12.5 ns @ 80 MHz                      │
└─────────────────────────────────────────────────────────────┘
```

## Hardware Configuration

### Timer G12

| Parameter | Value |
|-----------|-------|
| Clock Source | Bus Clock (80 MHz) |
| Resolution | 12.5 ns per count |
| Counter Width | 32 bits |
| Mode | Free-running, counting down |
| Range | ~53.7 seconds before rollover |

### UART Debug Output

| Signal | Pin | Baud Rate |
|--------|-----|-----------|
| UART0 Tx | PA10 | 115200 |
| UART0 Rx | PA11 | 115200 |

Requires jumpers J25 and J26 for XDS110 connection.

## Debug Functions

### Debug_Init

Initializes the debugging system.

```c
void Debug_Init(void);
```

**Operations:**
- Clears buffer index (DebugCnt = 0)
- Initializes Timer G12 for timestamp capture

### Debug_Dump

Records a single data point with timestamp.

```c
uint32_t Debug_Dump(uint32_t data);
```

**Parameters:**
- `data`: 32-bit value to record

**Returns:**
- 1: Success (data recorded)
- 0: Failure (buffer full)

**Implementation:**
```c
if(DebugCnt >= MAXBUF) return 0;
DataBuffer[DebugCnt] = data;
TimeBuffer[DebugCnt] = TIMG12->COUNTERREGS.CTR;
DebugCnt++;
return 1;
```

### Debug_Dump2

Records data only when it changes from the previous value.

```c
uint32_t Debug_Dump2(uint32_t data);
```

**Behavior:**
- Always records first call
- Subsequent calls only record if data differs from previous
- Reduces buffer usage for slowly-changing signals

### Debug_Period

Calculates signal period from recorded data.

```c
uint32_t Debug_Period(uint32_t mask);
```

**Parameters:**
- `mask`: Bit mask to isolate signal of interest

**Returns:**
- Average period in bus cycles (12.5 ns each)
- 0 if insufficient data

**Algorithm:**
1. Apply mask to each data sample
2. Detect rising edges (low→high transitions)
3. Calculate time difference between consecutive rising edges
4. Return average period across all detected edges

### Debug_Duty

Calculates duty cycle percentage.

```c
uint32_t Debug_Duty(uint32_t mask);
```

**Returns:**
- Duty cycle as integer percentage (0-100)

## Buffer Specification

| Parameter | Value |
|-----------|-------|
| Buffer Size | 50 samples |
| Data Width | 32 bits |
| Time Resolution | 12.5 ns |
| Maximum Recording | 50 data/time pairs |

## Test Programs

### main1 - Basic Timing Test

Verifies timestamp accuracy with known delays.

```
Test 1: 1 second delay
  DataBuffer[0] = 2, DataBuffer[1] = 3
  Time difference ≈ 80,000,000 cycles (1 second)

Test 2: 0.1 second delay
  DataBuffer[2] = 4, DataBuffer[3] = 5
  Time difference ≈ 8,000,000 cycles (100 ms)
```

### main2 - Period Measurement (16 samples)

Records 16 samples with 1 ms spacing, tests period calculation.

| Mask | Expected Period | Description |
|------|-----------------|-------------|
| 1 (bit 0) | 160,000 cycles | 2 ms period |
| 2 (bit 1) | 320,000 cycles | 4 ms period |
| 4 (bit 2) | 640,000 cycles | 8 ms period |
| 7 (bits 0-2) | 640,000 cycles | Combined mask |
| 8 (bit 3) | 0 | No edges detected |

### main3 - Buffer Overflow Test

Records 80 samples (exceeds 50-sample buffer), verifies overflow handling.

| Mask | Expected Period | Description |
|------|-----------------|-------------|
| 1 | 80,000 cycles | 1 ms period |
| 2 | 160,000 cycles | 2 ms period |
| 4 | 320,000 cycles | 4 ms period |

### main4 - Execution Time Measurement

Measures Debug_Dump execution overhead using SysTick.

**Target:** < 300 bus cycles per call

### main5 - Grader

Automated grading via Lab3Grader function.

## Application Example: Traffic Light Debugging

The `Dump()` function demonstrates capturing multiple GPIO states:

```c
void Dump(void) {
    uint32_t southLEDs = (GPIOB->DOUT31_0 & 0x07);        // PB0-PB2
    uint32_t westLEDs = ((GPIOB->DOUT31_0 >> 6) & 0x07);  // PB6-PB8
    uint32_t walkLEDs = ((GPIOB->DOUT31_0 >> 22) & 0x07); // PB22,26,27
    
    uint32_t data = (southLEDs | (westLEDs << 6) | (walkLEDs << 22));
    
    if(Debug_Dump(data) == 0) {
        Theperiod = Debug_Period(1<<18);
        __asm volatile("bkpt");  // Break when buffer full
    }
}
```

## Period Calculation Details

For a signal observed through a mask:

```
Rising Edge Detection:
  Previous sample: (DataBuffer[i-1] & mask) == 0  (LOW)
  Current sample:  (DataBuffer[i] & mask) != 0    (HIGH)

Period = TimeBuffer[edge1] - TimeBuffer[edge2]
         (Timer counts DOWN, so earlier time is larger)

Average Period = Σ(individual periods) / (number of edges - 1)
```

## Performance Characteristics

| Metric | Value |
|--------|-------|
| Debug_Dump execution | ~87 cycles (typical) |
| Debug_Dump overhead | ~1 μs @ 80 MHz |
| Minimum sample interval | ~1 μs |
| Maximum recording duration | 50 × sample_interval |

## Intrusion Analysis

The debugging system is "minimally intrusive" because:

1. **Low overhead**: Each Debug_Dump call adds only ~87 cycles
2. **No polling**: Uses hardware timer (no software timing loops)
3. **Bounded execution**: Constant time regardless of buffer state
4. **No interrupts disabled**: Does not affect interrupt latency

## Usage Guidelines

### When to Use Debug_Dump

- Capture every sample for high-frequency analysis
- When data changes frequently
- For precise timing measurements

### When to Use Debug_Dump2

- Monitoring state machines (only record transitions)
- Extending effective recording duration
- Reducing post-processing data volume

### Choosing Mask Values

| Scenario | Mask |
|----------|------|
| Single bit analysis | `(1 << bit_number)` |
| Multiple related bits | OR of individual masks |
| Full port capture | `0xFFFFFFFF` |

## File Structure

| File | Purpose |
|------|---------|
| `Dump.c` | Debug instrument implementation |
| `Dump.h` | Function prototypes |
| `ECE319K_Lab3main.c` | Test programs (main1-main5) |
| `Timer.c` | Timer G12 initialization |

## Building and Testing

1. **Run main1**: Verify basic timing accuracy
2. **Run main2**: Verify period calculation with 16 samples
3. **Run main3**: Verify buffer overflow handling
4. **Run main4**: Measure execution time (should be < 300 cycles)
5. **Run main5**: Execute automated grader

## Expected UART Output (main2)

```
Lab 3, Spring 2025, Step 2.
EID= AD56328
Period0= 160000 cycles, should be about 160000 (2ms)
Period1= 320000 cycles, should be about 320000 (4ms)
Period2= 640000 cycles, should be about 640000 (8ms)
Period7= 640000 cycles, should be about 640000 (8ms)
Period8= 0 cycles, should be exactly 0 (no edges)
```

