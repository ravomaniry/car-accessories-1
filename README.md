# Car Accessories System

A comprehensive Arduino-based car accessories system that automatically activates a backup camera when reverse gear is engaged, with manual control capabilities, horn control, GPS tracking functionality, automatic headlight control, and power management for accessories.

## Features

- **Automatic Camera Activation**: Camera turns on automatically when reverse gear is engaged
- **Manual Camera Control**: Manual button to activate camera for 15 seconds
- **Horn Control**: Capacitive touch button for horn activation with MOSFET control
- **GPS Tracking**: Real-time speed and location tracking using NEO-6M GPS module
- **Automatic Headlight Control**: Intelligent headlight system with photosensitive sensor
- **Power Management**: Autoradio power control via MOSFET with 60-second startup delay
- **Serial Communication**: Data transmission to ESP32 in format `KEY:VALUE` (e.g., `SPEED:120`, `DRL:1`, `LOWBEAM:0`)
- **ESP32 Integration**: Hardware serial pins reserved for ESP32 communication
- **Smart Timeout Logic**:
  - Manual activation: 15-second timeout
  - Reverse gear activation: 1-minute timeout after reverse is disengaged
  - Horn: Maximum 5-second continuous operation for safety
  - Autoradio: 60-second startup delay before power activation
  - Headlights: 5-second debounce when turning ON, 1-minute debounce when turning OFF
- **Debounced Inputs**: Reliable detection of reverse gear and button presses
- **Serial Debugging**: Status messages via Serial output

## Hardware Requirements

- Arduino board (tested with Arduino Uno/Nano)
- 12V Camera with MOSFET control
- 12V Horn with MOSFET control
- 12V Autoradio with MOSFET control
- 12V Headlight system (DRL, Tail Light, Low Beam) with MOSFET control
- NEO-6M GPS module
- Photosensitive sensor module (3-pin: VCC, GND, DO)
- Reverse gear switch (normally open)
- Capacitive touch button for camera (3-pin: GND, VCC, I/O)
- Capacitive touch button for horn (3-pin: GND, VCC, I/O)
- MOSFET modules for 12V camera, horn, autoradio, and headlight control
- Resistors: 4.7kΩ and 1kΩ for reverse gear voltage divider
- ESP32 (optional, for advanced communication)

## Pin Configuration

| Pin | Function              | Type             | Description                                             |
| --- | --------------------- | ---------------- | ------------------------------------------------------- |
| D0  | Serial RX             | Hardware Serial  | ESP32 TX (reserved for ESP32 communication)             |
| D1  | Serial TX             | Hardware Serial  | ESP32 RX (reserved for ESP32 communication)             |
| A0  | PHOTOSENSOR_PIN       | Input (Analog)   | Photosensitive sensor DO pin (analog reading)           |
| D3  | REVERSE_GEAR_PIN      | Input (Pull-up)  | Reverse gear switch input (LOW = reverse engaged)       |
| D4  | CAMERA_MOSFET_PIN     | Output           | Camera 12V MOSFET control (HIGH = camera on)            |
| D5  | CAMERA_BUTTON_PIN     | Input (External) | Camera capacitive touch button I/O pin (HIGH = touched) |
| D6  | HORN_BUTTON_PIN       | Input (External) | Horn capacitive touch button I/O pin (HIGH = touched)   |
| D7  | HORN_MOSFET_PIN       | Output           | Horn 12V MOSFET control (HIGH = horn on)                |
| D8  | GPS_TX_PIN            | Software Serial  | GPS RX pin (to NEO-6M GPS module)                       |
| D9  | GPS_RX_PIN            | Software Serial  | GPS TX pin (from NEO-6M GPS module)                     |
| D10 | AUTORADIO_MOSFET_PIN  | Output           | Autoradio 12V MOSFET control (HIGH = autoradio on)      |
| D11 | DRL_MOSFET_PIN        | Output           | DRL (Daytime Running Lights) MOSFET control             |
| D12 | TAIL_LIGHT_MOSFET_PIN | Output           | Tail light MOSFET control                               |
| D13 | LOW_BEAM_MOSFET_PIN   | Output           | Low beam headlight MOSFET control                       |

**Note**: Pin D2 is avoided as it may be damaged on some boards. Hardware serial pins (D0/D1) are reserved for ESP32 communication.

## Wiring Diagram

