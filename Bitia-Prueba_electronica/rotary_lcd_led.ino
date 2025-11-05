#include <LiquidCrystal_I2C.h>   // Librería para LCD con I2C
#include <Encoder.h>             // Librería para el rotary encoder

// === LCD ===
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Dirección 0x27 común, 16x2

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

// === Control por Serial ===
bool detenido = false;

void setup() {
  Serial.begin(9600);
  Serial.println("Escribí 'stop' para detener o 'run' para continuar.");

  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Sistema listo!");

  // Configurar pines encoder
  pinMode(boton, INPUT_PULLUP);
  pinMode(pinVcc, OUTPUT);
  pinMode(pinGND, OUTPUT);
  digitalWrite(pinVcc, HIGH);
  digitalWrite(pinGND, LOW);

  // Configurar pines LED
  int leds[] = {led1R, led1G, led1B, led2R, led2G, led2B, led3R, led3G, led3B};
  for (int i = 0; i < 9; i++) pinMode(leds[i], OUTPUT);
}

void loop() {
  // === Leer comandos del Serial Monitor ===
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
      lcd.print("Reanudando...");
      Serial.println("Programa reanudado.");
    }
  }

  if (detenido) return;  // No hace nada si está pausado

  // === Leer encoder ===
  long nuevoValor = encoder.read() / 4;  // Dividido para suavizar
  if (nuevoValor != valor) {
    valor = nuevoValor;
    lcd.clear();
    lcd.print("Encoder: ");
    lcd.setCursor(0, 1);
    lcd.print(valor);
    Serial.print("Encoder: ");
    Serial.println(valor);

    // Cambiar color LED según posición
    mostrarColor(valor);
  }

  delay(100);
}

// === Función para mostrar colores según valor del encoder ===
void mostrarColor(long valor) {
  // Colores cíclicos
  int color = valor % 4;
  apagarTodo();
  switch (color) {
    case 0: encenderColor(255, 0, 0); break;   // Rojo
    case 1: encenderColor(0, 255, 0); break;   // Verde
    case 2: encenderColor(0, 0, 255); break;   // Azul
    case 3: encenderColor(255, 255, 255); break; // Blanco
  }
}

void encenderColor(int r, int g, int b) {
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

void apagarTodo() {
  encenderColor(0, 0, 0);
}
