// This is the test code from last year on the sonic sensor

// Define the Trig and Echo pin and Servo:
#define TRIG_PIN 9
#define ECHO_PIN 8
#include <Servo.h>

Servo myservo;

int pos = 0;

// Define the speed of sound in cm/us (microseconds)
const float speedOfSoundInCmPerUs = 0.0343;

void setup() {
  // Initialize Serial communication:
  Serial.begin(9600);
  // Initialize the TRIG_PIN as an output and the ECHO_PIN as an input:
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  myservo.attach(10); //set pin
}

void loop() {
  // Clear the TRIG_PIN by setting it LOW:
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  // Trigger the sensor by setting the TRIG_PIN high for 10 microseconds:
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Read the time it takes for the echo to return to the sensor:
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance in centimeters:
  float distance = duration * speedOfSoundInCmPerUs / 2;

  // Print the distance:
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Delay a bit to avoid spamming:
  delay(500);

  if (distance < 20) {
    for (int pos = 10; pos <= 180; pos += 1) {
      myservo.write(pos);
      delay(10);
    }
    delay(1000); // Adding a delay for smooth transition
    for (int pos = 180; pos >= 10; pos -= 1) {
      myservo.write(pos);
      delay(10);
    }
    delay(1000); // Adding a delay for smooth transition
  }

}
