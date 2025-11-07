#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "credentials.h"
#include "apiSecrets.h"


Servo sprayer1;
Servo sprayer2;
const int sprayer1Pin = 14;
const int sprayer2Pin = 13;
const int buttonPin = 12;

const char* apiPath = "/api/v1/dispense/request";
const int requestTimeout = 15000; // TODO: Update as needed

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
    Serial.println("Rebooting in 5 seconds.\n");
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

bool requestSprayPermission() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  String serverUrl = String("http://") + SERVER_IP + ":" SERVER_PORT + apiPath;

  HTTPClient http;
  http.begin(serverUrl);
  http.setTimeout(requestTimeout);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("X-API-Key", API_KEY);

  // Create and send JSON payload
  StaticJsonDocument<200> requestDoc;
  requestDoc["device_id"] = DEVICE_ID;

  String requestBody;
  serializeJson(requestDoc, requestBody);

  int httpResponseCode = http.POST(requestBody);

  Serial.println("Sending request to server.");

  // Handle failed reponse errors
  if (httpResponseCode != 200) {
    Serial.print("\nHTTP Error: ");
    Serial.print(httpResponseCode);

    if (httpResponseCode == 401) {
      Serial.print("  (Authentication failed)\n");
    } else if (httpResponseCode == -11) {
      Serial.print("  (Request timed out)\n");
    } else if (httpResponseCode == -1) {
      Serial.print("  (Couldn't connect to server)\n");
    } else {
      Serial.print("\n");
    }

    http.end();
    return false;
  }

  String response = http.getString();
  StaticJsonDocument<200> responseDoc;

  // Handle successful response errors
  DeserializationError error = deserializeJson(responseDoc, response);
  if (error) {
    Serial.print("\nThere was a problem parsing the response: ");
    Serial.print(error.c_str());
    http.end();
    return false;
  }

  if (!responseDoc.containsKey("allowed")) {
    Serial.println("Error: \"allowed\" key not found in response object.");
    http.end();
    return false;
  }

  bool allowed = responseDoc["allowed"];
  http.end();
  return allowed;
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
  // WiFi reconnection handler
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Attempting reconnect.");

    if (!connectWiFi(15)) {
      Serial.println("Reconnection failed. Rebooting in 5 seconds.");
      delay(5000);
      esp_restart();
    }
  }

  // Button press handler
  if (digitalRead(buttonPin) == LOW) {
    if (requestSprayPermission()) {
      Serial.println("Permission granted. Spraying now");
      cycleSprayer(2);
    } else {
      Serial.println("Permission denied.");
    }

    while (digitalRead(buttonPin) == LOW) {
      delay(10);
    }

    delay(overuseDelay);
  }
}
