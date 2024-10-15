#include <ESP8266WiFi.h>   // Library untuk menghubungkan ESP8266 ke WiFi
#include <PubSubClient.h>  // Library untuk berkomunikasi dengan MQTT

// Informasi jaringan WiFi
const char* ssid = "";     // Nama WiFi
const char* password = "";   // Password WiFi
const char* mqtt_server = ""; // Alamat IP server MQTT

// Deklarasi pin untuk sensor ultrasonik HC-SR04
const int trigPin = 14; // Pin Trigger (D5 - GPIO14)
const int echoPin = 12; // Pin Echo (D6 - GPIO12)

// Objek untuk WiFi dan MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Variabel untuk pesan MQTT
unsigned long lastMsg = 0;           // Menyimpan waktu terakhir pesan dikirim
#define MSG_BUFFER_SIZE 50           // Ukuran buffer pesan
char msg[MSG_BUFFER_SIZE];           // Buffer untuk pesan
long duration;                       // Durasi waktu pantulan dari sensor ultrasonik
float distance;                      // Jarak yang dihitung dalam cm

// Fungsi untuk menghubungkan ESP8266 ke WiFi
void setup_wifi() {
  delay(10);  // Tunggu sebentar

  Serial.println(); // Baris kosong di Serial Monitor
  Serial.print("Connecting to ");
  Serial.println(ssid);  // Cetak nama WiFi

  WiFi.mode(WIFI_STA);      // Atur mode WiFi sebagai Station
  WiFi.begin(ssid, password); // Hubungkan ke jaringan WiFi

  // Tunggu hingga ESP8266 terhubung ke WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");  // Cetak titik setiap 500ms hingga terhubung
  }

  Serial.println("\nWiFi connected");  // Berhasil terhubung
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  // Cetak alamat IP ESP8266
}

// Callback untuk menangani pesan MQTT yang diterima
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);  // Cetak topik pesan
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);  // Cetak isi pesan
  }
  Serial.println();

  // Menghidupkan/mematikan LED berdasarkan pesan yang diterima
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);  // Nyalakan LED (logika terbalik)
  } 
  if ((char)payload[0] == '0') {
    digitalWrite(BUILTIN_LED, HIGH); // Matikan LED
  }
}

// Fungsi untuk menyambung ulang ke MQTT jika koneksi terputus
void reconnect() {
  // Terus mencoba hingga terhubung ke server MQTT
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";  // Buat client ID unik
    clientId += String(random(0xffff), HEX);  // Tambahkan angka acak

    // Coba terhubung ke server MQTT
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");  // Berhasil terhubung

      // Kirim pesan awal dan subscribe ke topik tertentu
      client.publish("codersid/nodemcu/v1", "hello world");
      client.subscribe("codersid/nodemcu/v1");
    } else {
      // Jika gagal, cetak kode kesalahan dan coba lagi setelah 5 detik
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Fungsi untuk membaca jarak dari sensor ultrasonik HC-SR04
float getDistance() {
  digitalWrite(trigPin, LOW);  // Bersihkan pin Trigger
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);  // Kirim sinyal trigger
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);   // Matikan sinyal trigger

  duration = pulseIn(echoPin, HIGH);  // Baca durasi pantulan
  distance = (duration * 0.0343) / 2; // Hitung jarak dalam cm
  return distance;  // Kembalikan nilai jarak
}

// Fungsi setup untuk inisialisasi
void setup() {
  pinMode(BUILTIN_LED, OUTPUT);  // Atur LED sebagai output
  Serial.begin(9600);            // Mulai komunikasi serial pada 9600 baud
  setup_wifi();                  // Hubungkan ke WiFi
  client.setServer(mqtt_server, 1883);  // Atur alamat server MQTT dan port
  client.setCallback(callback);  // Atur fungsi callback untuk MQTT

  pinMode(trigPin, OUTPUT);  // Atur pin Trigger sebagai output
  pinMode(echoPin, INPUT);   // Atur pin Echo sebagai input
}

// Fungsi loop utama, akan terus berjalan
void loop() {
  if (!client.connected()) {
    reconnect();  // Sambung ulang jika terputus dari server MQTT
  }
  client.loop();  // Pertahankan koneksi MQTT

  unsigned long now = millis();  // Dapatkan waktu saat ini
  if (now - lastMsg > 2000) {    // Kirim pesan setiap 2 detik
    lastMsg = now;               // Simpan waktu saat ini

    float distanceCm = getDistance();  // Baca jarak dari sensor
    snprintf(msg, MSG_BUFFER_SIZE, "Distance: %.2f cm", distanceCm);  // Format pesan

    Serial.print("Publish message: ");
    Serial.println(msg);  // Cetak pesan ke Serial Monitor
    client.publish("codersid/nodemcu/v1", msg);  // Kirim pesan via MQTT
  }
}
