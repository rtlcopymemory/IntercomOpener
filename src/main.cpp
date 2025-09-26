#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

int relayControlPin = 18;
int relayState = HIGH;

void setup() {
  pinMode(relayControlPin, OUTPUT);
}

void loop() {
  digitalWrite(relayControlPin, relayState);
  relayState ^= 1;
  sleep(2);
}
