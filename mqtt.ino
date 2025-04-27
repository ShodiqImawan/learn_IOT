//#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>


<<<<<<< Updated upstream
const char* ssid = ""; //Nama WiFi
const char* password = ""; //Password WiFi
=======
const char* ssid = ""; //Nama WiFi
const char* password = ""; //Password WiFi
>>>>>>> Stashed changes
const char* mqtt_server = ""; //IP untuk mqtt server

// Pin untuk sensor ultrasonik HC-SR04
const int trigPin = 14; // Pin trigger
const int echoPin = 12; // Pin echo

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
long duration;
float distance;

void setup_wifi() {
  delay(10);
  //Mulai menyambungkan ke WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  //Cuman buat tambahan aja, sebenernya sih gak butuh - butuh banget
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on
  } 
  if ((char)payload[0] == '0') {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off
  }
}

void reconnect() {
  //Terus mencoba untuk tersambung ke jaringan
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("codersid/nodemcu/v1", "hello world");
      client.subscribe("codersid/nodemcu/v1");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Fungsi untuk mendapatkan jarak dari sensor ultrasonik
float getDistance() {

  Serial.print("Duration: ");
  Serial.println(duration);
  Serial.print("Calculated Distance: ");
  Serial.println(distance);


  
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = (duration * 0.0343) / 2; // Hitung jarak dalam cm
  return distance;                    // Tulisan nya hitung dalam cm tapi aslinya mah tampilnya dalam meter atau milimeter kurang tau dah
                                      // Pokoknya output jaraknya nanti tertulisnya cuman dalam m
                                      // Tapi kurang tau sih kalo di coba sekarang keluarnya m atau udah jadi cm
                                      // Soalnya nih kode juga saya udah update setelah comment ini di buat
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT); // Cuman buat nandai kalo udah tersambung atau gagal tersambung dan terus mencoba tersambung ke jaringan
  Serial.begin(9600);           // Aslinya mah gak butuh - butuh banget, toh soalnya udah ada output yang ngasih tau kalo tersambung ke jaringan atau enggak
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Set up the pins for the ultrasonic sensor
  // Kurang tau ini buat apaan
  // Intinya mah saya yang kerja, saya yang pengen berhasil dan keluar outputnya, dan saya juga yang dapet ilmunya
  // Jadi kalo di tanya ini buat apa saya bakal jawab "Buat input dan output dari sensor ultrasonik nya"
  // Gak tau bener atau enggak jawaban nya, toh saya paham nya cuman di situ
  // Yang penting mah jadi, toh saya juga belajarnya sendiri bukan dari orang lain kecuali ada masalah yang gak bisa saya resolve sendiri
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    float distanceCm = getDistance(); // Mendapatkan jarak dalam cm

    snprintf(msg, MSG_BUFFER_SIZE, "Distance: %.2f cm", distanceCm);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("codersid/nodemcu/v1", msg); // Mengirimkan jarak ke MQTT
  }
}
