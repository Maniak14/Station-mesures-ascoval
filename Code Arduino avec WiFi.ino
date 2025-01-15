#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>

// Définition des constantes
#define DHTPIN 2
#define DHTTYPE DHT22
#define SD_CS 10
#define ESP_RX 2
#define ESP_TX 3

// Configuration WiFi
const char* WIFI_SSID = "VotreSSID";
const char* WIFI_PASSWORD = "VotreMotDePasse";

// Initialisation des objets
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial esp8266(ESP_RX, ESP_TX);
File dataFile;

void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);
  
  // Initialisation du capteur et LCD
  dht.begin();
  lcd.init();
  lcd.backlight();
  
  // Configuration du WiFi
  setupWiFi();
  
  // Initialisation SD
  if (!SD.begin(SD_CS)) {
    lcd.setCursor(0, 1);
    lcd.print("Erreur SD!");
    while (1);
  }
}

void setupWiFi() {
  lcd.clear();
  lcd.print("Config WiFi...");
  
  // Reset ESP8266
  sendCommand("AT+RST");
  delay(2000);
  
  // Mode station
  sendCommand("AT+CWMODE=1");
  
  // Connexion au réseau WiFi
  String cmd = "AT+CWJAP=\"";
  cmd += WIFI_SSID;
  cmd += "\",\"";
  cmd += WIFI_PASSWORD;
  cmd += "\"";
  
  sendCommand(cmd);
  delay(5000);
  
  lcd.clear();
}

void sendCommand(String command) {
  esp8266.println(command);
  delay(1000);
  while(esp8266.available()) {
    Serial.write(esp8266.read());
  }
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  if (isnan(h) || isnan(t)) {
    lcd.clear();
    lcd.print("Erreur capteur!");
    return;
  }
  
  // Affichage LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t, 1);
  lcd.print("C H:");
  lcd.print(h, 1);
  lcd.print("%");
  
  // Enregistrement SD
  saveToSD(t, h);
  
  // Envoi données WiFi
  sendDataWiFi(t, h);
  
  delay(5000);
}

void saveToSD(float temp, float hum) {
  dataFile = SD.open("data.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.print(temp);
    dataFile.print(",");
    dataFile.print(hum);
    dataFile.print(",");
    dataFile.println(millis()/1000);
    dataFile.close();
    lcd.setCursor(0, 1);
    lcd.print("Save OK ");
  }
}

void sendDataWiFi(float temp, float hum) {
  String data = "GET /update?temp=";
  data += String(temp);
  data += "&hum=";
  data += String(hum);
  
  // Envoi vers votre serveur
  sendCommand("AT+CIPSTART=\"TCP\",\"votreserveur.com\",80");
  String cmd = "AT+CIPSEND=";
  cmd += String(data.length() + 4);
  sendCommand(cmd);
  sendCommand(data);
}
