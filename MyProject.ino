// Define ultrasonic sensor pins
#define TRIG_PIN 9
#define ECHO_PIN 8

// Include libraries
#include <Servo.h>

// Servo motor for pushing the teabag
// Assume servo motor is connected to pin 10 on the Arduino board
Servo teabagServo;
const int TEABAG_SERVO_PIN = 10;

// Define button and LED indicators
// Assume the button is connected to pin 4 and LED is connected to pin 13 on the Arduino board
// LED turns ON when the machine is operating and turns OFF when it's idle
const int START_BUTTON = 4;
const int LED_INDICATOR = 13;

// Define ultrasonic sensor speed of sound
const float speedOfSoundInCmPerUs = 0.0343;

void setup() {
  // Serial communication
  Serial.begin(9600);
  
  // Ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Attach servo motor
  teabagServo.attach(TEABAG_SERVO_PIN);

  // Set button as input
  pinMode(START_BUTTON, INPUT_PULLUP);
  
  // Set LED as output
  pinMode(LED_INDICATOR, OUTPUT);

  Serial.println("Tea Machine Ready");
}

void loop() {
  // Check for a cup using ultrasonic sensor
  float distance = getDistance();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance < 20) {  
    dropTeabag();
  }

  delay(1000);
}

// Function to get distance from ultrasonic sensor
// Detects if a cup is placed under the teabag dispenser
float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * speedOfSoundInCmPerUs / 2;
}

// Function to push the teabag using a servo motor (motor returns to its initial position afterwards)
void dropTeabag() {
  Serial.println("Dropping teabag...");
  
  // Rotate the servo to push the teabag
  teabagServo.write(90); // Move to push position
  delay(500);
  
  // Return to original position
  teabagServo.write(0);  
  delay(500);
}

