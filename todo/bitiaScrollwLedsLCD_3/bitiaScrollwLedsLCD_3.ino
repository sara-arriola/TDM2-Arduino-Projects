#include <Wire.h>
#include <LiquidCrystal_I2C.h>   // Librería para LCD I2C
#include <Encoder.h>             // Librería para rotary encoder

// === LCD ===
LiquidCrystal_I2C lcd(0x27, 16, 2);

// === Encoder ===
Encoder encoder(2, 3);   // Pines CLK y DT
int boton = 4;           // Pulsador del encoder
int pinVcc = 5;          // +5V (si lo alimentás desde un pin)
int pinGND = 6;          // GND (si lo usás como referencia)
long valor = 0;

// === LEDs RGB ===
// LED 1
int led1R = 7;
int led1G = 8;
int led1B = 9;
// LED 2
int led2R = 10;
int led2G = 11;
int led2B = 12;
// LED 3
int led3R = 13;
int led3G = A0;
int led3B = A1;

// === Control ===
bool detenido = false;
int colorIndex = 0;

// === Caracteres personalizados del ecualizador ===
byte nivel0[8] = {B00000,B00000,B00000,B00000,B00000,B00000,B00000,B00000};
byte nivel1[8] = {B00000,B00000,B00000,B00000,B00000,B00000,B00000,B11111};
byte nivel2[8] = {B00000,B00000,B00000,B00000,B00000,B00000,B11111,B11111};
byte nivel3[8] = {B00000,B00000,B00000,B00000,B00000,B11111,B11111,B11111};
byte nivel4[8] = {B00000,B00000,B00000,B00000,B11111,B11111,B11111,B11111};
byte nivel5[8] = {B00000,B00000,B00000,B11111,B11111,B11111,B11111,B11111};
byte nivel6[8] = {B00000,B00000,B11111,B11111,B11111,B11111,B11111,B11111};
byte nivel7[8] = {B00000,B11111,B11111,B11111,B11111,B11111,B11111,B11111};

// === Texto desplazable ===
String mensaje = "  @somosbitia en instagram  ";
int offset = 0;

// === Temporizadores ===
unsigned long tiempoTexto = 0;
unsigned long tiempoEcualizador = 0;
const unsigned long intervaloTexto = 600;
const unsigned long intervaloEcualizador = 200;

// === Setup ===
void setup() {
  Serial.begin(9600);
  Serial.println("Encoder con LCD animado");

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, nivel0);
  lcd.createChar(1, nivel1);
  lcd.createChar(2, nivel2);
  lcd.createChar(3, nivel3);
  lcd.createChar(4, nivel4);
  lcd.createChar(5, nivel5);
  lcd.createChar(6, nivel6);
  lcd.createChar(7, nivel7);
  lcd.clear();

  // Pines encoder
  pinMode(boton, INPUT_PULLUP);
  pinMode(pinVcc, OUTPUT);
  pinMode(pinGND, OUTPUT);
  digitalWrite(pinVcc, HIGH);
  digitalWrite(pinGND, LOW);

  // Pines LED
  int leds[] = {led1R, led1G, led1B, led2R, led2G, led2B, led3R, led3G, led3B};
  for (int i = 0; i < 9; i++) pinMode(leds[i], OUTPUT);

  mostrarColor(0);
}

// === Loop ===
void loop() {
  // --- Control por Serial ---
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    if (comando == "stop") {
      detenido = true;
      lcd.clear();
      lcd.print("Pausado");
      Serial.println("Programa detenido.");
    } else if (comando == "run") {
      detenido = false;
      lcd.clear();
      Serial.println("Programa reanudado.");
    }
  }

  if (detenido) return;

  // --- Texto desplazable en LCD ---
  unsigned long ahora = millis();
  if (ahora - tiempoTexto >= intervaloTexto) {
    tiempoTexto = ahora;
    int largo = mensaje.length();
    String ventana = "";
    for (int i = 0; i < 16; i++) {
      int pos = (offset + i) % largo;
      ventana += mensaje[pos];
    }
    lcd.setCursor(0, 0);
    lcd.print(ventana);
    offset = (offset + 1) % largo;
  }

  // --- Ecualizador dinámico ---
  if (ahora - tiempoEcualizador >= intervaloEcualizador) {
    tiempoEcualizador = ahora;
    for (int i = 0; i < 16; i++) {
      int nivel = random(0, 8);
      lcd.setCursor(i, 1);
      lcd.write(byte(nivel));
    }
  }

  // --- Leer encoder ---
  long nuevoValor = encoder.read() / 4;
  if (nuevoValor != valor) {
    if (nuevoValor > valor) {
      colorIndex = (colorIndex + 1) % 4;
      Serial.println("CW");
    } else {
      colorIndex = (colorIndex + 3) % 4;
      Serial.println("CCW");
    }
    mostrarColor(colorIndex);
    valor = nuevoValor;
  }

  // --- Pulsador del encoder ---
  if (digitalRead(boton) == LOW) {
    Serial.println("BTN");
    delay(300);
  }
}

// === Función para mostrar color según índice ===
void mostrarColor(int index) {
  int r = 0, g = 0, b = 0;
  switch (index) {
    case 0: r = 255; g = 0;   b = 0;   break; // Rojo
    case 1: r = 0;   g = 255; b = 0;   break; // Verde
    case 2: r = 0;   g = 0;   b = 255; break; // Azul
    case 3: r = 255; g = 255; b = 0;   break; // Amarillo
  }
  analogWrite(led1R, r);
  analogWrite(led1G, g);
  analogWrite(led1B, b);
  analogWrite(led2R, r);
  analogWrite(led2G, g);
  analogWrite(led2B, b);
  analogWrite(led3R, r);
  analogWrite(led3G, g);
  analogWrite(led3B, b);
}
