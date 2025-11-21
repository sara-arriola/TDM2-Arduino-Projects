#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>

// === LCD I2C ===
LiquidCrystal_I2C lcd(0x27, 16, 2);

// === Encoder ===
Encoder encoder(2, 3);
int boton = 4;

// === LEDs RGB ===
int led1R = 7,  led1G = 8,  led1B = 9;
int led2R = 10, led2G = 11, led2B = 12;
int led3R = 13, led3G = A0, led3B = A1;

// === Animación "Iniciando..." ===
String base = "Iniciando";
int frame = 0;
unsigned long ultimoTiempo = 0;
const unsigned long intervalo = 300; // velocidad del cambio

// === Función para fijar color amarillo ===
void setAmarillo() {
  int r = 255, g = 180, b = 0;

  analogWrite(led1R, r); analogWrite(led1G, g); analogWrite(led1B, b);
  analogWrite(led2R, r); analogWrite(led2G, g); analogWrite(led2B, b);
  analogWrite(led3R, r); analogWrite(led3G, g); analogWrite(led3B, b);
}

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  // LEDs configurados permanentemente en amarillo
  setAmarillo();

  pinMode(boton, INPUT_PULLUP);

  ultimoTiempo = millis();
}

void loop() {

  // === LEDs se mantienen siempre amarillos ===
  setAmarillo();

  // === Animación LCD: "Iniciando..." ===
  unsigned long ahora = millis();

  if (ahora - ultimoTiempo >= intervalo) {
    ultimoTiempo = ahora;
    frame = (frame + 1) % 3;

    lcd.setCursor(0, 0);
    lcd.print("                "); // limpiar línea

    lcd.setCursor(0, 0);
    lcd.print(base);

    if (frame == 0) lcd.print(".");
    if (frame == 1) lcd.print("..");
    if (frame == 2) lcd.print("...");
  }

  // === Encoder se lee, pero NO hace nada ===
  long pos = encoder.read();
  if (digitalRead(boton) == LOW) {
    Serial.println("BTN");
    delay(200);
  }

}
