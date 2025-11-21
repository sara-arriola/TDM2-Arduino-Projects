#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>

// === LCD ===
LiquidCrystal_I2C lcd(0x27, 16, 2);

// === Encoder ===
Encoder encoder(2, 3);
int boton = 4;
int pinVcc = 5;
int pinGND = 6;
long valor = 0;

// === LEDs RGB ===
int led1R = 10, led1G = 11, led1B = 12;
int led2R = 7,  led2G = 8,  led2B = 9;
int led3R = 13, led3G = A0, led3B = A1;

// === Control ===
bool detenido = false;

// === Caracteres personalizados ===
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

// === SETUP ===
void setup() {
  Serial.begin(9600);

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

  // Encoder
  pinMode(boton, INPUT_PULLUP);
  pinMode(pinVcc, OUTPUT);
  pinMode(pinGND, OUTPUT);
  digitalWrite(pinVcc, HIGH);
  digitalWrite(pinGND, LOW);

  // Pines LED
  int leds[] = {led1R, led1G, led1B, led2R, led2G, led2B, led3R, led3G, led3B};
  for (int i = 0; i < 9; i++) pinMode(leds[i], OUTPUT);

  // 🔶 LEDS AMARILLAS DESDE EL INICIO
  mostrarColorAmarillo();

  // ===== MOSTRAR “NUCLEO DOS” POR 10 SEGUNDOS =====
  lcd.clear();
  lcd.setCursor(4, 0);  // centrado en fila 0
  lcd.print("NUCLEO");
  lcd.setCursor(6, 1);  // centrado en fila 1
  lcd.print("DOS");

  delay(10000);  // 10 segundos visibles

  lcd.clear();
}

// === LOOP ===
void loop() {
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    if (comando == "stop") {
      detenido = true;
      lcd.clear();
      lcd.print("Pausado");
    } else if (comando == "run") {
      detenido = false;
      lcd.clear();
    }
  }

  if (detenido) return;

  unsigned long ahora = millis();

  // Texto desplazable
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

  // Ecualizador
  if (ahora - tiempoEcualizador >= intervaloEcualizador) {
    tiempoEcualizador = ahora;
    for (int i = 0; i < 16; i++) {
      int nivel = random(0, 8);
      lcd.setCursor(i, 1);
      lcd.write(byte(nivel));
    }
  }

  // Encoder
  long nuevoValor = encoder.read() / 4;

  if (nuevoValor != valor) {
    if (nuevoValor > valor) Serial.println("CW");
    else Serial.println("CCW");
    valor = nuevoValor;
  }

  if (digitalRead(boton) == LOW) {
    Serial.println("BTN");
    delay(300);
  }
}

// === AMARILLO ===
void mostrarColorAmarillo() {
  int r = 255, g = 200, b = 0; // tono más distinguible del verde

  analogWrite(led1R, r); analogWrite(led1G, g); analogWrite(led1B, b);
  analogWrite(led2R, r); analogWrite(led2G, g); analogWrite(led2B, b);
  analogWrite(led3R, r); analogWrite(led3G, g); analogWrite(led3B, b);
}
