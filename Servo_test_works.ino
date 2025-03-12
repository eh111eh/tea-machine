#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

// Pin Definitions (easily adjustable)
const int encoderPinA = 10;  // Encoder A (Phase A)
const int encoderPinB = 1;   // Encoder B (Phase B)

// Motor Setup
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *myMotor = AFMS.getMotor(1); 

// Encoder variables
volatile long encoderTicks = 0;
int motorState = 0;
unsigned long pauseStartTime = 0;
bool shouldChangeDirection = false;

void setup() {
  Serial.begin(9600);
  AFMS.begin();  // Initialize motor shield
  
  // Set up encoder pins
  pinMode(encoderPinA, INPUT_PULLUP);  
  pinMode(encoderPinB, INPUT_PULLUP);  
  
  // Attach interrupt for encoder A
  attachInterrupt(digitalPinToInterrupt(encoderPinA), onEncoderTick, CHANGE);
  
  myMotor->setSpeed(150);  // Increase motor speed for better movement
}

void loop() {
  Serial.print("Encoder Ticks: ");
  Serial.println(encoderTicks);
  
  // Motor state machine
  switch (motorState) {
    case 0:
      if (encoderTicks < 3000 && !shouldChangeDirection) {
        myMotor->run(FORWARD);
      } else {
        shouldChangeDirection = true;
        myMotor->run(RELEASE); // Stop the motor to handle overshoot
        if (encoderTicks >= 3000) {
          motorState = 1; 
          pauseStartTime = millis();
        }
      }
      break;
    case 1:
      if (millis() - pauseStartTime > 500) {
        motorState = 2;
        encoderTicks = 0; // Reset encoder ticks to handle overshoot correctly
        shouldChangeDirection = false;
      }
      break;
    case 2:
      if (encoderTicks > -3000 && !shouldChangeDirection) {
        myMotor->run(BACKWARD);
      } else {
        shouldChangeDirection = true;
        myMotor->run(RELEASE); 
        if (encoderTicks <= -3000) {
          motorState = 3; 
          pauseStartTime = millis();
        }
      }
      break;
    case 3:
      if (millis() - pauseStartTime > 1000) {
        motorState = 0;
        encoderTicks = 0; 
        shouldChangeDirection = false;
      }
      break;
  }
  delay(100); 
}

void onEncoderTick() {
  if (motorState == 0 || motorState == 1) {
    encoderTicks++;
  } else if (motorState == 2 || motorState == 3) {
    encoderTicks--;
  }
}
