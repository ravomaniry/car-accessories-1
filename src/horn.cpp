#include "horn.h"

// Horn configuration
const int HORN_BUTTON_PIN = 6; // D6: Capacitive touch button for horn activation
const int HORN_MOSFET_PIN = 12; // D12: Horn 12V MOSFET control
const unsigned long HORN_BUTTON_DEBOUNCE_MS = 5;
const unsigned long HORN_MAX_DURATION_MS = 5000; // Maximum 5 seconds continuous horn

// Horn state variables
static bool hornIsActive = false;
static bool hornButtonPressed = false;
static int hornLastButtonState = HIGH;
static unsigned long hornLastDebounceTime = 0;
static unsigned long hornStartTime = 0;

void setupHorn() {
  // Setup horn MOSFET control
  pinMode(HORN_MOSFET_PIN, OUTPUT);
  digitalWrite(HORN_MOSFET_PIN, LOW); // Ensure horn is off initially

  // Setup capacitive touch button for horn
  // Capacitive touch button: GND, VCC, I/O pins
  // I/O pin connected to Arduino input, VCC to +5V, GND to ground
  pinMode(HORN_BUTTON_PIN, INPUT); // No internal pull-up, capacitive button has its own pull-up
  
  // Read initial button state
  hornLastButtonState = digitalRead(HORN_BUTTON_PIN);
}

void handleHorn() {
  // Read capacitive touch button state
  int buttonState = digitalRead(HORN_BUTTON_PIN);

  // If the button state changed, reset the debouncing timer
  if (buttonState != hornLastButtonState) {
    hornLastDebounceTime = millis();
  }

  // Check if button state has been stable long enough
  if ((millis() - hornLastDebounceTime) > HORN_BUTTON_DEBOUNCE_MS) {
    // Capacitive touch button reads HIGH when touched, LOW when not touched
    bool buttonPressed = (buttonState == HIGH);
    
    if (buttonPressed && !hornButtonPressed) {
      // Button just pressed - activate horn
      activateHorn();
      hornButtonPressed = true;
    } else if (!buttonPressed && hornButtonPressed) {
      // Button just released - deactivate horn
      deactivateHorn();
      hornButtonPressed = false;
    }
  }

  // Safety timeout - prevent horn from running too long
  if (hornIsActive && (millis() - hornStartTime) >= HORN_MAX_DURATION_MS) {
    deactivateHorn();
    Serial.println("Horn turned off - maximum duration reached (5 seconds)");
  }

  // Save the reading for next iteration
  hornLastButtonState = buttonState;
}

bool isHornActive() {
  return hornIsActive;
}

void activateHorn() {
  if (!hornIsActive) {
    hornIsActive = true;
    hornStartTime = millis();
    digitalWrite(HORN_MOSFET_PIN, HIGH);
    Serial.println("Horn activated!");
  }
}

void deactivateHorn() {
  if (hornIsActive) {
    hornIsActive = false;
    digitalWrite(HORN_MOSFET_PIN, LOW);
    Serial.println("Horn deactivated!");
  }
}
