#ifndef HORN_H
#define HORN_H

#include <Arduino.h>

// Horn configuration
extern const int HORN_BUTTON_PIN;
extern const int HORN_MOSFET_PIN;
extern const unsigned long HORN_BUTTON_DEBOUNCE_MS;
extern const unsigned long HORN_MAX_DURATION_MS;

// Horn functions
void setupHorn();
void handleHorn();
bool isHornActive();
void activateHorn();
void deactivateHorn();

#endif
