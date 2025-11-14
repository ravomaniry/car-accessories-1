#include <Arduino.h>
#include "reverse.h"
#include "horn.h"
#include "gps.h"
#include "headlights.h"

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  Serial.println("Car Accessories System Starting...");
  
  // Initialize reverse gear and camera module
  setupReverse();
  Serial.println("Reverse gear and camera module initialized");
  
  // Initialize horn module
  setupHorn();
  Serial.println("Horn module initialized");
  
  // Initialize GPS module
  setupGPS();
  Serial.println("GPS module initialized"); 
  
  // Initialize headlight system
  setupHeadlights();
  Serial.println("Headlight system initialized");
  
  Serial.println("System ready!");
}

void loop() {
  // Handle reverse gear detection, camera activation, and camera controls
  handleReverse();
  
  // Handle horn control
  handleHorn();
  
  // Handle GPS data collection and transmission
  handleGPS();
  
  // Handle headlight control
  handleHeadlights();
  
  // Small delay to prevent overwhelming the system
  delay(10);
}
