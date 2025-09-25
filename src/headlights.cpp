#include "headlights.h"
#include "gps.h"

// Headlight pin configuration
const int PHOTOSENSOR_PIN = A0;        // A0: Photosensitive sensor DO pin (analog input)
const int DRL_MOSFET_PIN = 11;        // D11: DRL MOSFET control
const int TAIL_LIGHT_MOSFET_PIN = 12; // D12: Tail light MOSFET control
const int LOW_BEAM_MOSFET_PIN = 13;   // D13: Low beam MOSFET control

// Timing configuration
const unsigned long HEADLIGHT_ON_DEBOUNCE_MS = 5000;   // 5 seconds debounce when turning lights ON
const unsigned long HEADLIGHT_OFF_DEBOUNCE_MS = 60000; // 1 minute debounce when turning lights OFF

// Light level thresholds (configurable - can be adjusted via serial commands)
int LOW_LIGHT_THRESHOLD = 300;    // Threshold for low light detection (0-1023)
int DARK_THRESHOLD = 150;         // Threshold for dark detection (0-1023)

// Speed threshold
const float DRL_ACTIVATION_SPEED_THRESHOLD = 5.0;  // Speed threshold for DRL activation

// Headlight state variables
bool drlActive = false;
bool tailLightActive = false;
bool lowBeamActive = false;

// Individual light debounce tracking
static unsigned long drlChangeTime = 0;
static bool drlChangePending = false;
static bool drlTurningOn = false;

static unsigned long tailLightChangeTime = 0;
static bool tailLightChangePending = false;
static bool tailLightTurningOn = false;

static unsigned long lowBeamChangeTime = 0;
static bool lowBeamChangePending = false;
static bool lowBeamTurningOn = false;

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
  
  Serial.println("Headlight system initialized");
  Serial.print("Low light threshold: ");
  Serial.println(LOW_LIGHT_THRESHOLD);
  Serial.print("Dark threshold: ");
  Serial.println(DARK_THRESHOLD);
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
      
      Serial.print("Conditions changed. Light: ");
      Serial.print(currentLightLevel);
      Serial.print(", Moving: ");
      Serial.println(currentCarMoving ? "YES" : "NO");
    }
    
    lastReadingTime = millis();
  }
  
  // Apply light state changes with appropriate debounce
  applyLightStateChanges();
}

void calculateDesiredLightStates() {
  int lightLevel = currentLightLevel;
  bool carMoving = currentCarMoving;
  
  // Determine light conditions
  bool isDay = lightLevel > LOW_LIGHT_THRESHOLD;
  bool isLowLight = lightLevel <= LOW_LIGHT_THRESHOLD && lightLevel > DARK_THRESHOLD;
  bool isDark = lightLevel <= DARK_THRESHOLD;
  
  // Calculate desired light states based on your requirements
  bool desiredDRL = false;
  bool desiredTailLight = false;
  bool desiredLowBeam = false;
  
  if (isDay) {
    // During the day: tail light OFF, low beam OFF
    desiredTailLight = false;
    desiredLowBeam = false;
    // DRL only when car is moving
    desiredDRL = carMoving;
  } else if (isLowLight) {
    // Low light: DRL and tail light ON (5 sec delay)
    desiredDRL = true;
    desiredTailLight = true;
    desiredLowBeam = false;
  } else if (isDark) {
    // Dark: DRL and tail light always ON
    desiredDRL = true;
    desiredTailLight = true;
    // Low beam only when car is moving
    desiredLowBeam = carMoving;
  }
  
  // Check each light individually for changes
  checkLightChange(desiredDRL, drlActive, drlChangePending, drlChangeTime, drlTurningOn, "DRL");
  checkLightChange(desiredTailLight, tailLightActive, tailLightChangePending, tailLightChangeTime, tailLightTurningOn, "Tail Light");
  checkLightChange(desiredLowBeam, lowBeamActive, lowBeamChangePending, lowBeamChangeTime, lowBeamTurningOn, "Low Beam");
}

void checkLightChange(bool desired, bool current, bool& changePending, unsigned long& changeTime, bool& turningOn, const char* lightName) {
  if (desired != current && !changePending) {
    changePending = true;
    changeTime = millis();
    turningOn = desired;
    Serial.print(lightName);
    Serial.print(" change requested - turning ");
    Serial.println(turningOn ? "ON" : "OFF");
  }
}

void applyLightStateChanges() {
  // Apply DRL changes
  applyIndividualLightChange(drlChangePending, drlChangeTime, drlTurningOn, drlActive, setDRL, "DRL");
  
  // Apply Tail Light changes
  applyIndividualLightChange(tailLightChangePending, tailLightChangeTime, tailLightTurningOn, tailLightActive, setTailLight, "Tail Light");
  
  // Apply Low Beam changes
  applyIndividualLightChange(lowBeamChangePending, lowBeamChangeTime, lowBeamTurningOn, lowBeamActive, setLowBeam, "Low Beam");
}

void applyIndividualLightChange(bool& changePending, unsigned long& changeTime, bool& turningOn, bool& currentState, void (*setFunction)(bool), const char* lightName) {
  if (!changePending) return;
  
  unsigned long debounceTime = turningOn ? HEADLIGHT_ON_DEBOUNCE_MS : HEADLIGHT_OFF_DEBOUNCE_MS;
  
  if (millis() - changeTime >= debounceTime) {
    setFunction(turningOn);
    currentState = turningOn;
    changePending = false;
    
    Serial.print(lightName);
    Serial.print(" ");
    Serial.print(turningOn ? "turned ON" : "turned OFF");
    Serial.println(" after debounce");
  }
}

bool isCarMoving() {
  float speed = getSpeed();
  return speed > DRL_ACTIVATION_SPEED_THRESHOLD;
}

int readLightLevel() {
  // Read analog value from photosensitive sensor
  // Higher values = more light, lower values = less light
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
