#include "headlights.h"
#include "gps.h"
#include <Arduino.h>

// Headlight pin configuration
const int PHOTOSENSOR_PIN = A0;        // A0: Photosensitive sensor DO pin (analog input)
const int DRL_MOSFET_PIN = 11;        // D11: DRL MOSFET control
const int TAIL_LIGHT_MOSFET_PIN = 12; // D12: Tail light MOSFET control
const int LOW_BEAM_MOSFET_PIN = 13;   // D13: Low beam MOSFET control

// Timing configuration
const unsigned long LIGHT_ON_DEBOUNCE_MS = 5000;   // 5 seconds debounce when turning lights ON
const unsigned long LIGHT_OFF_DEBOUNCE_MS = 60000; // 1 minute debounce when turning lights OFF
const unsigned long DRL_TIMEOUT_MS = 60000;        // 1 minute DRL timeout to avoid power competition during cranking

// Light level thresholds (configurable - can be adjusted via serial commands)
int LOW_LIGHT_THRESHOLD = 300;    // Threshold for low light detection (0-1023)
int DARK_THRESHOLD = 150;         // Threshold for dark detection (0-1023)

// Speed threshold
const float DRL_ACTIVATION_SPEED_THRESHOLD = 5.0;  // Speed threshold for DRL activation

// Headlight state variables
bool drlActive = false;
bool tailLightActive = false;
bool lowBeamActive = false;

// Individual light state change tracking
static unsigned long drlChangeRequestTime = 0;
static bool drlChangeRequested = false;
static bool drlChangeToOn = false;

static unsigned long tailLightChangeRequestTime = 0;
static bool tailLightChangeRequested = false;
static bool tailLightChangeToOn = false;

static unsigned long lowBeamChangeRequestTime = 0;
static bool lowBeamChangeRequested = false;
static bool lowBeamChangeToOn = false;

// DRL timeout tracking
static unsigned long drlStartTime = 0;

// Current stable states
static int currentLightLevel = 0;
static bool currentCarMoving = false;

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
  
  // Initialize all lights to OFF
  digitalWrite(DRL_MOSFET_PIN, LOW);
  digitalWrite(TAIL_LIGHT_MOSFET_PIN, LOW);
  digitalWrite(LOW_BEAM_MOSFET_PIN, LOW);
  
  // Initialize current states
  currentLightLevel = readLightLevel();
  currentCarMoving = isCarMoving();
  
  // Initialize DRL timeout for power management during cranking
  drlStartTime = millis() + DRL_TIMEOUT_MS;
  
  // Headlight system initialized
}

void handleHeadlights() {
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
  bool desiredLowBeam = false;
  
  switch (brightness) {
    case BRIGHT:
      // During the day: tail light OFF, low beam OFF
      desiredTailLight = false;
      desiredLowBeam = false;
      // DRL ON only if within timeout period (set at startup)
      desiredDRL = (millis() < drlStartTime);
      break;
      
    case LOW_LIGHT:
      // Low light: DRL and tail light ON (5 sec delay)
      desiredDRL = true;
      desiredTailLight = true;
      desiredLowBeam = false;
      // Reset DRL timeout when conditions change
      drlStartTime = 0;
      break;
      
    case DARK:
      // Dark: DRL and tail light always ON
      desiredDRL = true;
      desiredTailLight = true;
      // Low beam always ON in dark (modern car behavior)
      desiredLowBeam = true;
      // Reset DRL timeout when conditions change
      drlStartTime = 0;
      break;
  }
  
  // Check each light individually for changes
  checkLightChange(desiredDRL, drlActive, drlChangeRequested, drlChangeRequestTime, drlChangeToOn, "DRL");
  checkLightChange(desiredTailLight, tailLightActive, tailLightChangeRequested, tailLightChangeRequestTime, tailLightChangeToOn, "Tail Light");
  checkLightChange(desiredLowBeam, lowBeamActive, lowBeamChangeRequested, lowBeamChangeRequestTime, lowBeamChangeToOn, "Low Beam");
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
  
  // Apply Low Beam changes
  applyIndividualLightChange(lowBeamChangeRequested, lowBeamChangeRequestTime, lowBeamChangeToOn, lowBeamActive, setLowBeam, "Low Beam");
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

void setLowBeam(bool state) {
  if (lowBeamActive != state) {
    lowBeamActive = state;
    digitalWrite(LOW_BEAM_MOSFET_PIN, state ? HIGH : LOW);
    Serial.print("LOWBEAM:");
    Serial.println(state ? "1" : "0");
  }
}
