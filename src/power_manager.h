#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <Arduino.h>

// Power management configuration
const int AUTORADIO_MOSFET_PIN = 10;           // Pin 10 controls autoradio MOSFET
const unsigned long AUTORADIO_TIMEOUT_MS = 60 * 1000; // 60 seconds

// Function declarations
void setupPowerManager();
void handlePowerManager();

#endif // POWER_MANAGER_H
