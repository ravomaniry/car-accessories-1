#include "headlights.h"
#include "gps.h"
#include <Arduino.h>

// Headlight pin configuration
const int PHOTOSENSOR_PIN = A0;       // A0: Photosensitive sensor DO pin (analog input)
const int DRL_MOSFET_PIN = 11;        // D11: DRL MOSFET control
const int TAIL_LIGHT_MOSFET_PIN = 12; // D12: Tail light MOSFET control
const int LOW_BEAM_MOSFET_PIN = 13;   // D13: Low beam MOSFET control
const int HIGH_BEAM_MOSFET_PIN = A2;  // A2: High beam MOSFET control
const int JOYSTICK_Y_PIN = A1;        // A1: Joystick Y-axis analog pin

// Timing configuration
const unsigned long LIGHT_ON_DEBOUNCE_MS = 5000;   // 5 seconds debounce when turning lights ON
const unsigned long LIGHT_OFF_DEBOUNCE_MS = 60000; // 1 minute debounce when turning lights OFF
const unsigned long DRL_TIMEOUT_MS = 60000;        // 1 minute DRL timeout to avoid power competition during cranking
const unsigned long JOYSTICK_DEBOUNCE_MS = 200;     // 200ms debounce for joystick inputs
const unsigned long BEAM_FLASH_DURATION_MS = 300;  // 300ms duration for each beam flash (relay-friendly)
const unsigned long BEAM_FLASH_PAUSE_MS = 200;      // 200ms pause between flashes (relay-friendly)

// Light level thresholds (configurable - can be adjusted via serial commands)
int LOW_LIGHT_THRESHOLD = 300;    // Threshold for low light detection (0-1023)
int DARK_THRESHOLD = 150;         // Threshold for dark detection (0-1023)

// Speed threshold
const float DRL_ACTIVATION_SPEED_THRESHOLD = 5.0;  // Speed threshold for DRL activation

// Joystick analog thresholds (0-1023)
const int JOYSTICK_UP_THRESHOLD = 800;     // Above this value = joystick pushed up
const int JOYSTICK_DOWN_THRESHOLD = 200;   // Below this value = joystick pushed down
const int JOYSTICK_CENTER_MIN = 400;       // Center position minimum
const int JOYSTICK_CENTER_MAX = 600;       // Center position maximum

// Headlight state variables
bool drlActive = false;
bool tailLightActive = false;
BeamMode currentBeamMode = BEAM_OFF;

// Individual light state change tracking
static unsigned long drlChangeRequestTime = 0;
static bool drlChangeRequested = false;
static bool drlChangeToOn = false;

static unsigned long tailLightChangeRequestTime = 0;
static bool tailLightChangeRequested = false;
static bool tailLightChangeToOn = false;

static unsigned long beamModeChangeRequestTime = 0;
static bool beamModeChangeRequested = false;
static BeamMode beamModeChangeTo = BEAM_OFF;

// DRL timeout tracking
static unsigned long drlStartTime = 0;

// Current stable states
static int currentLightLevel = 0;
static bool currentCarMoving = false;

// Joystick state tracking
static bool joystickUpPressed = false;
static bool joystickDownPressed = false;
static unsigned long lastJoystickUpTime = 0;
static unsigned long lastJoystickDownTime = 0;
static bool beamFlashInProgress = false;
static unsigned long beamFlashStartTime = 0;
static int beamFlashStep = 0;
static BeamMode previousBeamMode = BEAM_OFF;
static int joystickYValue = 512;  // Center position (0-1023)

// Light level reading variables
static int lightLevelReadings[5] = {0};  // Array for averaging readings
static int readingIndex = 0;
static unsigned long lastReadingTime = 0;
static const unsigned long READING_INTERVAL_MS = 200;  // Read every 200ms

void setupHeadlights() {
  // Setup photosensitive sensor pin
  pinMode(PHOTOSENSOR_PIN, INPUT);
  
  // Setup MOSFET control pins
  pinMode(DRL_MOSFET_PIN, OUTPUT);
  pinMode(TAIL_LIGHT_MOSFET_PIN, OUTPUT);
  pinMode(LOW_BEAM_MOSFET_PIN, OUTPUT);
  pinMode(HIGH_BEAM_MOSFET_PIN, OUTPUT);
  
  // Setup joystick pin (analog input)
  pinMode(JOYSTICK_Y_PIN, INPUT);
  
  // Initialize all lights to OFF
  digitalWrite(DRL_MOSFET_PIN, LOW);
  digitalWrite(TAIL_LIGHT_MOSFET_PIN, LOW);
  digitalWrite(LOW_BEAM_MOSFET_PIN, LOW);
  digitalWrite(HIGH_BEAM_MOSFET_PIN, LOW);
  
  // Initialize current states
  currentLightLevel = readLightLevel();
  currentCarMoving = isCarMoving();
  
  // Initialize DRL timeout - will be set when first bright condition is detected
  drlStartTime = 0;
  
  // Headlight system initialized
}

