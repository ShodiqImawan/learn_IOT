#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define WLAN_SSID "Infinix NOTE 7" // Isi dengan SSID WiFi Anda
#define WLAN_PASS "PASSWORD" // Isi dengan Password WiFi Anda
#define AIO_SERVER "io.adafruit.com" // Isi dengan server Adafruit IO
#define AIO_SERVERPORT 1883
#define IO_USERNAME "USERNAME" // Isi dengan username Adafruit IO
#define IO_KEY "KEY" // Isi dengan key Adafruit IO

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, IO_USERNAME, IO_KEY);
Adafruit_MQTT_Publish relaytelegram = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/relay");
Adafruit_MQTT_Subscribe Kontroll_Relay = Adafruit_MQTT_Subscribe(&mqtt, IO_USERNAME "/feeds/relay");

void MQTT_connect();

int pin_relay = D2;
String Data;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println(F("Kontrol Relay"));
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  
  // Connect to WiFi
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Setup MQTT subscription
  mqtt.subscribe(&Kontroll_Relay);
  pinMode(pin_relay, OUTPUT);
  digitalWrite(pin_relay, 0);
}

uint32_t x = 0;

void loop() {
  MQTT_connect();

  // String message = bot.getNextMessage();
  // if (!message.isEmpty()) {
  //   Serial.print("Received message: ");
  //   Serial.println(message);

  //   // Contoh perintah:
  //   if (message.equalsIgnoreCase("")) {
  //     digitalWrite(relayPin, HIGH); // Nyalakan relay
  //     bot.sendMessage(chatID, "Relay dinyalakan");
  //   } else if (message.equalsIgnoreCase("mati")) {
  //     digitalWrite(relayPin, LOW); // Matikan relay
  //     bot.sendMessage(chatID, "Relay dimatikan");
  //   }
  // }
  
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &Kontroll_Relay) {
      Data = (char *)Kontroll_Relay.lastread;
      
      if (Data == "OFF") {
        digitalWrite(pin_relay, 0);
        Serial.println(Data);
      } else if (Data == "ON") {
        digitalWrite(pin_relay, 1);
        Serial.println(Data);
      }
    }
  }
}

void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
  return;
  }

  Serial.println("Connecting to MQTT...");
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);
    retries--;
    if (retries == 0) {
      while (1);
    }
  }
  Serial.println("MQTT Connected");
}
