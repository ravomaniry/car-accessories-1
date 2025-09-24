#ifndef POWER_MANAGER_H
#define POWER_MANAGER_H

#include <Arduino.h>

const int AUTORADIO_MOSFET_PIN = 10;
const unsigned long AUTORADIO_TIMEOUT_MS = 60000UL;
void setupPowerManager();
void handlePowerManager();

#endif // POWER_MANAGER_H
