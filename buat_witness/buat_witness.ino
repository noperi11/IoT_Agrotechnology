#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <AsyncTCP.h>
#include <LittleFS.h>


#define DHTPIN 4// Masukkan pin sensor DHT
#define DHTTYPE DHT21
#define RELAY1_PIN 26// Masukkan pin relay 1
#define RELAY2_PIN 25// Masukkan pin relay 2
#define RELAY3_PIN 17// Masukkan pin relay 3

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 20, 4);

const int soilPin = 34; // Masukkan pin sensor soil moisture

// Replace with your network credentials
const char* ssid = "UGMURO-INET";
const char* password = "Gepuk15000";

// Create an AsyncWebServer object on port 80
AsyncWebServer server(80);

void setup() {
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3, 0);
  lcd.print("Selamat Datang!");
  lcd.setCursor(0, 1);
  lcd.print("WS Agroteknologi IoT");
  lcd.setCursor(3, 3);
  lcd.print("-- UG MURO --");
  delay(5000);
  lcd.clear();
  delay(2000);

  // Initialize sensors and pins
  pinMode(soilPin, INPUT);
  dht.begin();
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, HIGH);  // HIGH to turn relay OFF
  digitalWrite(RELAY2_PIN, HIGH);  // HIGH to turn relay OFF
  digitalWrite(RELAY3_PIN, HIGH);  // HIGH to turn relay OFF

  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Handle HTTP GET requests to root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });

  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
  String json = "{\"temperature\":" + String(dht.readTemperature()) +
                 ",\"humidity\":" + String(dht.readHumidity()) +
                 ",\"soilPercentage\":" + String(map(analogRead(soilPin), 2048, 0, 0, 100)) + "}";
  request->send(200, "application/json", json);
});


  server.begin();
}


void loop() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int soilValue = analogRead(soilPin);
  int soilPercentage = map(soilValue, 2048, 0, 0, 100);
  soilPercentage = constrain(soilPercentage, 0, 100);

  lcd.setCursor(5, 0);
  lcd.print("Monitoring");

  lcd.setCursor(0, 1);
  lcd.print("Suhu   : ");
  lcd.setCursor(8, 1);
  lcd.print(temperature);
  lcd.setCursor(17, 1);
  lcd.print("C");

  lcd.setCursor(0, 2);
  lcd.print("K.Udara: ");
  lcd.setCursor(8, 2);
  lcd.print(humidity);
  lcd.setCursor(17, 2);
  lcd.print("%");

  lcd.setCursor(0, 3);
  lcd.print("K.Tanah: ");
  lcd.setCursor(9, 3);
  lcd.print("       ");
  lcd.setCursor(9, 3);
  lcd.print(soilPercentage);
  lcd.setCursor(17, 3);
  lcd.print("%");

  if (temperature > 24 && humidity > 70) {
    digitalWrite(RELAY1_PIN, HIGH); // Relay 1 menyala
  } else {
    digitalWrite(RELAY1_PIN, LOW); // Relay 1 mati
  }

  if (soilPercentage < 30) {
    digitalWrite(RELAY3_PIN, HIGH); // Relay 3 (pompa) menyala
  } else {
    digitalWrite(RELAY3_PIN, LOW); // Relay 3 (pompa) mati
  }

  delay(1000);
}
