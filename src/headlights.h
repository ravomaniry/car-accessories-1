#ifndef HEADLIGHTS_H
#define HEADLIGHTS_H

#include <Arduino.h>
#include "relay_config.h"

// Headlight configuration
extern const int PHOTOSENSOR_PIN;        // Photosensitive sensor DO pin
extern const int DRL_MOSFET_PIN;         // DRL (Daytime Running Lights) MOSFET control
extern const int TAIL_LIGHT_MOSFET_PIN;  // Tail light MOSFET control
extern const int LOW_BEAM_MOSFET_PIN;    // Low beam headlight MOSFET control
extern const int HIGH_BEAM_MOSFET_PIN;   // High beam headlight MOSFET control
extern const int JOYSTICK_Y_PIN;         // Joystick Y-axis analog pin

// Timing configuration
extern const unsigned long LIGHT_ON_DEBOUNCE_MS;   // Debounce time when turning lights ON (5 seconds)
extern const unsigned long LIGHT_OFF_DEBOUNCE_MS;  // Debounce time when turning lights OFF (1 minute)
extern const unsigned long DRL_TIMEOUT_MS;         // DRL timeout to avoid power competition during cranking (1 minute)

// Light level thresholds (configurable)
extern int LOW_LIGHT_THRESHOLD;    // Threshold for low light detection (0-1023)
extern int DARK_THRESHOLD;         // Threshold for dark detection (0-1023)

// Speed threshold
extern const float DRL_ACTIVATION_SPEED_THRESHOLD;  // Speed threshold for DRL activation

// Brightness level enum
enum BrightnessLevel {
  BRIGHT,      // Daytime conditions
  LOW_LIGHT,   // Low light conditions (dusk/dawn)
  DARK         // Dark conditions (night)
};

// Beam mode enum
enum BeamMode {
  BEAM_OFF,     // Both beams off
  BEAM_LOW,     // Low beam only
  BEAM_HIGH     // High beam only
};

// Joystick direction enum
enum JoystickDirection {
  JOYSTICK_CENTER,  // Center position (no action)
  JOYSTICK_UP,      // Up direction (beam flashing)
  JOYSTICK_DOWN     // Down direction (beam switching)
};

// Headlight state variables
extern bool drlActive;
extern bool tailLightActive;
extern BeamMode currentBeamMode;

// Headlight functions
void setupHeadlights();
void handleHeadlights();
void calculateDesiredLightStates();
void applyLightStateChanges();
void checkLightChange(bool desired, bool current, bool& changeRequested, unsigned long& changeRequestTime, bool& changeToOn, const char* lightName);
void applyIndividualLightChange(bool& changeRequested, unsigned long& changeRequestTime, bool& changeToOn, bool& currentState, void (*setFunction)(bool), const char* lightName);
bool isCarMoving();
int readLightLevel();
BrightnessLevel getBrightnessLevel();
void setDRL(bool state);
void setTailLight(bool state);
void setBeamMode(BeamMode mode);
void handleJoystick();
JoystickDirection readJoystickDirection();
void startBeamFlash();
void toggleBeamMode();
void performBeamFlash();

#endif
