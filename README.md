# Car Accessories System

A comprehensive Arduino-based car accessories system that automatically activates a backup camera when reverse gear is engaged, with manual control capabilities, horn control, GPS tracking functionality, automatic headlight control, and power management for accessories.

## Wiring Diagram

```
Arduino Uno/Nano
├── D0 (RX) ── ESP32 TX (Hardware Serial)
├── D1 (TX) ── ESP32 RX (Hardware Serial)
│
├── D3 ── 1kΩ ──┐
│               │
│               └── PIN 4 ── Reverse Gear Switch ── GND
│               │
│               └── 4.7kΩ ── +5V
│
├── D4 ── RELAY 1 ── Backup Camera
│
├── D5 ── Camera Capacitive Touch Button I/O
│
├── D6 ── Horn Capacitive Touch Button I/O
│
├── D7 ── RELAY 2 ── Horn
│
├── D8 ── GPS RX (to NEO-6M)
├── D9 ── GPS TX (from NEO-6M)
│
├── D10 ── MOSFET 3 ── Autoradio power
│
├── A3 ── MOSFET 4 ── High Beam
│
├── D11 ── RELAY 3 ── DRL
│
├── D12 ── MOSFET 7 ── Tail Light
|
├── D13 ── RELAY 4 ── Low Beam
│
├── A0 ── Photosensitive Sensor DO
├── A1 ── Joystick Y-Axis (Analog)
├── A2 ── RELAY 5 ── High beam
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

**Analog Joystick Module (A1)**:

- 2-axis analog joystick module (using Y-axis only)
- VCC pin connected to Arduino +5V
- GND pin connected to Arduino GND
- Y-axis pin connected to Arduino A1 (analog input)
- Provides analog readings (0-1023) for Y-axis position
- **Thresholds**:
  - Y > 800: Joystick pushed UP (beam flashing)
  - Y < 200: Joystick pushed DOWN (beam switching)
  - 400 < Y < 600: Center position (no action)
- Used for high/low beam switching and flashing control

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

**Headlight MOSFET Controls (A3, D11, D12, D13)**:

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

- **High Beam MOSFET (A3)**: Controls high beam headlights
  - MOSFET gate connected to Arduino A3
  - MOSFET source connected to GND
  - MOSFET drain connected to high beam 12V+ supply
  - HIGH signal enables high beam, LOW signal disables high beam

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

### High/Low Beam Control

- **Joystick Down (Y < 200)**: Toggle between LOW and HIGH beam modes
  - LOW → HIGH (switches to high beam)
  - HIGH → LOW (switches to low beam)
  - OFF → No action (ignores joystick input)
- **Joystick Up (Y > 800)**: Flash beams twice and return to previous state
  - Flashes high beam, then low beam, then high beam, then low beam
  - Each flash duration: 300ms with 200ms pause (relay-friendly timing)
  - Returns to original beam mode after flashing
- **Center Position (400 < Y < 600)**: No action (joystick centered)
- **Debounce**: 200ms debounce for joystick inputs to prevent multiple triggers
- **Automatic Control**: In dark conditions, automatically turns on low beam if currently OFF

### Power Management

- **Autoradio**: Automatically powers on after 60-second startup delay
- **GPS**: Continuous tracking with data transmission to ESP32

## Serial Communication

The system communicates with ESP32 using `KEY:VALUE` format:

- `SPEED:45.50` - Vehicle speed in km/h
- `LOCATION:40.712776,-74.005974` - GPS coordinates
- `DRL:1` - Daytime running lights ON
- `LOWBEAM:0` - Low beam headlights OFF
- `HIGHBEAM:1` - High beam headlights ON
- `TAIL_LIGHT:1` - Tail lights ON
