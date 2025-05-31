#define BLYNK_TEMPLATE_ID "TMPL6RKAV4e8q"
#define BLYNK_TEMPLATE_NAME "Air Quality Monitoring"
#define BLYNK_AUTH_TOKEN "_xwramKgAZjvIH2R-hkWfSnDx9HFLY6O"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WiFi credentials
char ssid[] = "Airmon";
char pass[] = "12345678";

// Inisialisasi Blynk dan Timer
BlynkTimer timer;

// Inisialisasi LCD dan DHT
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Pin sensor dan aktuator
int sensorPin = A0;     // Sensor gas (analog)
const int LDR = D5;     // LDR (digital)
const int laser = D8;   // Laser
const int relay = D0;   // Relay
#define buzer D7        // Buzzer

// Fungsi untuk kirim data ke Blynk
void sendToBlynk() {
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  int gas = analogRead(sensorPin);
  int ldr = digitalRead(LDR);

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, gas);
  Blynk.virtualWrite(V3, ldr);
}

void setup() {
  Serial.begin(9600);

  // Koneksi ke WiFi dan Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Inisialisasi sensor dan aktuator
  pinMode(sensorPin, INPUT);
  pinMode(buzer, OUTPUT);
  pinMode(LDR, INPUT);
  pinMode(laser, OUTPUT);
  pinMode(relay, OUTPUT);

  digitalWrite(buzer, HIGH);
  digitalWrite(laser, HIGH);
  digitalWrite(relay, HIGH);

  dht.begin();

  // Kirim data ke Blynk setiap 2 detik
  timer.setInterval(2000L, sendToBlynk);
}

void loop() {
  Blynk.run();
  timer.run();

  lcd.init();
  lcd.backlight();

  int h = dht.readHumidity();
  int t = dht.readTemperature();
  int gas = analogRead(sensorPin);
  int inputLDR = digitalRead(LDR);

  // LCD: suhu dan kelembaban
  lcd.setCursor(0, 0);
  lcd.print("Temp = ");
  lcd.print(t);
  lcd.print((char)223); // simbol derajat
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Humidity = ");
  lcd.print(h);
  lcd.print("%");

  delay(1000);

  // LCD: nilai gas
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Gas Value: ");
  lcd.print(gas);
  lcd.setCursor(0, 1);

  if (gas < 180) {
    lcd.print("Fresh Air");
    digitalWrite(relay, HIGH);
  } else {
    lcd.print("Bad Air");
    digitalWrite(relay, LOW);
  }

  // Monitor serial
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%  Temperature: ");
  Serial.print(t);
  Serial.print("°C  Gas: ");
  Serial.print(gas);
  Serial.print(" PPM  LDR: ");
  Serial.println(inputLDR);

  // Kontrol buzzer berdasarkan LDR
  if (inputLDR > 0) {
    digitalWrite(buzer, HIGH);
  } else {
    digitalWrite(buzer, LOW);
  }

  // Laser mati jika gas tinggi
  if (gas > 180) {
    digitalWrite(laser, LOW);
  } else {
    digitalWrite(laser, HIGH);
  }

  delay(500);
}
