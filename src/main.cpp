#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include "credentials.h"


Servo sprayer1;
Servo sprayer2;
const int sprayer1Pin = 14;
const int sprayer2Pin = 13;
const int buttonPin = 12;

const int overuseDelay = 0; // TODO: Update as needed


bool connectWiFi(int timeoutSeconds) {
  WiFi.mode(WIFI_STA);
  WiFi.persistent(true);
  WiFi.setAutoReconnect(true);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  int maxAttempts = timeoutSeconds * 2;
  
  while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnection successful!");

    return true;
  } else {
    Serial.print("\nConnection failed with error ");
    Serial.print(WiFi.status());

    return false;
  }
}

void setup() {
  // ESP32 Setup
  Serial.begin(115200);
  delay(2500); // Gives Serial monitor time to connect or something
  Serial.println("\n\n««Starting ESP32»»");

  // Connect to wifi
  Serial.print("\n\nConnecting to WiFi.");

  bool wifiConnected = connectWiFi(30);
  if (!wifiConnected) {
    delay(2000);
    Serial.print("\nRetrying WiFi connection once.");
    wifiConnected = connectWiFi(30);
  }
  if (!wifiConnected) {
    Serial.print("Rebooting in 5 seconds.");
    delay(5000);
    esp_restart(); // Hardware reboot
  }

  // Connect hardware
  sprayer1.attach(sprayer1Pin, 500, 2400);
  sprayer2.attach(sprayer2Pin, 500, 2400);
  sprayer1.write(0); // make sure servos are in neutral position
  sprayer2.write(0);

  pinMode(buttonPin, INPUT_PULLUP);

  Serial.println("\nSetup complete!\nBeginning loop");
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
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Attempting reconnect.");

    if (!connectWiFi(15)) {
      Serial.println("Reconnection failed. Rebooting in 5 seconds.");
      delay(5000);
      esp_restart();
    }
  }

  if (digitalRead(buttonPin) == LOW) {
    cycleSprayer(2);

    while (digitalRead(buttonPin) == LOW) {
      delay(10);
    }

    delay(overuseDelay);
  }
}
