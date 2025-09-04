#ifndef REVERSE_H
#define REVERSE_H

#include <Arduino.h>

// Reverse gear configuration
extern const byte REVERSE_GEAR_PIN;
extern const unsigned long REVERSE_GEAR_DEBOUNCE_MS;

// Camera configuration
extern const int CAMERA_MOSFET_PIN;
extern const int CAMERA_BUTTON_PIN;
extern const unsigned long CAMERA_BUTTON_DEBOUNCE_MS;
extern const unsigned long CAMERA_AUTO_OFF_TIMEOUT_MS;
extern const unsigned long CAMERA_MANUAL_TIMEOUT_MS;

// Reverse gear functions
void setupReverse();
void handleReverse();
bool isReverseGearEngaged();

// Camera functions
void activateCameraByReverse();
void deactivateCameraByReverse();
bool isCameraActive();

#endif