void handleHeadlights() {
  // Handle joystick inputs first (highest priority)
  handleJoystick();
  
  // Handle beam flashing if in progress
  if (beamFlashInProgress) {
    performBeamFlash();
  }
  
  // Read light level and speed at regular intervals
  if (millis() - lastReadingTime >= READING_INTERVAL_MS) {
    int newLightLevel = readLightLevel();
    bool newCarMoving = isCarMoving();
    // Check if conditions have changed significantly
    bool lightChanged = abs(newLightLevel - currentLightLevel) > 50;
    bool speedChanged = (newCarMoving != currentCarMoving);
    
    if (lightChanged || speedChanged) {
      // Update current states
      currentLightLevel = newLightLevel;
      currentCarMoving = newCarMoving;
      
      // Calculate desired light states
      calculateDesiredLightStates();
    }
    
    lastReadingTime = millis();
  }
  
  // Apply light state changes with appropriate debounce
  applyLightStateChanges();
}

void calculateDesiredLightStates() {
  BrightnessLevel brightness = getBrightnessLevel();
  
  // Calculate desired light states based on your requirements
  bool desiredDRL = false;
  bool desiredTailLight = false;
  BeamMode desiredBeamMode = currentBeamMode; // Keep current manual setting by default
  
  switch (brightness) {
    case BRIGHT:
      // During the day: tail light OFF, beams OFF (automatic control)
      desiredTailLight = false;
      desiredBeamMode = BEAM_OFF;
      // DRL ON if timeout period has passed (once activated, stays on permanently)
      if (drlStartTime == 0) {
        // Start DRL timeout on first bright condition
        drlStartTime = millis() + DRL_TIMEOUT_MS;
        desiredDRL = false; // Not yet activated
      } else if (millis() >= drlStartTime) {
        // Timeout has passed, DRL should be permanently ON
        desiredDRL = true;
      } else {
        // Still within timeout period, DRL stays OFF
        desiredDRL = false;
      }
      break;
      
    case LOW_LIGHT:
      // Low light: DRL and tail light ON (5 sec delay)
      desiredDRL = true;
      desiredTailLight = true;
      break;
      
    case DARK:
      // Dark: DRL and tail light always ON
      desiredDRL = true;
      desiredTailLight = true;
      // In dark conditions, ensure at least low beam is on (unless manually set to high beam)
      if (currentBeamMode == BEAM_OFF) {
        desiredBeamMode = BEAM_LOW;
      }
      break;
  }
  
  // Check each light individually for changes
  checkLightChange(desiredDRL, drlActive, drlChangeRequested, drlChangeRequestTime, drlChangeToOn, "DRL");
  checkLightChange(desiredTailLight, tailLightActive, tailLightChangeRequested, tailLightChangeRequestTime, tailLightChangeToOn, "Tail Light");
  
  // Handle beam mode changes with debounce (only if automatic system wants to change it)
  if (desiredBeamMode != currentBeamMode && !beamModeChangeRequested) {
    beamModeChangeRequested = true;
    beamModeChangeRequestTime = millis();
    beamModeChangeTo = desiredBeamMode;
  }
}

void checkLightChange(bool desired, bool current, bool& changeRequested, unsigned long& changeRequestTime, bool& changeToOn, const char* lightName) {
  if (desired != current && !changeRequested) {
    changeRequested = true;
    changeRequestTime = millis();
    changeToOn = desired;
  }
}

void applyLightStateChanges() {
  // Apply DRL changes
  applyIndividualLightChange(drlChangeRequested, drlChangeRequestTime, drlChangeToOn, drlActive, setDRL, "DRL");
  
  // Apply Tail Light changes
  applyIndividualLightChange(tailLightChangeRequested, tailLightChangeRequestTime, tailLightChangeToOn, tailLightActive, setTailLight, "Tail Light");
  
  // Apply Beam Mode changes with appropriate debounce timing
  if (beamModeChangeRequested) {
    unsigned long debounceTime;
    
    // Use different debounce times: 5sec for turning ON, 60sec for turning OFF
    if (beamModeChangeTo == BEAM_OFF) {
      debounceTime = LIGHT_OFF_DEBOUNCE_MS;  // 60 seconds for turning OFF
    } else {
      debounceTime = LIGHT_ON_DEBOUNCE_MS;   // 5 seconds for turning ON (LOW/HIGH)
    }
    
    if (millis() - beamModeChangeRequestTime >= debounceTime) {
      setBeamMode(beamModeChangeTo);
      beamModeChangeRequested = false;
    }
  }
}


