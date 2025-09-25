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
- Higher values indicate LESS light (darkness), lower values indicate MORE light (brightness)
- Sensor behavior is reversed: HIGH = dark, LOW = bright

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

## Usage

### Camera System

- **Automatic**: Activates when reverse gear is engaged
- **Manual**: Press capacitive touch button for 15-second activation
- **Timeout**: 1-minute delay after reverse gear disengagement

### Horn System

- **Activation**: Press and hold capacitive touch button
- **Safety**: Maximum 5-second continuous operation
- **Release**: Horn stops when button is released

### Headlight System

- **Day**: DRL ON with 1-minute timeout (avoids power competition during cranking)
- **Low Light**: DRL and tail lights ON
- **Dark**: DRL, tail lights, and low beam always ON
- **Debounce**: 5-second delay when turning ON, 1-minute delay when turning OFF

### Power Management

- **Autoradio**: Automatically powers on after 60-second startup delay
- **GPS**: Continuous tracking with data transmission to ESP32

## Serial Communication

The system communicates with ESP32 using `KEY:VALUE` format:

- `SPEED:45.50` - Vehicle speed in km/h
- `LOCATION:40.712776,-74.005974` - GPS coordinates
- `DRL:1` - Daytime running lights ON
- `LOWBEAM:0` - Low beam headlights OFF
- `TAIL_LIGHT:1` - Tail lights ON
