#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include "secret.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  5           // Time ESP32 will go to sleep (in seconds)

WiFiClientSecure client;
UniversalTelegramBot bot(botToken, client);

IPAddress local_IP(192, 168, 1, 110);
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

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

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

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
  delay(1000);
  digitalWrite(relayControlPin, LOW);
  Serial.println("EEPY");
  esp_deep_sleep_start();
}

void disableBluetooth() {
  btStop();
}

void handleMessages(int numNewMessages) {
  Serial.print("Messages: ");
  Serial.println(String(numNewMessages));
  while (numNewMessages) {
    String chat_id = String(bot.messages[0].chat_id);
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

    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
  Serial.println("End");
}
