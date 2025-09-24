#include "power_manager.h"

static bool autoradioEnabled = false;
static unsigned long startTime = 0;

void setupPowerManager() {
  pinMode(AUTORADIO_MOSFET_PIN, OUTPUT);
  digitalWrite(AUTORADIO_MOSFET_PIN, LOW);
  startTime = millis();
}

void handlePowerManager() {
  unsigned long currentTime = millis();
  unsigned long elapsed = currentTime - startTime;
  
  if (!autoradioEnabled && elapsed >= AUTORADIO_TIMEOUT_MS) {
    digitalWrite(AUTORADIO_MOSFET_PIN, HIGH);
    autoradioEnabled = true;
  }
}
