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

byte smiley[8] = {
B00000,
B01010,
B01010,
B00000,
B11111,
B10001,
B01110,
B00000
};

byte fox[8] = {
B10001,
B11011,
B11111,
B10101,
B11011,
B01010,
B00100,
B00000
};

char a = 0;
char b = 1;

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
  lcd.createChar(a, smiley);
  lcd.createChar(b, fox);
  lcd.setCursor(0,0);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.write(b);
  lcd.print("DC Labor Netzteil");
  lcd.write(b);
  lcd.setCursor(0,1);
  lcd.print("Spannung:  0V - 30V");
  lcd.setCursor(0,2);
  lcd.print("Strom:     0mA - 3A");
  delay(1000);
  lcd.setCursor(0,3);
  lcd.write(b);
  lcd.print(">>Von ReinekeWF<<");
  lcd.write(b);
  delay(2000);
  
  digitalWrite(PIN_RELAIS,LOW);
  lcd.clear();

  
  display_init_va();

  Serial.begin(9600);
  Serial.println("Hi. Setup Fertig.");

}

void display_temp(float temp_c) {
  lcd.setCursor(12, 0); 
  lcd.print(temp_c);
}

void display_init_va() {
  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.print("Spannung:");
  
  lcd.setCursor(19,0);
  lcd.print("V");
  
  lcd.setCursor(0,2);
  lcd.print("Strom:");
  
  lcd.setCursor(19,2);
  lcd.print("A"); 
}

void display_init_temp(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temperatur: ");  
}

void display_va(float spannung_v, float strom_a) {
  int l = 0;
  lcd.setCursor(0,1);
  for (; l <= ((spannung_v / 35) * 20); l++){
     lcd.setCursor(l,1);
     lcd.print("%");
  }
  for(; l <= 20; l++){
    lcd.print(" ");
  }
  
  lcd.setCursor(14,0);
  lcd.print(spannung_v);

  lcd.setCursor(14,2);
  lcd.print(strom_a);
}

float adc_temp, adc_spannung, adc_strom;
float temp_c, spannung_v, strom_a;
unsigned long updated_ms, current_ms;

void loop() {
  // Messungen nehmen
  Taster.update();
  static bool taster_zustand = false;
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
    
    if(Taster.read() != taster_zustand){
      taster_zustand = Taster.read(); // kleine race condition welche wir mal gepflegt ignorieren
      if(!taster_zustand){
        display_init_temp();
      }else{
        display_init_va();
      }
    }
    
    if(!taster_zustand) { // Temperatur anzeigen, während der Taster gedrückt wird
      display_temp(temp_c);
    } else {
      display_va(spannung_v, strom_a);
    }
  }
}
