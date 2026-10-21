# Arduino Digital Clock with Alarm

An Arduino-based digital clock and alarm system built using an Arduino Uno R3, a DS1302 Real-Time Clock (RTC) module, and a four-digit seven-segment display.

This project explores embedded C/C++ programming, RTC communication, display multiplexing, button handling, and alarm control.

## Project Preview

![Arduino Digital Clock with Alarm](images/completed_clock.jpg)

## Features

- Real-time timekeeping using a DS1302 RTC module.
- Four-digit seven-segment display with multiplexing.
- Hour and minute adjustment using two push buttons.
- Configurable alarm with a default time of 06:30.
- Alarm-setting mode activated by holding both buttons.
- Audible confirmation when switching modes.
- Automatic alarm shutoff after 10 seconds.
- Blinking decimal point for visual feedback.

## Hardware Components

| Component | Purpose |
|---|---|
| Arduino Uno R3 | Main controller |
| DS1302 RTC module | Timekeeping |
| Four-digit seven-segment display | Displays the time |
| BC547 transistors | Digit switching |
| Push buttons × 2 | Time and alarm adjustment |
| Buzzer | Audible alarm |
| 220 Ω resistors | Segment current limiting |
| Breadboard and jumper wires | Circuit assembly |

**Note:** Confirm the display type and transistor arrangement against the final working circuit before reproducing the hardware.

## Pin Configuration

### RTC Module

| RTC Signal | Arduino Pin |
|---|---|
| DAT | D4 |
| CLK | D5 |
| RST | D3 |
| VCC | 5V |
| GND | GND |

### Seven-Segment Display

| Segment | Arduino Pin |
|---|---|
| A | D6 |
| B | D7 |
| C | D8 |
| D | D9 |
| E | D10 |
| F | D11 |
| G | D12 |
| DP | D13 |

### Digit Controls

| Display Position | Arduino Pin |
|---|---|
| First digit | D2 |
| Second digit | A0 |
| Third digit | A1 |
| Fourth digit | A2 |

### Buttons and Buzzer

| Component | Arduino Pin |
|---|---|
| Hour button | A3 |
| Minute button | A4 |
| Buzzer | A5 |

The buttons use the Arduino's internal pull-up resistors and are active LOW.

## How It Works

### 1. Real-Time Clock

The DS1302 communicates with the Arduino through its data, clock, and reset connections.

The RTC maintains time independently of the main program and can continue timekeeping when the Arduino is powered off, provided its backup battery is functional.

### 2. Display Multiplexing

The firmware refreshes the four display digits in sequence.

It sets the required segment pattern, activates one digit, and then moves to the next. Repeating this process rapidly makes the display appear continuously illuminated.

A lookup table stores the segment patterns for digits 0–9.

### 3. Clock Adjustment

In clock mode:
- Pressing the hour button advances the hour.
- Pressing the minute button advances the minute.

The updated time is written to the RTC.

### 4. Alarm Setting

Holding both buttons for approximately 1.5 seconds switches between clock mode and alarm-setting mode.

In alarm-setting mode:
- The hour button advances the alarm hour.
- The minute button advances the alarm minute.

The display shows the alarm time while this mode is active.

The alarm time is stored in program variables and resets to 06:30 when the Arduino restarts.

### 5. Alarm Operation

When the RTC time matches the configured alarm hour and minute, the buzzer activates.

The alarm stops automatically after approximately 10 seconds. Pressing either button also silences it.

## Software

**Language:** Arduino C/C++

**Libraries:**
- `ThreeWire`
- `RtcDS1302`

The firmware uses the following programming concepts:
- Functions and arrays
- Digital input and output
- RTC communication
- Display multiplexing
- Button edge detection
- `millis()`-based timing
- State-based control logic
- Conditional alarm activation

## Project Structure

```text
arduino-digital-clock-with-alarm/
├── README.md
├── firmware/
│   └── digital_clock_alarm.ino
├── hardware/
│   └── circuit_diagram.png
└── images/
    └── completed_clock.jpg
```
## Future Improvements

- Save alarm settings across restarts.
- Improve button debouncing.
- Replace blocking display delays with non-blocking timing.
- Design a custom PCB using KiCad.
- Develop a suitable enclosure.
- Add an alarm enable/disable feature.

## Learning Outcomes

This project provided practical experience integrating a microcontroller, RTC module, display, buttons, and buzzer into an embedded system.

It also provided hands-on learning in display multiplexing, timing logic, digital interfaces, and hardware-software integration.
