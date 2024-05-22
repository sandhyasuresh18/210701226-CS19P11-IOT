#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"


#define DOUT 23
#define CLK 19

#define BLYNK_TEMPLATE_ID "TMPL30S2-08iK"               
#define BLYNK_TEMPLATE_NAME "IOT based IV bag monitor" 

LiquidCrystal_I2C lcd(0x27, 20, 2);
#define BUZZER 2
#define BLYNK_PRINT Serial

char auth[] = "fptdlnyuQwmuQ-ghbY3LH7H-or-kxo8m"; 
char ssid[] = "sandy 808";
char pass[] = "SHANDIYA";

int liter;
int val;
float weight;
float calibration_factor = 100; 


HX711 scale;

void setup() {
  Serial.begin(115200);
  lcd.begin();  
  lcd.backlight();
  pinMode(BUZZER, OUTPUT);
  Serial.println("Remove all weight from scale");

  
  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.tare();                             
  long zero_factor = scale.read_average();  
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);

  Blynk.begin(auth, ssid, pass);
}

void loop() {
  Blynk.run();
  measureweight();
}

void measureweight() {
  scale.set_scale(calibration_factor);  
  weight = scale.get_units(5);
  if (weight < 0) {
    weight = 0.00;
  }
  liter = weight * 1000;
  val = liter;
  val = map(val, 0, 505, 0, 100);

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("IOT based IV Bag");
  lcd.setCursor(2, 1);
  lcd.print("Monitoring System");
  Serial.print("Kilogram: ");
  Serial.print(weight);
  Serial.println(" Kg");
  lcd.setCursor(1, 2);
  lcd.print("IV Bottle = ");
  lcd.print(liter);
  lcd.print(" mL");
  Serial.print("IV BOTTLE: ");
  Serial.print(liter);
  Serial.println(" mL");
  lcd.setCursor(1, 3);
  lcd.print("IV Bag Percent=");
  lcd.print(val);
  lcd.print("%");
  Serial.print("IV Bag Percent: ");
  Serial.print(val);
  Serial.println("%");
  Serial.println();
  delay(500);
  if (val <= 90 && val >= 75) {
    Blynk.logEvent("iv_alert", "IV Bottle is 50%");
    digitalWrite(BUZZER, HIGH);
    delay(50);
    digitalWrite(BUZZER, LOW);
    delay(50);
  } else if (val <= 20) {
    Blynk.logEvent("iv_alert", "IV Bottle is too LOW");
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }
  Blynk.virtualWrite(V0, liter);
  Blynk.virtualWrite(V1, val);
}
