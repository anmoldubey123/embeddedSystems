# Geometry Dash - Embedded Game Design 

## Overview

A side-scrolling rhythm-based platformer game inspired by Geometry Dash, implemented on the TI MSPM0G3507 microcontroller. Players navigate through obstacle courses while transforming between different movement modes (cube, ship, wave) via portals. Features bilingual support (English/Spanish), background music, and real-time graphics rendering on an ST7735 LCD.

## Authors

- Anmol Dubey

## Game Mechanics

### Player Modes

| Mode | Control | Physics |
|------|---------|---------|
| **Cube** | Button 1 to jump | Gravity-based jumping, single jump per ground contact |
| **Ship** | Hold Button 1 to rise | Continuous vertical control, gradual acceleration |
| **Wave** | Button 1 toggles direction | Diagonal movement with visual trail effect |

### Game Objects

| Object | Type ID | Description |
|--------|---------|-------------|
| Spike Up | 1 | Upward-facing obstacle (instant death) |
| Spike Down | 2 | Downward-facing obstacle |
| Spike Left | 3 | Left-facing obstacle |
| Spike Right | 4 | Right-facing obstacle |
| Cube Portal | 5 | Transforms player to cube mode |
| Ship Portal | 6 | Transforms player to ship mode |
| Wave Portal | 7 | Transforms player to wave mode |

### Game States

- **START**: Instructions screen with language selection
- **PLAYING**: Active gameplay with collision detection
- **GAMEOVER**: Death screen with restart option
- **LEVELCOMPLETE**: Level finished notification
- **VICTORY**: 100% completion celebration

## Hardware Configuration

### Input Devices

| Component | Pin | Function |
|-----------|-----|----------|
| Jump Button | PB17 | Player action (jump/rise/direction) |
| Menu Button | PB12 | Start game / Exit to menu |
| Slide Potentiometer | ADC1 | Language selection (English/Spanish) |

### Output Devices

| Component | Pin | Function |
|-----------|-----|----------|
| ST7735 LCD | SPI1 | 128×160 pixel game display |
| 5-bit DAC | PB4-PB0 | Audio output for background music |
| LEDs | GPIO | Status indicators |

### Display Layout

```
┌────────────────────────────┐
│ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ │ ← Top ground (y=0-10)
│      Progress: XX%          │
│                             │
│    ▲    ◇                   │
│   [■]  ▲ ▲    ║             │ ← Player, spikes, portals
│                             │
│                    B2:Exit  │
│ ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ │ ← Bottom ground (y=150-160)
└────────────────────────────┘
```

## Level Design

### Level 1 Structure (4500 units)

The level progresses through three distinct sections:

1. **Cube Section** (0-840): Basic jump obstacles with increasing difficulty
2. **Ship Section** (840-2500): Continuous spike corridors requiring precise vertical navigation
3. **Wave Section** (2500-4500): Diagonal spike patterns with ascending/descending challenges

### Object Placement

```
Distance    Content
--------    -------
150         Single spike
250-269     Double spike
360         Single spike
480-509     Double spike
600-619     Double spike
720         Single spike
840         Ship portal
900-1700    40 spike pairs (top/bottom corridor)
1700-2500   40 spike pairs (narrower corridor)
2500        Wave portal stack (4 portals)
2500-3500   Diagonal spike patterns
```

## Audio System

### Sound Implementation

- **Engine**: SysTick interrupt-driven DAC output
- **Waveform**: 32-sample square wave lookup table
- **Note System**: Period-based frequency control

### Music Tracks

| Track | Context | Characteristics |
|-------|---------|-----------------|
| Menu Song | Start screen | Slower tempo, 32 notes |
| Game Song | Active gameplay | Faster tempo, 32 notes |

### Note Periods (80 MHz clock)

| Note | Period | Frequency |
|------|--------|-----------|
| C4 | 9542 | 262 Hz |
| D4 | 8513 | 294 Hz |
| E4 | 7159 | 330 Hz |
| F4 | 6378 | 349 Hz |

## Sprite Dimensions

| Sprite | Width | Height |
|--------|-------|--------|
| Cube | 20 | 20 |
| Ship | 39 | 18 |
| Wave | 17 | 17 |
| Spike | 19 | 19 |
| Portal | 7 | 32 |

## Timing

| Parameter | Value | Description |
|-----------|-------|-------------|
| Game Loop | 30 Hz | TimerG12 interrupt |
| Sound Engine | Variable | SysTick based on note period |
| Scroll Speed | 4 pixels/frame | Object movement rate |
| Level Length | 4500 units | ~37.5 seconds at 30 Hz |

## Controls

### During Gameplay

- **Button 1 (PB17)**: Mode-dependent action
  - Cube: Jump
  - Ship: Ascend (hold)
  - Wave: Toggle upward movement
- **Button 2 (PB12)**: Return to menu

### Menu Navigation

- **Button 2**: Start game / Confirm selection
- **Slider**: Switch between English and Spanish

## Physics Constants

```c
const int16_t gravity = -1;      // Cube mode gravity
PlayerYvel (cube jump) = 10;     // Initial jump velocity
PlayerYvel (ship max) = ±4;      // Ship vertical speed limit
PlayerYvel (wave) = ±5;          // Wave diagonal speed
```

## Wave Trail Effect

The wave mode features a visual trail showing recent positions:

- Trail length: 8 positions
- Trail colors: Cyan (recent) → Blue (older)
- Size decreases with age for depth effect

## File Structure

| File | Purpose |
|------|---------|
| `Lab9Main.c` | Game engine, state machine, rendering |
| `Switch.c` | Button input handling |
| `Sound.c` | Audio playback via DAC |
| `LED.c` | LED control functions |
| `SmallFont.c` | Numeric display rendering |
| `images/images.h` | Sprite bitmap data |
| `sounds/sounds.h` | Audio sample data |

## Building and Running

1. Load project in Code Composer Studio
2. Connect MSPM0G3507 LaunchPad
3. Attach ST7735 LCD to SPI1 pins
4. Connect buttons to PB17 and PB12
5. Connect DAC circuit to PB4-PB0
6. Connect slide potentiometer to ADC1 input
7. Build and flash to device
8. Use slider to select language, press Button 2 to start

## Test Programs

| Main Function | Purpose |
|---------------|---------|
| `main0` | Arabic text rendering test |
| `main1` | Character set and language display test |
| `main2` | Small font and game over screen test |
| `main3` | Sound system test |
| `main` | Full game |
