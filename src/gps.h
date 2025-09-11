#ifndef GPS_H
#define GPS_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// GPS configuration
extern const int GPS_RX_PIN;        // GPS TX pin connected to Arduino digital pin
extern const int GPS_TX_PIN;        // GPS TX pin connected to Arduino digital pin
extern const int GPS_BAUD_RATE;     // GPS module baud rate (usually 9600)
extern const unsigned long GPS_UPDATE_INTERVAL_MS; // How often to update GPS data

// GPS state variables
extern TinyGPSPlus gps;
extern SoftwareSerial gpsSerial;

// GPS functions
void setupGPS();
void handleGPS();
void sendGPSData();
bool isGPSValid();
float getSpeed();
void getLocation(float& latitude, float& longitude);

#endif
