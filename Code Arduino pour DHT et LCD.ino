// Inclusion des bibliothèques nécessaires
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Définition des constantes
#define DHTPIN 2          // Pin de données du DHT
#define DHTTYPE DHT22     // Type de capteur (DHT22 ou DHT11)

// Initialisation des objets
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Adresse I2C 0x27, écran 16x2

void setup() {
  // Initialisation du port série pour le debugging
  Serial.begin(9600);
  
  // Initialisation du capteur DHT
  dht.begin();
  
  // Initialisation de l'écran LCD
  lcd.init();
  lcd.backlight();
  
  // Message de démarrage
  lcd.setCursor(0, 0);
  lcd.print("Initialisation...");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Lecture des données du capteur
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  // Vérification si la lecture a réussi
  if (isnan(h) || isnan(t)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erreur capteur!");
    Serial.println("Échec de lecture du capteur DHT!");
    delay(2000);
    return;
  }
  
  // Affichage sur le port série (pour debug)
  Serial.print("Humidité: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Température: ");
  Serial.print(t);
  Serial.println(" °C");
  
  // Affichage sur l'écran LCD
  lcd.clear();
  
  // Ligne 1 : Température
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(t, 1);
  lcd.print((char)223);  // Symbole degré
  lcd.print("C");
  
  // Ligne 2 : Humidité
  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(h, 1);
  lcd.print("%");
  
  // Attente avant la prochaine lecture
  delay(2000);
}