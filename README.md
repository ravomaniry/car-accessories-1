# Car Accessories System

A simple Arduino-based system that automatically activates a backup camera when reverse gear is engaged, with additional manual control capabilities and horn control.

## Features

- **Automatic Camera Activation**: Camera turns on automatically when reverse gear is engaged
- **Manual Camera Control**: Manual button to activate camera for 15 seconds
- **Horn Control**: Capacitive touch button for horn activation with MOSFET control
- **Smart Timeout Logic**:
  - Manual activation: 15-second timeout
  - Reverse gear activation: 1-minute timeout after reverse is disengaged
  - Horn: Maximum 5-second continuous operation for safety
- **Debounced Inputs**: Reliable detection of reverse gear and button presses
- **Serial Debugging**: Status messages via Serial output

## Hardware Requirements

- Arduino board (tested with Arduino Uno/Nano)
- 12V Camera with MOSFET control
- 12V Horn with MOSFET control
- Reverse gear switch (normally open)
- Capacitive touch button for camera (3-pin: GND, VCC, I/O)
- Capacitive touch button for horn (3-pin: GND, VCC, I/O)
- MOSFET modules for 12V camera and horn control
- Resistors: 4.7kΩ and 1kΩ for reverse gear voltage divider

## Pin Configuration

| Pin | Function          | Type             | Description                                             |
| --- | ----------------- | ---------------- | ------------------------------------------------------- |
| D3  | REVERSE_GEAR_PIN  | Input (Pull-up)  | Reverse gear switch input (LOW = reverse engaged)       |
| D4  | CAMERA_MOSFET_PIN | Output           | Camera 12V MOSFET control (HIGH = camera on)            |
| D5  | CAMERA_BUTTON_PIN | Input (External) | Camera capacitive touch button I/O pin (HIGH = touched) |
| D6  | HORN_BUTTON_PIN   | Input (External) | Horn capacitive touch button I/O pin (HIGH = touched)   |
| D7  | HORN_MOSFET_PIN   | Output           | Horn 12V MOSFET control (HIGH = horn on)                |

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
├── D4 ── Camera MOSFET Gate
│        Camera MOSFET Source ── GND
│        Camera MOSFET Drain ── Camera 12V+
│
├── D5 ── Camera Capacitive Touch Button I/O
│
├── D6 ── Horn Capacitive Touch Button I/O
│
├── D7 ── Horn MOSFET Gate
│        Horn MOSFET Source ── GND
│        Horn MOSFET Drain ── Horn 12V+
│
├── +5V ── Capacitive Touch Buttons VCC
│
├── GND ── Capacitive Touch Buttons GND
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

**Camera Capacitive Touch Button (D5)**:

- 3-pin capacitive touch sensor
- VCC pin connected to Arduino +5V
- GND pin connected to Arduino GND
- I/O pin connected to Arduino D5
- Built-in pull-up resistor and touch detection
- When touched: I/O pin reads HIGH
- When not touched: I/O pin reads LOW

**Horn Capacitive Touch Button (D6)**:

- 3-pin capacitive touch sensor
- VCC pin connected to Arduino +5V
- GND pin connected to Arduino GND
- I/O pin connected to Arduino D6
- Built-in pull-up resistor and touch detection
- When touched: I/O pin reads HIGH
- When not touched: I/O pin reads LOW

## Software Architecture

### Files Structure

- `main.cpp` - Main program entry point
- `reverse.h` - Header file with function declarations and constants
- `reverse.cpp` - Implementation of reverse gear detection and camera control
- `horn.h` - Header file with horn function declarations and constants
- `horn.cpp` - Implementation of horn control with capacitive button and MOSFET

### Key Functions

#### Setup Functions

- `setupReverse()` - Initialize all pins and initial states
- `setupHorn()` - Initialize horn pins and initial states

#### Main Loop Functions

- `handleReverse()` - Main function called in loop() to handle all logic
- `handleHorn()` - Main function called in loop() to handle horn logic

#### Reverse Gear Functions

- `isReverseGearEngaged()` - Returns current reverse gear state

#### Camera Functions

- `activateCameraByReverse()` - Activate camera due to reverse gear
- `deactivateCameraByReverse()` - Start timeout countdown when reverse disengaged
- `isCameraActive()` - Returns current camera state

#### Horn Functions

- `activateHorn()` - Activate horn
- `deactivateHorn()` - Deactivate horn
- `isHornActive()` - Returns current horn state

## Configuration Constants

