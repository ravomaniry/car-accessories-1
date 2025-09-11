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
  
  Serial.println("GPS module initialized");
  Serial.println("Waiting for GPS signal...");
  Serial.println("Debug: GPS serial communication started on pins D9(RX) and D8(TX)");
  
  // Test GPS connection
  delay(1000);
  Serial.println("Testing GPS connection...");
  if (gpsSerial.available()) {
    Serial.println("SUCCESS: GPS data detected!");
  } else {
    Serial.println("WARNING: No GPS data detected - check wiring!");
  }
  
  // Reset last update time
  lastGPSUpdate = millis();
}

void handleGPS() {
  static unsigned long lastDebugTime = 0;
  static int rawDataCount = 0;
  static int parsedDataCount = 0;
  
  // Read GPS data from serial
  while (gpsSerial.available() > 0) {
    rawDataCount++;
    if (gps.encode(gpsSerial.read())) {
      parsedDataCount++;
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
  
  // Debug information every 5 seconds
  if (millis() - lastDebugTime >= 5000) {
    Serial.print("Debug: GPS raw data received: ");
    Serial.print(rawDataCount);
    Serial.print(", parsed sentences: ");
    Serial.print(parsedDataCount);
    Serial.print(", satellites: ");
    Serial.print(gps.satellites.value());
    Serial.print(", location valid: ");
    Serial.print(gps.location.isValid() ? "YES" : "NO");
    Serial.print(", speed valid: ");
    Serial.print(gps.speed.isValid() ? "YES" : "NO");
    if (gps.location.isValid()) {
      Serial.print(", lat: ");
      Serial.print(gps.location.lat(), 6);
      Serial.print(", lng: ");
      Serial.print(gps.location.lng(), 6);
    }
    Serial.println();
    
    lastDebugTime = millis();
    rawDataCount = 0;
    parsedDataCount = 0;
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
