#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Konfigurasi Wi-Fi
char ssid[] = "";       
char pass[] = "";         

// Auth Token dari Blynk App
char auth[] = "";  

// Alamat server Blynk lokal
char server[] = ""; 
int port = ;                      

// Konfigurasi Sensor Gas
const int gasSensorPin = A0;  

// Konfigurasi Buzzer dan LED
const int buzzerPin = D2;  
const int ledPin = D1;     

// Batas tingkat bahaya gas (dalam PPM)
const float gasThreshold = 50.0;

// Variabel untuk menyimpan status sebelumnya
bool gasStatusBerbahaya = false; 

void setup() {
  Serial.begin(115200);
  
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  digitalWrite(buzzerPin, LOW);
  digitalWrite(ledPin, LOW);

  Blynk.begin(auth, ssid, pass, server, port);
  Serial.println("Menghubungkan ke Blynk Server...");
}

void loop() {
  Blynk.run();

  // Baca data sensor gas
  int gasValue = analogRead(gasSensorPin);
  float gasPPM = (gasValue / 1024.0) * 100;

  Blynk.virtualWrite(V0, gasPPM);

  // Cek apakah kondisi berubah
  bool statusSaatIni = gasPPM > gasThreshold;

  if (statusSaatIni != gasStatusBerbahaya) {
    gasStatusBerbahaya = statusSaatIni;

    if (gasStatusBerbahaya) {
      Serial.println("⚠️ Gas terdeteksi! Menyalakan alarm!");
      Blynk.notify("⚠️ Gas Terdeteksi! Waspada!");
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(ledPin, HIGH);
    } else {
      Serial.println("✅ Gas aman.");
      digitalWrite(buzzerPin, LOW);
      digitalWrite(ledPin, LOW);
    }
  }

  delay(1000); // Tunggu 1 detik sebelum membaca kembali
}