```
Arduino Uno/Nano
├── D0 (RX) ── ESP32 TX (Hardware Serial)
├── D1 (TX) ── ESP32 RX (Hardware Serial)
│
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
├── D8 ── GPS RX (to NEO-6M)
├── D9 ── GPS TX (from NEO-6M)
│
├── D10 ── Autoradio MOSFET Gate
│         Autoradio MOSFET Source ── GND
│         Autoradio MOSFET Drain ── Autoradio 12V+
│
├── D11 ── DRL MOSFET Gate
│         DRL MOSFET Source ── GND
│         DRL MOSFET Drain ── DRL 12V+
│
├── D12 ── Tail Light MOSFET Gate
│         Tail Light MOSFET Source ── GND
│         Tail Light MOSFET Drain ── Tail Light 12V+
│
├── D13 ── Low Beam MOSFET Gate
│         Low Beam MOSFET Source ── GND
│         Low Beam MOSFET Drain ── Low Beam 12V+
│
├── A0 ── Photosensitive Sensor DO
│
├── +5V ── Capacitive Touch Buttons VCC
├── +5V ── GPS VCC
├── +5V ── Photosensitive Sensor VCC
│
├── GND ── Capacitive Touch Buttons GND
├── GND ── GPS GND
├── GND ── ESP32 GND
├── GND ── Photosensitive Sensor GND
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

**GPS Module (NEO-6M) (D8, D9)**:

- 4-pin GPS module (VCC, GND, TX, RX)
- VCC pin connected to Arduino +5V
- GND pin connected to Arduino GND
- TX pin connected to Arduino D9 (GPS RX)
- RX pin connected to Arduino D8 (GPS TX)
- Uses SoftwareSerial for communication
- Default baud rate: 9600
- Provides real-time speed and location data

**Autoradio MOSFET Control (D10)**:

- MOSFET gate connected to Arduino D10
- MOSFET source connected to GND
- MOSFET drain connected to autoradio 12V+ supply
- Autoradio GND connected to common ground
- Autoradio powers on automatically after 60-second delay
- HIGH signal on D10 enables autoradio power
- LOW signal on D10 disables autoradio power

**Photosensitive Sensor (A0)**:

- 3-pin photosensitive sensor module (VCC, GND, DO)
- VCC pin connected to Arduino +5V
- GND pin connected to Arduino GND
- DO pin connected to Arduino A0 (analog input)
- Provides analog readings (0-1023) for light level detection
- Higher values indicate more light, lower values indicate less light

**Headlight MOSFET Controls (D11, D12, D13)**:

- **DRL MOSFET (D11)**: Controls Daytime Running Lights

  - MOSFET gate connected to Arduino D11
  - MOSFET source connected to GND
  - MOSFET drain connected to DRL 12V+ supply
  - HIGH signal enables DRL, LOW signal disables DRL

- **Tail Light MOSFET (D12)**: Controls tail lights

  - MOSFET gate connected to Arduino D12
  - MOSFET source connected to GND
  - MOSFET drain connected to tail light 12V+ supply
  - HIGH signal enables tail lights, LOW signal disables tail lights

- **Low Beam MOSFET (D13)**: Controls low beam headlights
  - MOSFET gate connected to Arduino D13
  - MOSFET source connected to GND
  - MOSFET drain connected to low beam 12V+ supply
  - HIGH signal enables low beam, LOW signal disables low beam

**ESP32 Communication (D0, D1)**:

- Hardware serial communication
- Arduino D0 (RX) connected to ESP32 TX
- Arduino D1 (TX) connected to ESP32 RX
- Common ground connection required
- Used for advanced data transmission and processing

## Software Architecture

The system is organized into modular components:

- **Reverse Gear Module**: Handles reverse gear detection and camera control
- **Horn Module**: Manages horn activation with capacitive touch button
- **GPS Module**: Provides real-time location and speed tracking
- **Headlight Module**: Intelligent headlight control with photosensitive sensor
- **Power Management Module**: Controls autoradio power with MOSFET switching

## Configuration Constants

```cpp
// Pin assignments
const byte REVERSE_GEAR_PIN = 3;
const int CAMERA_MOSFET_PIN = 4;
const int CAMERA_BUTTON_PIN = 5;
const int HORN_BUTTON_PIN = 6;
const int HORN_MOSFET_PIN = 7;
const int GPS_RX_PIN = 8;
const int GPS_TX_PIN = 9;
const int AUTORADIO_MOSFET_PIN = 10;
const int PHOTOSENSOR_PIN = A0;
const int DRL_MOSFET_PIN = 11;
const int TAIL_LIGHT_MOSFET_PIN = 12;
const int LOW_BEAM_MOSFET_PIN = 13;

