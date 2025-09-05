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
- Capacitive touch button (3-pin: GND, VCC, I/O)
- MOSFET module for 12V camera control
- Resistors: 4.7kΩ and 1kΩ for reverse gear voltage divider

## Pin Configuration

| Pin | Function          | Type             | Description                                       |
| --- | ----------------- | ---------------- | ------------------------------------------------- |
| D3  | REVERSE_GEAR_PIN  | Input (Pull-up)  | Reverse gear switch input (LOW = reverse engaged) |
| D4  | CAMERA_MOSFET_PIN | Output           | Camera 12V MOSFET control (HIGH = camera on)      |
| D5  | CAMERA_BUTTON_PIN | Input (External) | Capacitive touch button I/O pin (HIGH = touched)  |

**Note**: Pin D2 is avoided as it may be damaged on some boards.

## Wiring Diagram

```
Arduino Uno/Nano
├── D3 (green) ── 1kΩ ──┐
│                       │
│                       └── Reverse Gear Switch ── GND
│                       │
│                       └── 4.7kΩ ── +5V
│
├── D4 ── MOSFET Gate
│        MOSFET Source ── GND
│        MOSFET Drain ── Camera 12V+
│
├── D5 ── Capacitive Touch Button I/O
│
├── +5V ── Capacitive Touch Button VCC
│
├── GND ── Capacitive Touch Button GND
│
└── GND ── Common Ground
```

### Wiring Details

**Reverse Gear Switch (D3)**:

- Uses voltage divider circuit for reliable detection
- 4.7kΩ resistor from D3 to +5V (pull-up)
- 1kΩ resistor in series with switch
- Switch connects to GND when reverse engaged
- When reverse engaged: D3 reads LOW (through 1kΩ)
- When not in reverse: D3 reads HIGH (through 4.7kΩ)

**Capacitive Touch Button (D5)**:

- 3-pin capacitive touch sensor
- VCC pin connected to Arduino +5V
- GND pin connected to Arduino GND
- I/O pin connected to Arduino D5
- Built-in pull-up resistor and touch detection
- When touched: I/O pin reads HIGH
- When not touched: I/O pin reads LOW

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

- Verify voltage divider circuit:
  - 4.7kΩ resistor from D3 to +5V
  - 1kΩ resistor in series with switch
  - Switch connects to GND when reverse engaged
- Check that switch is normally open (closed when reverse engaged)
- Measure voltage at D3: should be ~5V when not in reverse, ~0.8V when in reverse
- Monitor Serial output for gear state changes

### Capacitive Touch Button Not Working

- Verify 3-pin connections:
  - VCC pin to Arduino +5V
  - GND pin to Arduino GND
  - I/O pin to Arduino D5
- Check that capacitive button is powered (VCC connected)
- Test touch sensitivity - some buttons require direct finger contact
- Monitor Serial output for touch detection
- Ensure button is not damaged or defective

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
