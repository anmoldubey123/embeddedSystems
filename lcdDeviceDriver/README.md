# LCD Device Driver 

## Overview

This project implements a complete device driver for the ST7735 TFT LCD display on the TI MSPM0G3507 microcontroller. The lab combines ARM assembly language routines for low-level SPI communication with C functions for high-level graphics operations, including character rendering, bitmap display, and data plotting.

## Authors

- Anmol Dubey 

## Objectives

1. Implement SPI busy-wait communication routines in assembly
2. Create integer-to-string conversion without division hardware
3. Interface with the ST7735 128×160 pixel color LCD
4. Display formatted numerical data with fixed-point representation

## Hardware Configuration

### ST7735 LCD Connection

| LCD Pin | Function | MCU Pin | Description |
|---------|----------|---------|-------------|
| SCK | SPI Clock | PB9 | SPI1_SCLK |
| MOSI | SPI Data | PB8 | SPI1_PICO |
| CS | Chip Select | PB6 | SPI1_CS0 |
| DC | Data/Command | PA13 | GPIO (High=Data, Low=Command) |
| RST | Reset | PB15 | GPIO (Active Low) |
| VCC | Power | 3.3V | |
| GND | Ground | GND | |
| LED | Backlight | 3.3V | |

### UART Debug Interface

| Signal | Pin | Function |
|--------|-----|----------|
| UART0 Tx | PA10 | Debug output to PC |
| UART0 Rx | PA11 | Debug input from PC |

Requires jumpers J25 and J26 installed for XDS110 UART connection.

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                      Application Layer                       │
│         (Lab6Main.c - Test programs, printf support)        │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                    String Conversion Layer                   │
│            (StringConversion.s - Dec2String, udivby10)      │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                      Graphics Layer                          │
│    (ST7735.c - Characters, bitmaps, shapes, plotting)       │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                       SPI Driver Layer                       │
│       (SPI.c + BusyWait.s - Hardware communication)         │
└─────────────────────────────────────────────────────────────┘
                              │
┌─────────────────────────────────────────────────────────────┐
│                        Hardware                              │
│              (ST7735 LCD, SPI1, GPIO PA13/PB15)             │
└─────────────────────────────────────────────────────────────┘
```

## Key Components

### Assembly Routines (BusyWait.s)

#### SPIOutCommand

Sends an 8-bit command to the LCD with proper D/C signal handling.

```
Algorithm:
1. Wait for SPI BUSY bit (bit 4) to clear
2. Clear PA13 (D/C = 0 for command)
3. Write command to SPI TXDATA
4. Wait for SPI BUSY bit to clear
```

#### SPIOutData

Sends 8-bit data to the LCD.

```
Algorithm:
1. Wait for TNF bit (bit 1) to be high (TX FIFO not full)
2. Set PA13 (D/C = 1 for data)
3. Write data to SPI TXDATA
```

### Integer Division (StringConversion.s)

#### udivby10

Performs division by 10 without hardware divide instruction using multiplication by reciprocal.

```
Formula: quotient = (n × 52429) >> 19
         remainder = n - (quotient × 10)

Example: 43 ÷ 10
  43 × 52429 = 2,254,447
  2,254,447 >> 19 = 4 (quotient)
  43 - (4 × 10) = 3 (remainder)
