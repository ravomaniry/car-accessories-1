#include "reverse.h"

// Reverse gear configuration
const byte REVERSE_GEAR_PIN = 3; // D3: Reverse gear switch input
const unsigned long REVERSE_GEAR_DEBOUNCE_MS = 100;

// Camera configuration
const int CAMERA_MOSFET_PIN = 4; // D4: Camera 12V MOSFET control
const int CAMERA_BUTTON_PIN = 5; // D5: Manual camera activation button
const unsigned long CAMERA_BUTTON_DEBOUNCE_MS = 200;
const unsigned long CAMERA_AUTO_OFF_TIMEOUT_MS = 30000; // 30 seconds
const unsigned long CAMERA_MANUAL_TIMEOUT_MS = 60000;   // 15 seconds

// Reverse gear state variables
static bool reverseGearEngaged = false;
static uint8_t reverseLastRawReading = HIGH;
static bool reverseStableState = HIGH;          // debounced state
static unsigned long reverseLastChangeMillis = 0;

// Camera state tracking
static bool cameraIsActive = false;
static bool cameraActivatedByReverse = false;
static bool cameraActivatedByButton = false;
static unsigned long cameraStartTime = 0;
static int cameraLastButtonState = HIGH;
static unsigned long cameraLastDebounceTime = 0;

void setupReverse() {
  // Setup reverse gear detection
  // Note: Uses external voltage divider (4.7kΩ pull-up + 1kΩ series resistor)
  // When reverse engaged: switch closes, pin reads LOW through 1kΩ resistor
  // When not in reverse: switch open, pin reads HIGH through 4.7kΩ pull-up
  pinMode(REVERSE_GEAR_PIN, INPUT);  // No internal pull-up, using external voltage divider

  // read initial state
  reverseLastRawReading = digitalRead(REVERSE_GEAR_PIN);
  reverseStableState = reverseLastRawReading;
  // For reverse gear switch: LOW = reverse engaged, HIGH = not in reverse
  reverseGearEngaged = (reverseStableState == LOW);

  reverseLastChangeMillis = millis();

  // Setup camera control
  // Set camera relay pin as output
  pinMode(CAMERA_MOSFET_PIN, OUTPUT);
  digitalWrite(CAMERA_MOSFET_PIN, RELAY_OFF); // Ensure camera is off initially

  // Set camera button pin as input (capacitive touch button with external pull-up)
  // Capacitive touch button: GND, VCC, I/O pins
  // I/O pin connected to Arduino input, VCC to +5V, GND to ground
  pinMode(CAMERA_BUTTON_PIN, INPUT); // No internal pull-up, capacitive button has its own pull-up
  
  // Send initial reverse gear status
  sendReverseStatus();
}

void handleReverse() {
  // Handle reverse gear detection
  uint8_t raw = digitalRead(REVERSE_GEAR_PIN);

  // if input changed, reset timer
  if (raw != reverseLastRawReading) {
    reverseLastChangeMillis = millis();
    reverseLastRawReading = raw;
  } else {
    // if stable long enough and different from stable state → update
    if (raw != reverseStableState && (millis() - reverseLastChangeMillis >= REVERSE_GEAR_DEBOUNCE_MS)) {
      reverseStableState = raw;
      // For reverse gear switch: LOW = reverse engaged, HIGH = not in reverse
      reverseGearEngaged = (reverseStableState == LOW);

      // Send reverse status immediately when state change is stable
      sendReverseStatus();

      // Handle camera activation based on reverse gear
      if (reverseGearEngaged) {
        activateCameraByReverse();
      } else {
        deactivateCameraByReverse();
      }
      
      // Reset the debounce timer after state change to prevent immediate re-triggering
      reverseLastChangeMillis = millis();
    }
  }

  // Handle manual camera button
  int buttonState = digitalRead(CAMERA_BUTTON_PIN);

  // If the switch changed, due to noise or pressing:
  if (buttonState != cameraLastButtonState) {
    // reset the debouncing timer
    cameraLastDebounceTime = millis();
  }

  if ((millis() - cameraLastDebounceTime) > CAMERA_BUTTON_DEBOUNCE_MS) {
    // Check if the capacitive touch button is touched (HIGH) and camera is not currently active
    // Note: This capacitive touch button reads HIGH when touched, LOW when not touched
    if (buttonState == HIGH && !cameraIsActive) {
      cameraIsActive = true;
      cameraActivatedByButton = true;
      cameraActivatedByReverse = false;
      cameraStartTime = millis();
      digitalWrite(CAMERA_MOSFET_PIN, RELAY_ON);
      Serial.println("Camera activated by capacitive touch button!");
    }
  }

  // Handle camera timeout logic
  if (cameraIsActive) {
    unsigned long elapsedTime = millis() - cameraStartTime;
    bool shouldTurnOff = false;

    if (cameraActivatedByButton && elapsedTime >= CAMERA_MANUAL_TIMEOUT_MS) {
      // Manual activation timeout (15 seconds)
      shouldTurnOff = true;
      Serial.println("Camera turned off - manual timeout (15 seconds)");
    } else if (!cameraActivatedByReverse && !cameraActivatedByButton && elapsedTime >= CAMERA_AUTO_OFF_TIMEOUT_MS) {
      // Auto-off timeout (1 minute) - only if not activated by reverse or button
      shouldTurnOff = true;
      Serial.println("Camera turned off - auto timeout (1 minute)");
    }

    if (shouldTurnOff) {
      cameraIsActive = false;
      cameraActivatedByButton = false;
      cameraActivatedByReverse = false;
      digitalWrite(CAMERA_MOSFET_PIN, RELAY_OFF);
    }
  }

  // Save the reading. Next time through the loop, it'll be the lastButtonState:
  cameraLastButtonState = buttonState;
}

bool isReverseGearEngaged() {
  return reverseGearEngaged;
}

void activateCameraByReverse() {
  if (!cameraIsActive) {
    // Reverse gear engaged - activate camera
    cameraIsActive = true;
    cameraActivatedByReverse = true;
    cameraActivatedByButton = false;
    cameraStartTime = millis();
    digitalWrite(CAMERA_MOSFET_PIN, RELAY_ON);
    Serial.println("Camera activated by reverse gear!");
  } else {
    // Camera is already active (e.g., counting down from previous disengagement)
    // Reset it to reverse-activated mode to cancel any countdown
    cameraActivatedByReverse = true;
    cameraActivatedByButton = false;
    cameraStartTime = millis(); // Reset timer
    Serial.println("Camera reactivated by reverse gear (was counting down)!");
  }
}

void deactivateCameraByReverse() {
  if (cameraActivatedByReverse) {
    // Reverse gear disengaged - start timeout countdown to turn off camera
    cameraActivatedByReverse = false;
    cameraStartTime = millis(); // Reset timer for auto-off
    Serial.println("Reverse gear disengaged - camera will turn off in 30 seconds");
  }
}

bool isCameraActive() {
  return cameraIsActive;
}

void sendReverseStatus() {
  // Send current reverse gear status in KEY:VALUE format
  if (reverseGearEngaged) {
    Serial.println("REVERSE:1");
  } else {
    Serial.println("REVERSE:0");
  }
}
