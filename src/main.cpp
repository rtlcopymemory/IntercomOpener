#include <Arduino.h>

int myPin = 18;
int on = HIGH;

void setup() {
  pinMode(myPin, OUTPUT);
}

void loop() {
  digitalWrite(myPin, on);
  on ^= 1;
  sleep(2);
}
