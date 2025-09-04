# Car Accessories System

A simple Arduino-based system that automatically activates a backup camera when reverse gear is engaged, with additional manual control capabilities.

## Features

- **Automatic Camera Activation**: Camera turns on automatically when reverse gear is engaged
- **Manual Camera Control**: Manual button to activate camera for 15 seconds
- **Smart Timeout Logic**:
  - Manual activation: 15-second timeout
  - Reverse gear activation: 1-minute timeout after reverse is disengaged
- **Debounced Inputs**: Reliable detection of reverse gear and button presses
- **Serial Debugging**: Status messages via Serial output

## Hardware Requirements

- Arduino board (tested with Arduino Uno/Nano)
- 12V Camera with MOSFET control
- Reverse gear switch (normally open)
- Manual camera activation button
- MOSFET module for 12V camera control
- Pull-up resistors (internal Arduino pull-ups used)

## Pin Configuration

| Pin | Function          | Type            | Description                                       |
| --- | ----------------- | --------------- | ------------------------------------------------- |
| D3  | REVERSE_GEAR_PIN  | Input (Pull-up) | Reverse gear switch input (LOW = reverse engaged) |
| D4  | CAMERA_MOSFET_PIN | Output          | Camera 12V MOSFET control (HIGH = camera on)      |
| D5  | CAMERA_BUTTON_PIN | Input (Pull-up) | Manual camera activation button (LOW = pressed)   |

**Note**: Pin D2 is avoided as it may be damaged on some boards.

## Wiring Diagram

```
Arduino Uno/Nano
├── D3 ──┐
│        │
│        └── Reverse Gear Switch ── GND
│
├── D4 ── MOSFET Gate
│        MOSFET Source ── GND
│        MOSFET Drain ── Camera 12V+
│
├── D5 ──┐
│        │
│        └── Manual Button ── GND
│
└── GND ── Common Ground
```

## Software Architecture

### Files Structure

- `main.cpp` - Main program entry point
- `reverse.h` - Header file with function declarations and constants
- `reverse.cpp` - Implementation of reverse gear detection and camera control

### Key Functions

#### Setup Functions

- `setupReverse()` - Initialize all pins and initial states

#### Main Loop Functions

- `handleReverse()` - Main function called in loop() to handle all logic

#### Reverse Gear Functions

- `isReverseGearEngaged()` - Returns current reverse gear state

#### Camera Functions

- `activateCameraByReverse()` - Activate camera due to reverse gear
- `deactivateCameraByReverse()` - Start timeout countdown when reverse disengaged
- `isCameraActive()` - Returns current camera state

## Configuration Constants

```cpp
// Pin assignments
const byte REVERSE_GEAR_PIN = 3;
const int CAMERA_MOSFET_PIN = 4;
const int CAMERA_BUTTON_PIN = 5;

// Timing constants
const unsigned long REVERSE_GEAR_DEBOUNCE_MS = 100;
const unsigned long CAMERA_BUTTON_DEBOUNCE_MS = 100;
const unsigned long CAMERA_AUTO_OFF_TIMEOUT_MS = 60000;  // 1 minute
const unsigned long CAMERA_MANUAL_TIMEOUT_MS = 15000;    // 15 seconds
```

## Operation Modes

### 1. Automatic Mode (Reverse Gear)

- When reverse gear is engaged (D3 goes LOW), camera activates immediately
- Camera stays on while reverse gear is engaged
- When reverse gear is disengaged, camera starts 1-minute countdown
- Camera turns off after 1 minute unless manually activated

### 2. Manual Mode (Button Press)

- Press and hold button (D5 to GND) to activate camera
- Camera runs for 15 seconds then automatically turns off
- Manual activation overrides reverse gear timeout

### 3. Combined Mode

- Both modes can work together
- Manual activation takes priority over reverse gear timeout
- Camera state is properly managed between both modes

## Serial Output

The system provides debug information via Serial (9600 baud):

```
Car Accessories System Starting...
Reverse gear and camera module initialized
System ready!
Camera activated by reverse gear!
Reverse gear disengaged - camera will turn off in 1 minute
Camera activated by manual button!
Camera turned off - manual timeout (15 seconds)
Camera turned off - auto timeout (1 minute)
```

## Installation

1. **Hardware Setup**:

   - Connect reverse gear switch between D3 and GND
   - Connect MOSFET gate to D4
   - Connect camera button between D5 and GND
   - Connect 12V camera to MOSFET drain and GND

2. **Software Setup**:
   - Upload the code to your Arduino
   - Open Serial Monitor at 9600 baud to see debug messages
   - Test reverse gear detection and manual button

## Troubleshooting

### Camera Not Turning On

- Check MOSFET connections (D4 to gate, 12V to drain)
- Verify camera power supply (12V)
- Check Serial output for activation messages

### Reverse Gear Not Detected

- Verify switch connection between D3 and GND
- Check that switch is normally open (closed when reverse engaged)
- Monitor Serial output for gear state changes

### Button Not Working

- Check button connection between D5 and GND
- Verify button is normally open (closed when pressed)
- Check debounce timing in Serial output

### Camera Stays On

- Check timeout logic in Serial output
- Verify reverse gear detection is working
- Manual button may be stuck (check physical button)

## Customization

### Changing Timeouts

Modify these constants in `reverse.cpp`:

```cpp
const unsigned long CAMERA_AUTO_OFF_TIMEOUT_MS = 60000;  // 1 minute
const unsigned long CAMERA_MANUAL_TIMEOUT_MS = 15000;    // 15 seconds
```

### Changing Pin Assignments

Update pin constants in `reverse.cpp`:

```cpp
const byte REVERSE_GEAR_PIN = 3;
const int CAMERA_MOSFET_PIN = 4;
const int CAMERA_BUTTON_PIN = 5;
```

### Adding Features

- LED indicators for camera status
- Multiple camera support
- Remote control integration
- Data logging capabilities

## Safety Notes

- Ensure proper 12V isolation for camera circuit
- Use appropriate MOSFET ratings for your camera current
- Test all connections before final installation
- Consider adding fuses for 12V circuit protection

## License

This project is open source. Feel free to modify and distribute as needed.

## Version History

- v1.0 - Initial release with basic reverse gear and camera control
- v1.1 - Merged separate modules into single reverse.cpp file
- v1.2 - Updated pin assignments to avoid broken pin D2
