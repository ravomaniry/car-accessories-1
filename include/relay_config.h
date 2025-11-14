#ifndef RELAY_CONFIG_H
#define RELAY_CONFIG_H

#include <Arduino.h>

// Provide fallbacks if compiling in a non-Arduino analysis context
#ifndef LOW
#define LOW 0x0
#endif
#ifndef HIGH
#define HIGH 0x1
#endif

// Active Low Relay Configuration
// These relays are activated when the control pin is pulled LOW
// and deactivated when the control pin is pulled HIGH

// Relay control values (active low)
constexpr int RELAY_ON = LOW;   // Pull pin LOW to activate relay
constexpr int RELAY_OFF = HIGH; // Pull pin HIGH to deactivate relay

#endif // RELAY_CONFIG_H