void applyIndividualLightChange(bool& changeRequested, unsigned long& changeRequestTime, bool& changeToOn, bool& currentState, void (*setFunction)(bool), const char* lightName) {
  if (!changeRequested) return;
  
  unsigned long debounceTime = changeToOn ? LIGHT_ON_DEBOUNCE_MS : LIGHT_OFF_DEBOUNCE_MS;
  
  if (millis() - changeRequestTime >= debounceTime) {
    setFunction(changeToOn);
    currentState = changeToOn;
    changeRequested = false;
  }
}

bool isCarMoving() {
  float speed = getSpeed();
  return speed > DRL_ACTIVATION_SPEED_THRESHOLD;
}

int readLightLevel() {
  // Read analog value from photosensitive sensor
  // This sensor outputs HIGHER values in DARKNESS, LOWER values in BRIGHT LIGHT
  int reading = analogRead(PHOTOSENSOR_PIN);
  
  // Add to rolling average
  lightLevelReadings[readingIndex] = reading;
  readingIndex = (readingIndex + 1) % 5;
  
  // Calculate average
  int sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += lightLevelReadings[i];
  }
  
  return sum / 5;
}

BrightnessLevel getBrightnessLevel() {
  int lightLevel = readLightLevel();
  
  // Determine brightness level (sensor reversed: HIGH = dark, LOW = bright)
  if (lightLevel < LOW_LIGHT_THRESHOLD) {
    return BRIGHT;      // Low sensor value = bright day
  } else if (lightLevel < DARK_THRESHOLD) {
    return LOW_LIGHT;  // Medium sensor value = low light
  } else {
    return DARK;       // High sensor value = dark
  }
}

void setDRL(bool state) {
  if (drlActive != state) {
    drlActive = state;
    digitalWrite(DRL_MOSFET_PIN, state ? HIGH : LOW);
    Serial.print("DRL:");
    Serial.println(state ? "1" : "0");
  }
}

void setTailLight(bool state) {
  if (tailLightActive != state) {
    tailLightActive = state;
    digitalWrite(TAIL_LIGHT_MOSFET_PIN, state ? HIGH : LOW);
    // Note: ESP32 doesn't have a specific TAIL_LIGHT key, so we'll use a custom key
    Serial.print("TAIL_LIGHT:");
    Serial.println(state ? "1" : "0");
  }
}

void setBeamMode(BeamMode mode) {
  if (currentBeamMode != mode) {
    // Cancel any pending automatic beam mode changes when manually setting
    beamModeChangeRequested = false;
    
    currentBeamMode = mode;
    
    // Set low beam based on mode
    bool lowBeamState = (mode == BEAM_LOW);
    digitalWrite(LOW_BEAM_MOSFET_PIN, lowBeamState ? HIGH : LOW);
    Serial.print("LOWBEAM:");
    Serial.println(lowBeamState ? "1" : "0");
    
    // Set high beam based on mode
    bool highBeamState = (mode == BEAM_HIGH);
    digitalWrite(HIGH_BEAM_MOSFET_PIN, highBeamState ? HIGH : LOW);
    Serial.print("HIGHBEAM:");
    Serial.println(highBeamState ? "1" : "0");
    
    // Debug output
    const char* modeNames[] = {"OFF", "LOW", "HIGH"};
    Serial.print("Beam mode changed to: ");
    Serial.println(modeNames[mode]);
  }
}

JoystickDirection readJoystickDirection() {
  int joystickValue = analogRead(JOYSTICK_Y_PIN);
  joystickYValue = joystickValue; // Store for debug output
  
  if (joystickValue > JOYSTICK_UP_THRESHOLD) {
    return JOYSTICK_UP;
  } else if (joystickValue < JOYSTICK_DOWN_THRESHOLD) {
    return JOYSTICK_DOWN;
  } else {
    return JOYSTICK_CENTER;
  }
}

