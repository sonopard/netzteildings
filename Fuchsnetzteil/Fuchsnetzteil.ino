#define BOUNCE_LOCK_OUT
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Bounce2.h>

#define DISPLAY_UPDATE_MS 100
#define PIN_TASTER 2
#define PIN_RELAIS 7
#define PIN_VOLT A0
#define PIN_AMP A3
#define PIN_TEMP A4
#define THERMISTOR_SPANNUNGSTEILER_RES 10000
#define THERMISTOR_NOMINALWERT 10000
#define THERMISTOR_NOMINALTEMP 25
#define THERMISTOR_B_KOEFFIZIENT 3950

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

Bounce Taster;

void setup() {
  pinMode(PIN_TASTER, INPUT_PULLUP);
  pinMode(PIN_RELAIS, OUTPUT);
  pinMode(PIN_VOLT, INPUT);
  pinMode(PIN_AMP, INPUT);
  pinMode(PIN_TEMP, INPUT);

  digitalWrite(PIN_RELAIS,HIGH);

  Taster.attach(PIN_TASTER);
  Taster.interval(30);

  lcd.begin(20,4);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" DC Labor Netzteil");
  lcd.setCursor(0,1);
  lcd.print("Spannung:  0V - 30V");
  lcd.setCursor(0,2);
  lcd.print("Strom:     0mA - 3A");
  delay(1000);
  lcd.setCursor(0,3);
  lcd.print(" >>Von ReinekeWF<<");
  delay(2000);
  digitalWrite(PIN_RELAIS,LOW);
  lcd.clear();
}

void display_temp(float temp_c) {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(temp_c);
}

void display_va(float spannung_v, float strom_a) {
  lcd.clear();

  for (int l = 0; l <= ((spannung_v / 35) * 20); l++){
     lcd.setCursor(l,1);
     lcd.print("%");
  }
  lcd.setCursor(0,0);
  lcd.print("Spannung:");
  lcd.setCursor(14,0);
  lcd.print(spannung_v);
  lcd.setCursor(19,0);
  lcd.print("V");

  lcd.setCursor(0,2);
  lcd.print("Strom:");
  lcd.setCursor(14,2);
  lcd.print(strom_a);
  lcd.setCursor(19,2);
  lcd.print("A");
}

float adc_temp, adc_spannung, adc_strom;
float temp_c, spannung_v, strom_a;
unsigned long updated_ms, current_ms;

void loop() {
  // Messungen nehmen
  Taster.update();
  adc_temp = analogRead(PIN_TEMP);
  adc_spannung = analogRead(PIN_VOLT);
  adc_strom = analogRead(PIN_AMP);

  // Werte berechnen
  temp_c = 1023 / adc_temp -1;
  temp_c = THERMISTOR_SPANNUNGSTEILER_RES / temp_c;
  temp_c = temp_c / THERMISTOR_NOMINALWERT; // (R/Ro)
  temp_c = log(temp_c); // ln(R/Ro)
  temp_c /= THERMISTOR_B_KOEFFIZIENT; // 1/B * ln(R/Ro)
  temp_c += 1.0 / (THERMISTOR_NOMINALTEMP + 273.15); // + (1/To)
  temp_c = 1.0 / temp_c; // Kehrwert
  temp_c -= 273.15; // Celsiusieren

  spannung_v = (adc_spannung / 1024) * 33 * 0.8035;

  strom_a = (adc_strom - 512) * 0.3 * 0.25;
  if(strom_a < 0)
    strom_a = 0;

  // Display aktualisieren
  current_ms = millis();
  if(updated_ms == current_ms)
     return;
  updated_ms = current_ms;

  if(!(updated_ms % DISPLAY_UPDATE_MS)) { // Display nur alle 100ms updaten
    if(!Taster.read()) { // Temperatur anzeigen, während der Taster gedrückt wird
      display_temp(temp_c);
    } else {
      display_va(spannung_v, strom_a);
    }
  }
}
