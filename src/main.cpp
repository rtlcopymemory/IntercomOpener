#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "secret.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

int relayControlPin = 18;
int relayState = HIGH;

void disableBluetooth();
void disablePreSleep();
void enablePostSleep();
void handleMessages(int numNewMessages);

void setup() {
  Serial.begin(115200);

  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

  pinMode(relayControlPin, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org

  disableBluetooth();

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
}

void loop() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

  if (numNewMessages) {
    Serial.println("There's new messages");
    handleMessages(numNewMessages);
  }

  delay(500);
  digitalWrite(relayControlPin, LOW);
  delay(500);
}

void disableBluetooth() {
  btStop();
}

void handleMessages(int numNewMessages) {
  while (numNewMessages) {
    String chat_id = String(bot.messages[0].chat_id);
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    if (chat_id != ownerID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    String text = bot.messages[0].text;
    if (text == "/start") {
      String welcome = "Welcome.\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/open to open the door \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/open") {
      delay(1000);
      digitalWrite(relayControlPin, HIGH);
      bot.sendMessage(chat_id, "Door Opened, you lazy human 🤖", "");
      sleep(1);
      digitalWrite(relayControlPin, LOW);
    }
  }
}
