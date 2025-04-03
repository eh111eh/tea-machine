#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

// Create the Motor Shield object
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
// Get motor channel 1 (using the M1 and M2 terminals on the shield)
Adafruit_DCMotor *myMotor = AFMS.getMotor(1);

// Encoder pin definitions (must use interrupt-capable pins on an Uno)
const int encoderPinA = 2;
const int encoderPinB = 3;

// Volatile variable to store encoder ticks (changed in ISR)
volatile long encoderTicks = 0;

// State machine variables for motor control
int motorState = 0;
unsigned long pauseStartTime = 0;
bool shouldChangeDirection = false;

//---------------------------------------------------------------------
// Interrupt Service Routines for quadrature encoder decoding
//---------------------------------------------------------------------
void handleEncoderA() {
  // Read both channels to determine rotation direction.
  if (digitalRead(encoderPinA) == digitalRead(encoderPinB)) {
    encoderTicks++;  // Forward rotation
  } else {
    encoderTicks--;  // Reverse rotation
  }
}

void handleEncoderB() {
  // Mirror the logic for the second channel.
  if (digitalRead(encoderPinA) == digitalRead(encoderPinB)) {
    encoderTicks--;  // Reverse rotation
  } else {
    encoderTicks++;  // Forward rotation
  }
}

//---------------------------------------------------------------------
// Setup
//---------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  
  // Initialize the motor shield (the default frequency is 1600 Hz)
  AFMS.begin();
  
  // Set up the encoder pins as inputs with internal pullups
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  
  // Attach interrupts for both encoder channels (using CHANGE mode)
  attachInterrupt(digitalPinToInterrupt(encoderPinA), handleEncoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), handleEncoderB, CHANGE);
  
  // Set an initial (low) motor speed for safety; you can adjust this later
  myMotor->setSpeed(20);
  myMotor->run(RELEASE); // Make sure the motor is stopped initially
}

//---------------------------------------------------------------------
// Main loop: state machine for motor movement
//---------------------------------------------------------------------
void loop() {
  // Debug: Print the encoder tick count
  Serial.print("Encoder Ticks: ");
  Serial.println(encoderTicks);
  
  switch (motorState) {
    case 0:
      // Run forward until we reach +3000 ticks
      if (encoderTicks < 3000 && !shouldChangeDirection) {
        myMotor->run(FORWARD);
      } else {
        // Stop motor and prepare to change direction
        shouldChangeDirection = true;
        myMotor->run(RELEASE);
        if (encoderTicks >= 3000) {
          motorState = 1;
          pauseStartTime = millis();
        }
      }
      break;
      
    case 1:
      // Pause for 500ms
      if (millis() - pauseStartTime > 500) {
        motorState = 2;
        encoderTicks = 0;  // Reset encoder count for backward motion
        shouldChangeDirection = false;
      }
      break;
      
    case 2:
      // Run backward until we reach -3000 ticks
      if (encoderTicks > -3000 && !shouldChangeDirection) {
        myMotor->run(BACKWARD);
      } else {
        // Stop motor and prepare for final pause
        shouldChangeDirection = true;
        myMotor->run(RELEASE);
        if (encoderTicks <= -3000) {
          motorState = 3;
          pauseStartTime = millis();
        }
      }
      break;
      
    case 3:
      // Pause for 1000ms before restarting the cycle
      if (millis() - pauseStartTime > 1000) {
        motorState = 0;
        encoderTicks = 0;
        shouldChangeDirection = false;
      }
      break;
  }
  
  delay(100); // Small delay for debugging output
}
