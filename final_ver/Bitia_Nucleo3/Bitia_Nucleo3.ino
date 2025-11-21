#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>

// === LCD ===
LiquidCrystal_I2C lcd(0x27, 16, 2);

// === Encoder ===
Encoder encoder(2, 3);
int boton = 4;

// === LEDs RGB ===
int led1R = 7,  led1G = 8,  led1B = 9;
int led2R = 10, led2G = 11, led2B = 12;
int led3R = 13, led3G = A0, led3B = A1;

// === Estado del color ===
int estadoColor = 0;

// === Texto desplazable ===
String mensaje = "  @somosbitia en instagram  ";
int offset = 0;

unsigned long tiempoTexto = 0;
unsigned long tiempoEcualizador = 0;

const unsigned long intervaloTexto = 180;
const unsigned long intervaloEcualizador = 200;

bool faseInicialTerminada = false;

// === Función para aplicar color en los 3 LEDs ===
void setColor(int r, int g, int b) {
  analogWrite(led1R, r); analogWrite(led1G, g); analogWrite(led1B, b);
  analogWrite(led2R, r); analogWrite(led2G, g); analogWrite(led2B, b);
  analogWrite(led3R, r); analogWrite(led3G, g); analogWrite(led3B, b);
}

// === Selección de color ===
void aplicarEstadoColor() {
  switch (estadoColor) {
    case 0: setColor(255,   0,   0); break;   // ROJO
    case 1: setColor(0,   255,   0); break;   // VERDE
    case 2: setColor(0,     0, 255); break;   // AZUL
    case 3: setColor(255, 180,   0); break;   // AMARILLO cálido
  }
}

long valorAnterior = 0;

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  // === Mostrar "NUCLEO TRES" centrado ===
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("NUCLEO");
  lcd.setCursor(6, 1);
  lcd.print("TRES");

  // LEDs arrancan en ROJO y el encoder YA funciona
  estadoColor = 0;
  aplicarEstadoColor();

  pinMode(boton, INPUT_PULLUP);

  // Guardamos el inicio del tiempo
  tiempoTexto = millis();
}

void loop() {

  // === Encoder funcionando SIEMPRE ===
  long pos = encoder.read() / 4;

  if (pos != valorAnterior) {
    if (pos > valorAnterior) {
      estadoColor = (estadoColor + 1) % 4;
    } else {
      estadoColor = (estadoColor + 3) % 4;
    }

    aplicarEstadoColor();
    valorAnterior = pos;
  }

  // Botón (solo debug)
  if (digitalRead(boton) == LOW) {
    Serial.println("BTN");
    delay(300);
  }

  // === ¿Ya pasaron los 10 segundos iniciales? ===
  if (!faseInicialTerminada) {
    if (millis() - tiempoTexto >= 10000) {

      faseInicialTerminada = true;
      lcd.clear();
    } else {
      return; // durante los 10s no mostramos nada más en LCD
    }
  }

  // === Después de los 10 segundos: texto + ecualizador ===

  unsigned long ahora = millis();

  // --- Texto desplazable ---
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
}
