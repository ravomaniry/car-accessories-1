#include "power_manager.h"

// Simple state tracking
static bool autoradioEnabled = false;
static unsigned long startTime = 0;

void setupPowerManager() {
  // Configure autoradio MOSFET pin as output
  pinMode(AUTORADIO_MOSFET_PIN, OUTPUT);
  
  // Initially disable autoradio (MOSFET off = LOW)
  digitalWrite(AUTORADIO_MOSFET_PIN, LOW);
  
  // Record start time
  startTime = millis();
  
  Serial.println("Power Manager: Autoradio will enable after 30 seconds");
}

void handlePowerManager() {
  // If autoradio not enabled yet and timeout reached, enable it
  if (!autoradioEnabled && (millis() - startTime >= AUTORADIO_TIMEOUT_MS)) {
    digitalWrite(AUTORADIO_MOSFET_PIN, HIGH);
    autoradioEnabled = true;
    Serial.println("Power Manager: Autoradio ENABLED via MOSFET pin 10");
  }
}