// Timing constants
const unsigned long REVERSE_GEAR_DEBOUNCE_MS = 100;
const unsigned long CAMERA_BUTTON_DEBOUNCE_MS = 100;
const unsigned long CAMERA_AUTO_OFF_TIMEOUT_MS = 60000;  // 1 minute
const unsigned long CAMERA_MANUAL_TIMEOUT_MS = 15000;    // 15 seconds
const unsigned long HORN_BUTTON_DEBOUNCE_MS = 100;
const unsigned long HORN_MAX_DURATION_MS = 5000;         // 5 seconds
const unsigned long AUTORADIO_TIMEOUT_MS = 60000;         // 60 seconds
const unsigned long GPS_UPDATE_INTERVAL_MS = 1000;        // 1 second
const unsigned long HEADLIGHT_ON_DEBOUNCE_MS = 5000;      // 5 seconds when turning lights ON
const unsigned long HEADLIGHT_OFF_DEBOUNCE_MS = 60000;     // 1 minute when turning lights OFF

// GPS constants
const int GPS_BAUD_RATE = 9600;

// Headlight constants
const float DRL_ACTIVATION_SPEED_THRESHOLD = 5.0;        // Speed threshold for DRL activation
int LOW_LIGHT_THRESHOLD = 300;                           // Low light detection threshold
int DARK_THRESHOLD = 150;                                // Dark detection threshold
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

### 4. Power Management Mode

- Autoradio power control via MOSFET on pin 10
- 60-second startup delay after system initialization
- Autoradio automatically powers on after delay
- No manual intervention required
- MOSFET provides clean 12V switching for autoradio

### 5. GPS Tracking Mode

- Continuous GPS data collection from NEO-6M module
- Real-time speed and location tracking
- Data transmitted every 1 second via serial when GPS is valid
- Format: `SPEED:value` and `LOCATION:lat,lng`
- No data transmitted when GPS signal is invalid or unavailable

### 6. Combined Mode

- All modes can work together independently
- Manual camera activation takes priority over reverse gear timeout
- Horn operates independently of camera system
- Autoradio power management runs automatically in background
- GPS tracking runs continuously in background
- Camera, horn, and autoradio states are properly managed separately

## Serial Output

The system provides debug information via Serial (9600 baud):

```
Car Accessories System Starting...
Reverse gear and camera module initialized
Horn module initialized
GPS module initialized
Power management system initialized
System ready!
Camera activated by reverse gear!
Reverse gear disengaged - camera will turn off in 1 minute
Camera activated by manual button!
Camera turned off - manual timeout (15 seconds)
Camera turned off - auto timeout (1 minute)
Horn activated!
Horn deactivated!
Horn turned off - maximum duration reached (5 seconds)
SPEED:45.50
LOCATION:40.712776,-74.005974
SPEED:46.20
LOCATION:40.712800,-74.005950
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
   - Connect GPS module TX to D9, RX to D8, VCC to +5V, GND to GND
   - Connect ESP32 TX to D0, RX to D1, GND to GND (optional)

2. **Software Setup**:
   - Upload the code to your Arduino
   - Open Serial Monitor at 9600 baud to see debug messages
   - Test reverse gear detection, camera button, and horn button
   - Wait for GPS signal acquisition (may take 1-2 minutes)
   - Verify GPS data output in serial monitor

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

### GPS Not Working

- Verify GPS module connections:
  - TX pin to Arduino D9
  - RX pin to Arduino D8
  - VCC pin to Arduino +5V
  - GND pin to Arduino GND
- Check GPS module power (LED should blink when powered)
- Wait for GPS signal acquisition (1-2 minutes for first fix)
- Ensure GPS module has clear view of sky
- Check Serial output for GPS data format
- Verify GPS module is not damaged

### GPS Data Invalid

- Check that GPS has valid satellite fix
- Verify GPS module is receiving satellite signals
- Ensure GPS module is not in indoor/underground location
- Check GPS antenna connection
- Monitor Serial output for GPS status messages

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

Update pin constants in `reverse.cpp`, `horn.cpp`, and `gps.cpp`:

```cpp
// In reverse.cpp
const byte REVERSE_GEAR_PIN = 3;
const int CAMERA_MOSFET_PIN = 4;
const int CAMERA_BUTTON_PIN = 5;

// In horn.cpp
const int HORN_BUTTON_PIN = 6;
const int HORN_MOSFET_PIN = 7;

// In gps.cpp
const int GPS_RX_PIN = 9;
const int GPS_TX_PIN = 8;
```

### Adding Features

- LED indicators for camera, horn, and GPS status
- Multiple camera support
- Horn patterns (short beeps, long beeps)
- Remote control integration
- Data logging capabilities
- GPS data storage to SD card
- Real-time GPS tracking display
- Speed alerts and notifications

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