```

#### Dec2String / OutDec

Converts 16-bit unsigned integer to ASCII string using stack-based digit extraction.

```
Process:
1. Repeatedly divide by 10, storing remainders on stack
2. Add 0x30 ('0') to convert each digit to ASCII
3. Pop digits in reverse order to output string
```

### Display Specifications

| Parameter | Value |
|-----------|-------|
| Resolution | 128 × 160 pixels |
| Color Depth | 16-bit (RGB565) |
| Text Grid | 21 columns × 16 rows |
| Character Size | 6 × 10 pixels (with spacing) |
| SPI Clock | 8 MHz |

## Test Programs

| Main | Purpose | LCD Required |
|------|---------|--------------|
| main0 | Test udivby10 division routine | No |
| main1 | Grade SPIOutCommand, SPIOutData, Dec2String | No |
| main2 | Test OutDec via UART output | No |
| main (main3) | Full LCD demonstration with graphics | Yes |
| main4 | Oscilloscope timing measurement | Yes |

## Graphics Functions

### Text Output

| Function | Description |
|----------|-------------|
| `ST7735_DrawChar()` | Single character with foreground/background colors |
| `ST7735_DrawString()` | Null-terminated string at cursor position |
| `ST7735_OutString()` | String output with automatic cursor advance |
| `ST7735_SetCursor()` | Position cursor (0-20 columns, 0-15 rows) |
| `ST7735_SetTextColor()` | Set text foreground color |

### Graphics Primitives

| Function | Description |
|----------|-------------|
| `ST7735_DrawPixel()` | Single pixel |
| `ST7735_FillRect()` | Filled rectangle |
| `ST7735_DrawFastVLine()` | Vertical line |
| `ST7735_DrawFastHLine()` | Horizontal line |
| `ST7735_Line()` | Arbitrary line (Bresenham's algorithm) |
| `ST7735_DrawBitmap()` | 16-bit color bitmap image |
| `ST7735_DrawCircle()` | 10-pixel diameter circle |

### Data Plotting

| Function | Description |
|----------|-------------|
| `ST7735_PlotClear()` | Initialize plot area with Y-axis range |
| `ST7735_PlotPoint()` | Plot single data point |
| `ST7735_PlotLine()` | Plot connected line segment |
| `ST7735_PlotNext()` | Advance X coordinate |
| `ST7735_Drawaxes()` | Draw labeled axes for charts |

## Color Definitions

```c
#define ST7735_BLACK   0x0000
#define ST7735_BLUE    0x001F
#define ST7735_RED     0xF800
#define ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW  0xFFE0
#define ST7735_WHITE   0xFFFF
```

RGB565 format: `color = ((R & 0x1F) << 11) | ((G & 0x3F) << 5) | (B & 0x1F)`

## SPI Status Register

| Bit | Name | Description |
|-----|------|-------------|
| 4 | BUSY | SPI currently transmitting |
| 3 | RNF | Receive FIFO not full |
| 2 | RFE | Receive FIFO empty |
| 1 | TNF | Transmit FIFO not full |
| 0 | TFE | Transmit FIFO empty |

## Building and Testing

### Test Sequence

1. **Division Test**: Run main0 and verify at breakpoints:
   - Input 43: quotient=4, remainder=3
   - Input 65535: quotient=6553, remainder=5
   - Input 0: quotient=0, remainder=0

2. **Grader Test**: Run main1 with Lab6Grader(4) for automated verification

3. **UART Test**: Run main2 to verify string conversion output matches expected values

4. **LCD Demo**: Run main (main3) to see full graphical demonstration

### Expected Output (main2 via UART)

```
Test of OutDec
Your: 0 Correct: 0
Your: 7 Correct: 7
Your: 99 Correct: 99
Your: 100 Correct: 100
Your: 654 Correct: 654
...
Your: 65535 Correct: 65535
```

## File Structure

| File | Purpose |
|------|---------|
| `BusyWait.s` | Assembly SPI communication routines |
| `StringConversion.s` | Assembly number-to-string conversion |
| `ST7735.c` | LCD graphics driver library |
| `SPI.c` | SPI peripheral initialization and C wrappers |
| `Lab6Main.c` | Test programs and demonstrations |
| `Clock.c` | System clock configuration (80 MHz) |
| `Timer.c` | Hardware timer functions |
| `UART.c` / `UARTbusywait.c` | Serial debug interface |
| `LaunchPad.c` | Board initialization |
| `images.h` | Bitmap image data |

## Technical Notes

### AAPCS Compliance

The assembly routines preserve registers R4-R7 per ARM Architecture Procedure Call Standard. The `udivby10` function is non-compliant as it returns two values (quotient in R0, remainder in R1).

### Timing Considerations

At 80 MHz bus clock with 8 MHz SPI:
- Single character draw: ~40 pixels × 2 bytes = 80 SPI transfers
- Full screen clear: 128 × 160 × 2 = 40,960 bytes ≈ 5 ms

### Extended ASCII Support

The font table includes characters 0-255 with extended ASCII for international characters (ñ, ü, etc.) and box-drawing characters.

