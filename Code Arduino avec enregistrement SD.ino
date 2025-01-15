#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>

// Définition des constantes
#define DHTPIN 2
#define DHTTYPE DHT22
#define SD_CS 10  // Pin CS pour la carte SD

// Initialisation des objets
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);
File dataFile;

void setup() {
  Serial.begin(9600);
  
  // Initialisation du capteur DHT
  dht.begin();
  
  // Initialisation de l'écran LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Initialisation...");
  
  // Initialisation de la carte SD
  Serial.print("Initialisation SD...");
  if (!SD.begin(SD_CS)) {
    Serial.println("Echec!");
    lcd.setCursor(0, 1);
    lcd.print("Erreur SD!");
    while (1);
  }
  Serial.println("OK!");
  
  // Création de l'en-tête du fichier
  dataFile = SD.open("data.csv", FILE_WRITE);
  if (dataFile) {
    if (dataFile.size() == 0) {
      dataFile.println("Temperature,Humidite,Date");
    }
    dataFile.close();
  }
  
  lcd.clear();
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
  lcd.print("C ");
  lcd.print("H:");
  lcd.print(h, 1);
  lcd.print("%");
  
  // Enregistrement sur SD
  dataFile = SD.open("data.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.print(t);
    dataFile.print(",");
    dataFile.print(h);
    dataFile.print(",");
    dataFile.println(millis()/1000); // Temps en secondes
    dataFile.close();
    
    lcd.setCursor(0, 1);
    lcd.print("Save OK");
  } else {
    lcd.setCursor(0, 1);
    lcd.print("Erreur Save!");
  }
  
  delay(5000); // Enregistrement toutes les 5 secondes
}