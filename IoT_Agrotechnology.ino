#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <AsyncTCP.h>
#include <LittleFS.h>
#include <PubSubClient.h>

// --- Konfigurasi Pin ---
#define DHTPIN 4
#define DHTTYPE DHT21
#define SOIL_PIN 34
#define RELAY1_PIN 26
#define RELAY2_PIN 25
#define RELAY3_PIN 17

#define TRIG_PIN 18
#define ECHO_PIN 5

#define LED_GREEN 23
#define LED_YELLOW 22
#define LED_RED 21

// --- Konfigurasi WiFi & ThingsBoard ---
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* tb_server = "thingsboard.cloud";
const int tb_port = 1883;
const char* tb_token = "BxAEfPxNaNBmMHDVLqgB";

// --- Objek Library ---
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 20, 4);
AsyncWebServer server(80);
WiFiClient espClient;
PubSubClient client(espClient);

// --- Setup ---
void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0); lcd.print("Selamat Datang!");
  lcd.setCursor(0, 1); lcd.print("WS Agroteknologi IoT");
  lcd.setCursor(3, 3); lcd.print("-- UG MURO --");
  delay(5000);
  lcd.clear();

  pinMode(SOIL_PIN, INPUT);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW);
  digitalWrite(RELAY3_PIN, LOW);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Terhubung!");
  Serial.println(WiFi.localIP());

  client.setServer(tb_server, tb_port);
  client.setCallback(callback);

  if (!LittleFS.begin()) {
    Serial.println("Gagal mounting LittleFS");
    return;
  }

  // Web Server
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
    float suhu = dht.readTemperature();
    float kelembaban = dht.readHumidity();
    int soil = analogRead(SOIL_PIN);
    int soilPct = map(soil, 2048, 0, 0, 100);
    soilPct = constrain(soilPct, 0, 100);
    String json = "{\"temperature\":" + String(suhu) +
                  ",\"humidity\":" + String(kelembaban) +
                  ",\"soilPercentage\":" + String(soilPct) + "}";
    request->send(200, "application/json", json);
  });

  server.begin();
}

// --- Loop ---
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float suhu = dht.readTemperature();
  float kelembaban = dht.readHumidity();
  int soilVal = analogRead(SOIL_PIN);
  int soilPct = map(soilVal, 2048, 0, 0, 100);
  soilPct = constrain(soilPct, 0, 100);
  float jarak = getDistanceCM();

  // LCD (tanpa menampilkan jarak)
  lcd.setCursor(5, 0); lcd.print("Monitoring");
  lcd.setCursor(0, 1); lcd.print("Suhu   : ");
  lcd.setCursor(8, 1); lcd.print(suhu);
  lcd.setCursor(17, 1); lcd.print("C");

  lcd.setCursor(0, 2); lcd.print("K.Udara: ");
  lcd.setCursor(8, 2); lcd.print(kelembaban);
  lcd.setCursor(17, 2); lcd.print("%");

  lcd.setCursor(0, 3); lcd.print("K.Tanah: ");
  lcd.setCursor(9, 3); lcd.print("     ");
  lcd.setCursor(9, 3); lcd.print(soilPct);
  lcd.setCursor(17, 3); lcd.print("%");

  // Kontrol Relay
  digitalWrite(RELAY1_PIN, (suhu > 24 && kelembaban > 70) ? HIGH : LOW);
  digitalWrite(RELAY3_PIN, (soilPct < 30) ? HIGH : LOW);

  // Kontrol LED berdasarkan jarak
  kontrolLED(jarak);

  // Kirim ke ThingsBoard
  String data = "{";
  data += "\"Suhu\":" + String(suhu) + ",";
  data += "\"Kelembaban\":" + String(kelembaban) + ",";
  data += "\"Tinggi Air\":" + String(jarak) + ",";
  data += "\"Soil Moisture\":" + String(soilPct);
  data += "}";
  char payload[data.length() + 1];
  data.toCharArray(payload, sizeof(payload));
  client.publish("v1/devices/me/telemetry", payload);

  Serial.println("Data terkirim ke ThingsBoard.");
  delay(5000);
}

// --- Fungsi Jarak Ultrasonik ---
float getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.034 / 2.0;
}

// --- Fungsi LED Berdasarkan Jarak ---
void kontrolLED(float jarak) {
  if (jarak > 30) {
    setLED(HIGH, LOW, LOW); // Merah
  } else if (jarak >= 15 && jarak <= 30) {
    setLED(LOW, HIGH, LOW); // Kuning
  } else {
    setLED(LOW, LOW, HIGH); // Hijau
  }
}

void setLED(bool red, bool yellow, bool green) {
  digitalWrite(LED_RED, red);
  digitalWrite(LED_YELLOW, yellow);
  digitalWrite(LED_GREEN, green);
}

// --- Callback MQTT ---
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Pesan dari topic: ");
  Serial.println(topic);
}

// --- Reconnect MQTT ---
void reconnect() {
  while (!client.connected()) {
    Serial.print("Menghubungkan ke ThingsBoard...");
    if (client.connect("ESP32Client", tb_token, "")) {
      Serial.println("Berhasil");
    } else {
      Serial.print("Gagal, status=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}
