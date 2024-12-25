//Library Yang Digunakan
#include <Adafruit_Sensor.h>
#include <Arduino.h>
#include <DHT.h>
#include <DHT_U.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <AsyncTCP.h>
#include <LittleFS.h>

// PIN yang digunakan
#define DHTPIN 4// Masukkan pin sensor DHT
#define DHTTYPE DHT21
#define RELAY1_PIN 26// Masukkan pin relay 1
#define RELAY2_PIN 25// Masukkan pin relay 2
#define RELAY3_PIN 17// Masukkan pin relay 3
const int soilPin = 34; // Masukkan pin sensor soil moisture

//Inisialisasi DHT Sensor
DHT dht(DHTPIN, DHTTYPE);

//Inisialisasi i2c LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

// SSID dan PW Wifi
const char* ssid = "your-ssid"; //ganti dengan ssid wifi kamu
const char* password = "your-password"; //ganti dengan password wifi kamu

// Jalankan Webserver Asinkronus
AsyncWebServer server(80);

void setup() {
  
  // Inisialisasi Serial Monitor
  Serial.begin(115200);

  // Mulai LCD
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

  // Inisialisasi mode pin dari sensor dan relay
  pinMode(soilPin, INPUT);
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(RELAY3_PIN, OUTPUT);

  //Mulai menyalakan sensor DHT
  dht.begin();

  //Mematikan relay
  digitalWrite(RELAY1_PIN, LOW);
  digitalWrite(RELAY2_PIN, LOW); 
  digitalWrite(RELAY3_PIN, LOW);  

  // Inisialisasi Little FS
  if (!LittleFS.begin()) {
    Serial.println("Error Mounting Little FS");
    return;
  }

  // Memulai koneksi ke WIFI
  WiFi.begin(ssid, password);

  //Looping Untuk mencoba koneksi ke WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //Output ke Serial Monitor bahwa sudah terkoneksi ke WiFi
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Menghandle HTTP Request ke Index.html
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html");
  });

  // Untuk handle CSS Index.html
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });

  // Memparsing data dari sensor ke Server
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request){
  String json = "{\"temperature\":" + String(dht.readTemperature()) +
                 ",\"humidity\":" + String(dht.readHumidity()) +
                 ",\"soilPercentage\":" + String(map(analogRead(soilPin), 2048, 0, 0, 100)) + "}";
  request->send(200, "application/json", json);
});
  //Memulai Server
  server.begin();
}


void loop() {
  // Inisialisasi variabel untuk menyimpan data dari sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int soilValue = analogRead(soilPin);
  int soilPercentage = map(soilValue, 2048, 0, 0, 100);
  soilPercentage = constrain(soilPercentage, 0, 100);

  // Menampilkan data ke LCD I2C
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

  // Proses 1
  if (temperature > 24 && humidity > 70) {
    digitalWrite(RELAY1_PIN, HIGH); // Relay 1 menyala
  } else {
    digitalWrite(RELAY1_PIN, LOW); // Relay 1 mati
  }

  // Proses 2
  if (soilPercentage < 30) {
    digitalWrite(RELAY3_PIN, HIGH); // Relay 3 (pompa) menyala
  } else {
    digitalWrite(RELAY3_PIN, LOW); // Relay 3 (pompa) mati
  }

  delay(1000);
}