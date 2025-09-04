#include <Arduino.h>
#include "reverse.h"

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("Car Accessories System Starting...");
  
  // Initialize reverse gear and camera module
  setupReverse();
  Serial.println("Reverse gear and camera module initialized");
  
  Serial.println("System ready!");
}

void loop() {
  // Handle reverse gear detection, camera activation, and camera controls
  handleReverse();
  
  // Small delay to prevent overwhelming the system
  delay(10);
}
