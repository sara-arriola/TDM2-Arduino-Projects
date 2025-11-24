#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Encoder.h>

// ==========================================
// CONFIGURACIÓN DE PINES Y HARDWARE
// ==========================================

// --- PANTALLA LCD ---
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --- ENCODER ---
Encoder encoder(2, 3); // CLK=2, DT=3
long valorAnteriorEncoder = -999;

// --- LEDS RGB ---
// NOTA: Arduino UNO tiene pocos pines PWM. 
// En los pines 7, 8, 12, 13, A0, A1 NO se pueden mezclar colores suaves.
// Solo admiten ENCENDIDO (255) o APAGADO (0).

// LED 1
const int led1R = 10; const int led1G = 11; const int led1B = 12;
// LED 2
const int led2R = 7;  const int led2G = 8;  const int led2B = 9;
// LED 3
const int led3R = 13; const int led3G = A0; const int led3B = A1;

// ==========================================
// VARIABLES GLOBALES
// ==========================================

int estadoActual = 0; // 0=Iniciando, 1=Nucleo1, 2=Nucleo2, 3=Nucleo3
unsigned long tiempoInicioEstado = 0; 

// --- Variables Animación ---
int frameIniciando = 0;
unsigned long ultimoTiempoIniciando = 0;
String mensaje = "  @somosbitia en instagram  ";
int offsetTexto = 0;
unsigned long tiempoTexto = 0;
unsigned long tiempoEcualizador = 0;

const int velocidadTexto = 600; 
const int velocidadEcualizador = 150; 

int colorIndexNucleo3 = 0; 

// --- Caracteres Personalizados ---
byte nivel0[8] = {B00000,B00000,B00000,B00000,B00000,B00000,B00000,B00000};
byte nivel1[8] = {B00000,B00000,B00000,B00000,B00000,B00000,B00000,B11111};
byte nivel2[8] = {B00000,B00000,B00000,B00000,B00000,B00000,B11111,B11111};
byte nivel3[8] = {B00000,B00000,B00000,B00000,B00000,B11111,B11111,B11111};
byte nivel4[8] = {B00000,B00000,B00000,B00000,B11111,B11111,B11111,B11111};
byte nivel5[8] = {B00000,B00000,B00000,B11111,B11111,B11111,B11111,B11111};
byte nivel6[8] = {B00000,B00000,B11111,B11111,B11111,B11111,B11111,B11111};
byte nivel7[8] = {B00000,B11111,B11111,B11111,B11111,B11111,B11111,B11111};

// ==========================================
// SETUP
// ==========================================
void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  
  lcd.createChar(0, nivel0); lcd.createChar(1, nivel1); lcd.createChar(2, nivel2);
  lcd.createChar(3, nivel3); lcd.createChar(4, nivel4); lcd.createChar(5, nivel5);
  lcd.createChar(6, nivel6); lcd.createChar(7, nivel7);

  // Pines LED como salida
  pinMode(led1R, OUTPUT); pinMode(led1G, OUTPUT); pinMode(led1B, OUTPUT);
  pinMode(led2R, OUTPUT); pinMode(led2G, OUTPUT); pinMode(led2B, OUTPUT);
  pinMode(led3R, OUTPUT); pinMode(led3G, OUTPUT); pinMode(led3B, OUTPUT);

  cambiarEstado(0); 
}

// ==========================================
// LOOP
// ==========================================
void loop() {
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    if (comando.startsWith("ESTADO:")) {
      int nuevoEstado = comando.substring(7).toInt();
      cambiarEstado(nuevoEstado);
    }
  }

  leerEncoder();

  switch (estadoActual) {
    case 0: ejecutarIniciando(); break;
    case 1: ejecutarNucleo(1);   break;
    case 2: ejecutarNucleo(2);   break;
    case 3: ejecutarNucleo(3);   break;
  }
}

// ==========================================
// LOGICA
// ==========================================
void cambiarEstado(int nuevoID) {
  estadoActual = nuevoID;
  tiempoInicioEstado = millis(); 
  lcd.clear();
  offsetTexto = 0;
  frameIniciando = 0;

  if (estadoActual == 0 || estadoActual == 1 || estadoActual == 2) {
    setAmarillo();
  } else if (estadoActual == 3) {
    colorIndexNucleo3 = 0; 
    aplicarColorNucleo3();
  }
}

