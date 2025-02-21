// Program Detektor Kebocoran Gas dengan Blynk
// Menggunakan Virtual Pin V0 untuk Gauge
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""
#define BLYNK_PRINT Serial // Debugging via Serial Monitor
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "";  // Ganti dengan token autentikasi dari Blynk
char ssid[] = "";  // Ganti dengan nama WiFi
char pass[] = "";  // Ganti dengan password WiFi

int buzzer = D2;  // Pin untuk buzzer
int smokeA0 = A0; // Pin untuk sensor MQ-2 atau MQ-135
int sensorThres = 600;  // Batas ambang gas

void setup() {
    pinMode(buzzer, OUTPUT);
    pinMode(smokeA0, INPUT);
    
    Serial.begin(9600);
    Blynk.begin(auth, ssid, pass);
}

void loop() {
    int analogSensor = analogRead(smokeA0); // Membaca nilai sensor

    Serial.print("Gas Level: ");
    Serial.println(analogSensor);

    Blynk.virtualWrite(V0, analogSensor); // Kirim data sensor ke Gauge di Blynk

    if (analogSensor > sensorThres) {
        digitalWrite(buzzer, HIGH); // Nyalakan buzzer jika ada kebocoran gas
        Serial.print("Gas Detected");
    } else {
        digitalWrite(buzzer, LOW); // Matikan buzzer jika aman
    }

    delay(1000); // Update setiap 1 detik
    Blynk.run(); // Jalankan Blynk
}
