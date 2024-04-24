#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

#define CHAT_ID "1526002982"
#define BOTtoken "6223491390:AAHRYiYFhslLaiXADFDDdqXVsg0Jr_7sUn4"

#define BUZZER_PIN D1
#define LED_PIN D2
#define MQ3_PIN A0

// Replace with your network credentials
const char* ssid = "Namrata";

const char* password = "123456789";

#ifdef ESP8266
  X509List cert(TELEGRAM_CERTIFICATE_ROOT);
#endif

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int buzzerState = LOW;
int ledState = LOW;

void setup() {
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);

#ifdef ESP8266
  configTime(0, 0, "pool.ntp.org");
  client.setTrustAnchors(&cert);
#endif

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

#ifdef ESP32
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
#endif

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float alcoholLevel = analogRead(MQ3_PIN) * 5.0 / 1024.0;

  if (alcoholLevel >= 0.2) {
    if (buzzerState == LOW) {
      digitalWrite(BUZZER_PIN, HIGH);
      buzzerState = HIGH;
    }

    if (ledState == LOW) {
      digitalWrite(LED_PIN, HIGH);
      ledState = HIGH;
    }
  } else {
    if (buzzerState == HIGH) {
      digitalWrite(BUZZER_PIN, LOW);
      buzzerState = LOW;
    }

    if (ledState == HIGH) {
      digitalWrite(LED_PIN, LOW);
      ledState = LOW;
    }
  }

  if (millis() > bot.last_message_received + 30000) {
    bot.sendMessage(CHAT_ID, "Alcohol level: " + String(alcoholLevel, 2) + " ppm", "");
  }
}