void leerEncoder() {
  long nuevoValor = encoder.read() / 4; 
  if (nuevoValor != valorAnteriorEncoder) {
    if (nuevoValor > valorAnteriorEncoder) {
      Serial.println("CW");
      if (estadoActual == 3) {
        colorIndexNucleo3++;
        if (colorIndexNucleo3 > 3) colorIndexNucleo3 = 0; 
        aplicarColorNucleo3();
      }
    } else {
      Serial.println("CCW");
      if (estadoActual == 3) {
        colorIndexNucleo3--;
        if (colorIndexNucleo3 < 0) colorIndexNucleo3 = 3; 
        aplicarColorNucleo3();
      }
    }
    valorAnteriorEncoder = nuevoValor;
  }
}

void ejecutarIniciando() {
  setAmarillo(); 
  if (millis() - ultimoTiempoIniciando >= 300) {
    ultimoTiempoIniciando = millis();
    lcd.setCursor(0, 0);
    lcd.print("Iniciando       "); 
    lcd.setCursor(0, 0);
    lcd.print("Iniciando");
    if (frameIniciando == 0) lcd.print(".");
    if (frameIniciando == 1) lcd.print("..");
    if (frameIniciando == 2) lcd.print("...");
    frameIniciando = (frameIniciando + 1) % 3;
  }
}

void ejecutarNucleo(int numeroNucleo) {
  unsigned long tiempoTranscurrido = millis() - tiempoInicioEstado;

  if (tiempoTranscurrido < 10000) {
    lcd.setCursor(5, 0);
    lcd.print("NUCLEO");
    lcd.setCursor(6, 1);
    if (numeroNucleo == 1) lcd.print("UNO ");
    if (numeroNucleo == 2) lcd.print("DOS ");
    if (numeroNucleo == 3) lcd.print("TRES");
  } 
  else {
    if (millis() - tiempoTexto >= velocidadTexto) {
      tiempoTexto = millis();
      String ventana = "";
      int largo = mensaje.length();
      for (int i = 0; i < 16; i++) {
        int pos = (offsetTexto + i) % largo;
        ventana += mensaje[pos];
      }
      lcd.setCursor(0, 0);
      lcd.print(ventana);
      offsetTexto = (offsetTexto + 1) % largo;
    }
    if (millis() - tiempoEcualizador >= velocidadEcualizador) {
      tiempoEcualizador = millis();
      for (int i = 0; i < 16; i++) {
        lcd.setCursor(i, 1);
        lcd.write(byte(random(0, 8)));
      }
    }
  }
}

// ==========================================
// CONTROL DE LUCES (MODO DIGITAL)
// ==========================================

// Función auxiliar para forzar 0 o 255 (simulando digitalWrite)
// Esto asegura compatibilidad con pines NO-PWM
void safeWrite(int pin, int val) {
  if (val > 127) digitalWrite(pin, HIGH);
  else digitalWrite(pin, LOW);
  
  // Nota: Si el pin SI es PWM, esto funciona igual como ON/OFF puro.
}

void setAllLeds(int r, int g, int b) {
  // Usamos digitalWrite indirectamente para asegurar que los pines no-PWM prendan
  safeWrite(led1R, r); safeWrite(led1G, g); safeWrite(led1B, b);
  safeWrite(led2R, r); safeWrite(led2G, g); safeWrite(led2B, b);
  safeWrite(led3R, r); safeWrite(led3G, g); safeWrite(led3B, b);
}

void setAmarillo() {
  setAllLeds(255, 180, 0); 
}

void aplicarColorNucleo3() {
  switch (colorIndexNucleo3) {
    case 0: setAllLeds(255, 0, 0);   break; // Rojo
    case 1: setAllLeds(0, 255, 0);   break; // Verde
    case 2: setAllLeds(0, 0, 255);   break; // Azul
    case 3: setAllLeds(255, 180, 0); break; // Amarillo (R+G puros)
  }
}
