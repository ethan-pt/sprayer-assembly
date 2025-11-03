#include <Arduino.h>
#include <ESP32Servo.h>

Servo sprayer1;
Servo sprayer2;
const int sprayer1Pin = 14;
const int sprayer2Pin = 13;
const int buttonPin = 12;

const int overuseDelay = 0; // TODO: Update as needed

void setup() {
  Serial.begin(115200);

  sprayer1.attach(sprayer1Pin, 500, 2400);
  sprayer2.attach(sprayer2Pin, 500, 2400);

  // make sure servos are in neutral position
  sprayer1.write(0);
  sprayer2.write(0);

  pinMode(buttonPin, INPUT_PULLUP);
}

void cycleSprayer(int cycles) {
  for (int i = 0; i < cycles; i++) {
    sprayer1.write(45);
    sprayer2.write(45);
    delay(250);

    sprayer1.write(0);
    sprayer2.write(0);
    delay(250);
  }
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    cycleSprayer(2);

    while (digitalRead(buttonPin) == LOW) {
      delay(10);
    }

    delay(overuseDelay);
  }
}
