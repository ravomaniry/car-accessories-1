#include "gps.h"

// GPS configuration
const int GPS_RX_PIN = 9;        // D9: GPS TX pin connected to Arduino digital pin
const int GPS_TX_PIN = 8;        // D8: GPS RX pin connected to Arduino digital pin  
const int GPS_BAUD_RATE = 9600;  // NEO-6M default baud rate
const unsigned long GPS_UPDATE_INTERVAL_MS = 1000; // Update every 1 second

// GPS objects
TinyGPSPlus gps;
SoftwareSerial gpsSerial(GPS_RX_PIN, GPS_TX_PIN);

// GPS state variables
static unsigned long lastGPSUpdate = 0;
static float lastSpeed = 0.0;
static float lastLatitude = 0.0;
static float lastLongitude = 0.0;

void setupGPS() {
  // Initialize GPS serial communication
  gpsSerial.begin(GPS_BAUD_RATE);
  
  // Reset last update time
  lastGPSUpdate = millis();
}

void handleGPS() {
  // Read GPS data from serial
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      // GPS data successfully parsed
      if (gps.location.isValid()) {
        lastLatitude = gps.location.lat();
        lastLongitude = gps.location.lng();
      }
      
      if (gps.speed.isValid()) {
        lastSpeed = gps.speed.kmph(); // Speed in km/h
      }
    }
  }
  
  // Send GPS data at regular intervals
  if (millis() - lastGPSUpdate >= GPS_UPDATE_INTERVAL_MS) {
    sendGPSData();
    lastGPSUpdate = millis();
  }
}

void sendGPSData() {
  if (isGPSValid()) {
    // Send speed data
    Serial.print("SPEED:");
    Serial.println(lastSpeed, 2);
    
    // Send location data
    Serial.print("LOCATION:");
    Serial.print(lastLatitude, 6);
    Serial.print(",");
    Serial.println(lastLongitude, 6);
  }
  // Don't send anything if GPS data is invalid
}

bool isGPSValid() {
  return gps.location.isValid() && gps.speed.isValid();
}

float getSpeed() {
  return lastSpeed;
}

void getLocation(float& latitude, float& longitude) {
  latitude = lastLatitude;
  longitude = lastLongitude;
}
