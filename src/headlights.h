#ifndef HEADLIGHTS_H
#define HEADLIGHTS_H

#include <Arduino.h>

// Headlight configuration
extern const int PHOTOSENSOR_PIN;        // Photosensitive sensor DO pin
extern const int DRL_MOSFET_PIN;         // DRL (Daytime Running Lights) MOSFET control
extern const int TAIL_LIGHT_MOSFET_PIN;  // Tail light MOSFET control
extern const int LOW_BEAM_MOSFET_PIN;    // Low beam headlight MOSFET control

// Timing configuration
extern const unsigned long HEADLIGHT_ON_DEBOUNCE_MS;   // Debounce time when turning lights ON (5 seconds)
extern const unsigned long HEADLIGHT_OFF_DEBOUNCE_MS;  // Debounce time when turning lights OFF (1 minute)

// Light level thresholds (configurable)
extern int LOW_LIGHT_THRESHOLD;    // Threshold for low light detection (0-1023)
extern int DARK_THRESHOLD;         // Threshold for dark detection (0-1023)

// Speed threshold
extern const float DRL_ACTIVATION_SPEED_THRESHOLD;  // Speed threshold for DRL activation

// Headlight state variables
extern bool drlActive;
extern bool tailLightActive;
extern bool lowBeamActive;

// Headlight functions
void setupHeadlights();
void handleHeadlights();
void calculateDesiredLightStates();
void applyLightStateChanges();
void checkLightChange(bool desired, bool current, bool& changePending, unsigned long& changeTime, bool& turningOn, const char* lightName);
void applyIndividualLightChange(bool& changePending, unsigned long& changeTime, bool& turningOn, bool& currentState, void (*setFunction)(bool), const char* lightName);
bool isCarMoving();
int readLightLevel();
void setDRL(bool state);
void setTailLight(bool state);
void setLowBeam(bool state);

#endif