```cpp
// Pin assignments
const byte REVERSE_GEAR_PIN = 3;
const int CAMERA_MOSFET_PIN = 4;
const int CAMERA_BUTTON_PIN = 5;
const int HORN_BUTTON_PIN = 6;
const int HORN_MOSFET_PIN = 7;

// Timing constants
const unsigned long REVERSE_GEAR_DEBOUNCE_MS = 100;
const unsigned long CAMERA_BUTTON_DEBOUNCE_MS = 100;
const unsigned long CAMERA_AUTO_OFF_TIMEOUT_MS = 60000;  // 1 minute
const unsigned long CAMERA_MANUAL_TIMEOUT_MS = 15000;    // 15 seconds
const unsigned long HORN_BUTTON_DEBOUNCE_MS = 100;
const unsigned long HORN_MAX_DURATION_MS = 5000;         // 5 seconds
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

### 3. Horn Mode

- Press and hold capacitive touch button to activate horn
- Horn runs while button is pressed
- Maximum 5-second continuous operation for safety
- Horn turns off when button is released or timeout reached

### 4. Combined Mode

- All modes can work together independently
- Manual camera activation takes priority over reverse gear timeout
- Horn operates independently of camera system
- Camera and horn states are properly managed separately

## Serial Output

The system provides debug information via Serial (9600 baud):

```
Car Accessories System Starting...
Reverse gear and camera module initialized
Horn module initialized
System ready!
Camera activated by reverse gear!
Reverse gear disengaged - camera will turn off in 1 minute
Camera activated by manual button!
Camera turned off - manual timeout (15 seconds)
Camera turned off - auto timeout (1 minute)
Horn activated!
Horn deactivated!
Horn turned off - maximum duration reached (5 seconds)
```

## Installation

1. **Hardware Setup**:

   - Connect reverse gear switch between D3 and GND
   - Connect camera MOSFET gate to D4
   - Connect camera button between D5 and GND
   - Connect 12V camera to camera MOSFET drain and GND
   - Connect horn capacitive touch button to D6, +5V, and GND
   - Connect horn MOSFET gate to D7
   - Connect 12V horn to horn MOSFET drain and GND

2. **Software Setup**:
   - Upload the code to your Arduino
   - Open Serial Monitor at 9600 baud to see debug messages
   - Test reverse gear detection, camera button, and horn button

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

### Horn Not Working

- Check MOSFET connections (D7 to gate, 12V to drain)
- Verify horn power supply (12V)
- Check Serial output for horn activation messages
- Verify capacitive touch button connections:
  - VCC pin to Arduino +5V
  - GND pin to Arduino GND
  - I/O pin to Arduino D6
- Test touch sensitivity - some buttons require direct finger contact
- Ensure button is not damaged or defective

### Horn Stays On

- Check that capacitive touch button is not stuck
- Verify button release detection in Serial output
- Check for timeout safety mechanism (5-second limit)

## Customization

### Changing Timeouts

Modify these constants in `reverse.cpp` and `horn.cpp`:

```cpp
// In reverse.cpp
const unsigned long CAMERA_AUTO_OFF_TIMEOUT_MS = 60000;  // 1 minute
const unsigned long CAMERA_MANUAL_TIMEOUT_MS = 15000;    // 15 seconds

// In horn.cpp
const unsigned long HORN_MAX_DURATION_MS = 5000;         // 5 seconds
```

### Changing Pin Assignments

Update pin constants in `reverse.cpp` and `horn.cpp`:

```cpp
// In reverse.cpp
const byte REVERSE_GEAR_PIN = 3;
const int CAMERA_MOSFET_PIN = 4;
const int CAMERA_BUTTON_PIN = 5;

// In horn.cpp
const int HORN_BUTTON_PIN = 6;
const int HORN_MOSFET_PIN = 7;
```

### Adding Features

- LED indicators for camera and horn status
- Multiple camera support
- Horn patterns (short beeps, long beeps)
- Remote control integration
- Data logging capabilities

## Safety Notes

- Ensure proper 12V isolation for camera and horn circuits
- Use appropriate MOSFET ratings for your camera and horn current
- Test all connections before final installation
- Consider adding fuses for 12V circuit protection
- Horn has built-in 5-second safety timeout to prevent damage

## License

This project is open source. Feel free to modify and distribute as needed.

## Version History

- v1.0 - Initial release with basic reverse gear and camera control
- v1.1 - Merged separate modules into single reverse.cpp file
- v1.2 - Updated pin assignments to avoid broken pin D2
- v1.3 - Added horn support with capacitive button and MOSFET control
