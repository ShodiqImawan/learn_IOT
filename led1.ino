#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_AUTH_TOKEN ""

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "";        
char pass[] = "";    

int ledPin = 0; // GPIO0 (D3)

void setup() {
    Serial.begin(115200);
    Serial.println("Connecting to WiFi...");
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected!");
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    pinMode(ledPin, OUTPUT);
}

BLYNK_WRITE(V1) { 
    int pinValue = param.asInt();
    digitalWrite(ledPin, pinValue);
    Serial.print("LED State: ");
    Serial.println(pinValue);
}

void loop() {
    Blynk.run();
}