void startBeamFlash() {
  // Start beam flashing sequence
  if (!beamFlashInProgress) {
    beamFlashInProgress = true;
    beamFlashStartTime = millis();
    beamFlashStep = 0;
    
    // Store current beam states
    previousBeamMode = currentBeamMode;
    
    Serial.print("Beam flash started (Y=");
    Serial.print(joystickYValue);
    Serial.println(")");
  }
}

void toggleBeamMode() {
  // Toggle between LOW and HIGH beam modes only
  // If currently OFF, ignore joystick input
  if (currentBeamMode == BEAM_LOW) {
    setBeamMode(BEAM_HIGH);
    Serial.print("Switched to high beam (Y=");
    Serial.print(joystickYValue);
    Serial.println(")");
  } else if (currentBeamMode == BEAM_HIGH) {
    setBeamMode(BEAM_LOW);
    Serial.print("Switched to low beam (Y=");
    Serial.print(joystickYValue);
    Serial.println(")");
  }
  // If BEAM_OFF, do nothing (ignore joystick input)
}

void handleJoystick() {
  // Read joystick and get direction in one call
  JoystickDirection direction = readJoystickDirection();
  
  // Handle joystick up (beam flashing)
  if (direction == JOYSTICK_UP && !joystickUpPressed && (millis() - lastJoystickUpTime > JOYSTICK_DEBOUNCE_MS)) {
    joystickUpPressed = true;
    lastJoystickUpTime = millis();
    startBeamFlash();
  } else if (direction != JOYSTICK_UP) {
    joystickUpPressed = false;
  }
  
  // Handle joystick down (beam switching)
  if (direction == JOYSTICK_DOWN && !joystickDownPressed && (millis() - lastJoystickDownTime > JOYSTICK_DEBOUNCE_MS)) {
    joystickDownPressed = true;
    lastJoystickDownTime = millis();
    
    // Only switch if not currently flashing
    if (!beamFlashInProgress) {
      toggleBeamMode();
    }
  } else if (direction != JOYSTICK_DOWN) {
    joystickDownPressed = false;
  }
}

void performBeamFlash() {
  unsigned long elapsed = millis() - beamFlashStartTime;
  
  // Flash sequence: high-low-high-low (2 complete cycles)
  // Each flash is 300ms duration with 200ms pause (relay-friendly)
  
  switch (beamFlashStep) {
    case 0: // First flash - high beam
      if (elapsed >= 0) {
        setBeamMode(BEAM_HIGH);
        beamFlashStep = 1;
      }
      break;
      
    case 1: // First pause
      if (elapsed >= BEAM_FLASH_DURATION_MS) {
        setBeamMode(BEAM_LOW);
        beamFlashStep = 2;
      }
      break;
      
    case 2: // Second flash - low beam
      if (elapsed >= BEAM_FLASH_DURATION_MS + BEAM_FLASH_PAUSE_MS) {
        setBeamMode(BEAM_LOW);
        beamFlashStep = 3;
      }
      break;
      
    case 3: // Second pause
      if (elapsed >= 2 * (BEAM_FLASH_DURATION_MS + BEAM_FLASH_PAUSE_MS)) {
        setBeamMode(BEAM_OFF);
        beamFlashStep = 4;
      }
      break;
      
    case 4: // Third flash - high beam
      if (elapsed >= 2 * (BEAM_FLASH_DURATION_MS + BEAM_FLASH_PAUSE_MS) + BEAM_FLASH_PAUSE_MS) {
        setBeamMode(BEAM_HIGH);
        beamFlashStep = 5;
      }
      break;
      
    case 5: // Third pause
      if (elapsed >= 3 * (BEAM_FLASH_DURATION_MS + BEAM_FLASH_PAUSE_MS)) {
        setBeamMode(BEAM_LOW);
        beamFlashStep = 6;
      }
      break;
      
    case 6: // Fourth flash - low beam
      if (elapsed >= 3 * (BEAM_FLASH_DURATION_MS + BEAM_FLASH_PAUSE_MS) + BEAM_FLASH_PAUSE_MS) {
        setBeamMode(BEAM_LOW);
        beamFlashStep = 7;
      }
      break;
      
    case 7: // Final pause and restore
      if (elapsed >= 4 * (BEAM_FLASH_DURATION_MS + BEAM_FLASH_PAUSE_MS)) {
        // Restore previous beam mode
        setBeamMode(previousBeamMode);
        
        // End flashing sequence
        beamFlashInProgress = false;
        beamFlashStep = 0;
        
        Serial.println("Beam flash completed");
      }
      break;
  }
}
