#include <Servo.h>

const int irSensorPin = 3;       // IR sensor connected to D3
const int servoPin = 9;          // Servo connected to D9
Servo myServo;

bool objectDetected = false;
bool cooldown = false;
unsigned long actionTime = 0;

void setup() {
  pinMode(irSensorPin, INPUT);
  myServo.attach(servoPin);
  myServo.write(60);             // Start at 60°
}

void loop() {
  int sensorValue = digitalRead(irSensorPin);

  // When object is detected for the first time
  if (sensorValue == LOW && !objectDetected && !cooldown) {
    myServo.write(0);            // Move to 0°
    actionTime = millis();       // Start 5-second timer
    objectDetected = true;
  }

  // After 5 seconds at 0°, go back to 60°
  if (objectDetected && millis() - actionTime >= 5000) {
    myServo.write(60);           // Return to 60°
    cooldown = true;             // Start cooldown - ignore IR input
    objectDetected = false;
    actionTime = millis();       // Reset timer for cooldown period
  }

  // Wait for object to leave and 5 seconds to pass before reactivating
  if (cooldown && sensorValue == HIGH && millis() - actionTime >= 5000) {
    cooldown = false;            // Ready to detect again
  }
}
