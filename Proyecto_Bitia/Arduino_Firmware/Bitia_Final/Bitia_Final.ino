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
// LED 1
const int led1R = 10; const int led1G = 11; const int led1B = 12;
// LED 2
const int led2R = 7;  const int led2G = 8;  const int led2B = 9;
// LED 3
const int led3R = 13; const int led3G = A0; const int led3B = A1;

// ==========================================
// VARIABLES GLOBALES DEL SISTEMA
// ==========================================

int estadoActual = 0; // 0=Iniciando, 1=Nucleo1, 2=Nucleo2, 3=Nucleo3
unsigned long tiempoInicioEstado = 0; // Para controlar los 10s de intro

// --- Para Animación "Iniciando" (Estado 0) ---
int frameIniciando = 0;
unsigned long ultimoTiempoIniciando = 0;

// --- Para Texto Desplazable y Ecualizador (Estados 1, 2, 3) ---
String mensaje = "  @somosbitia en instagram  ";
int offsetTexto = 0;
unsigned long tiempoTexto = 0;
unsigned long tiempoEcualizador = 0;
const int velocidadTexto = 250;
const int velocidadEcualizador = 150;

// --- Para el control de color en NUCLEO 3 ---
int colorIndexNucleo3 = 0; // 0=Rojo, 1=Verde, 2=Azul, 3=Amarillo

// --- Caracteres Personalizados (Ecualizador) ---
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

  // Inicializar LCD
  lcd.init();
  lcd.backlight();
  
  // Cargar caracteres personalizados
  lcd.createChar(0, nivel0); lcd.createChar(1, nivel1); lcd.createChar(2, nivel2);
  lcd.createChar(3, nivel3); lcd.createChar(4, nivel4); lcd.createChar(5, nivel5);
  lcd.createChar(6, nivel6); lcd.createChar(7, nivel7);

  // Configurar Pines LED
  pinMode(led1R, OUTPUT); pinMode(led1G, OUTPUT); pinMode(led1B, OUTPUT);
  pinMode(led2R, OUTPUT); pinMode(led2G, OUTPUT); pinMode(led2B, OUTPUT);
  pinMode(led3R, OUTPUT); pinMode(led3G, OUTPUT); pinMode(led3B, OUTPUT);

  // Configuración inicial
  cambiarEstado(0); // Arrancar en modo Iniciando
}

// ==========================================
// LOOP PRINCIPAL
// ==========================================
void loop() {
  // 1. LEER COMANDOS SERIALES DE LA WEB
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim(); // Quitar espacios extra

    if (comando.startsWith("ESTADO:")) {
      // Extraer el número después de los dos puntos
      int nuevoEstado = comando.substring(7).toInt();
      cambiarEstado(nuevoEstado);
    }
  }

  // 2. LEER ENCODER
  leerEncoder();

  // 3. EJECUTAR LÓGICA DEL ESTADO ACTUAL
  switch (estadoActual) {
    case 0: ejecutarIniciando(); break;
    case 1: ejecutarNucleo(1);   break;
    case 2: ejecutarNucleo(2);   break;
    case 3: ejecutarNucleo(3);   break;
  }
}

// ==========================================
// LÓGICA DE CONTROL DE ESTADOS
// ==========================================

void cambiarEstado(int nuevoID) {
  estadoActual = nuevoID;
  tiempoInicioEstado = millis(); // Reseteamos el reloj del estado
  lcd.clear();
  
  // Resetear variables de animación
  offsetTexto = 0;
  frameIniciando = 0;

  // Configuración inicial de LEDs según el estado
  if (estadoActual == 0 || estadoActual == 1 || estadoActual == 2) {
    setAmarillo();
  } else if (estadoActual == 3) {
    colorIndexNucleo3 = 0; // Resetear a Rojo al entrar
    aplicarColorNucleo3();
  }
}

// ==========================================
// FUNCIONES DE COMPORTAMIENTO
// ==========================================

void leerEncoder() {
  long nuevoValor = encoder.read() / 4; // Dividir por 4 por la resolución del encoder

  if (nuevoValor != valorAnteriorEncoder) {
    // Detectar dirección
    if (nuevoValor > valorAnteriorEncoder) {
      Serial.println("CW");
      
      // Si estamos en estado 3, cambiar color
      if (estadoActual == 3) {
        colorIndexNucleo3++;
        if (colorIndexNucleo3 > 3) colorIndexNucleo3 = 0; // Ciclo 0-3
        aplicarColorNucleo3();
      }
      
    } else {
      Serial.println("CCW");
      
      // Si estamos en estado 3, cambiar color
      if (estadoActual == 3) {
        colorIndexNucleo3--;
        if (colorIndexNucleo3 < 0) colorIndexNucleo3 = 3; // Ciclo 0-3
        aplicarColorNucleo3();
      }
    }
    valorAnteriorEncoder = nuevoValor;
  }
}

// --- ESTADO 0: INICIANDO ---
void ejecutarIniciando() {
  // LEDs siempre amarillos (ya seteados al cambiar estado)
  setAmarillo(); 

  // Animación LCD sin delay
  if (millis() - ultimoTiempoIniciando >= 300) {
    ultimoTiempoIniciando = millis();
    lcd.setCursor(0, 0);
    lcd.print("Iniciando       "); // Limpia sobras
    lcd.setCursor(0, 0);
    lcd.print("Iniciando");
    
    if (frameIniciando == 0) lcd.print(".");
    if (frameIniciando == 1) lcd.print("..");
    if (frameIniciando == 2) lcd.print("...");
    
    frameIniciando = (frameIniciando + 1) % 3;
  }
}

// --- ESTADOS 1, 2 y 3: NUCLEOS ---
void ejecutarNucleo(int numeroNucleo) {
  unsigned long tiempoTranscurrido = millis() - tiempoInicioEstado;

  // FASE 1: Los primeros 10 segundos -> Mostrar Título
  if (tiempoTranscurrido < 10000) {
    lcd.setCursor(5, 0);
    lcd.print("NUCLEO");
    lcd.setCursor(6, 1);
    
    if (numeroNucleo == 1) lcd.print("UNO ");
    if (numeroNucleo == 2) lcd.print("DOS ");
    if (numeroNucleo == 3) lcd.print("TRES");
    
    // En Nucleo 3, el encoder cambia colores AQUÍ y DESPUÉS. 
    // En 1 y 2, los LEDs son fijos amarillos.
  } 
  // FASE 2: Pasaron 10 segundos -> Scroll + Ecualizador
  else {
    // A) Texto Desplazable
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

    // B) Ecualizador
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
// CONTROL DE LUCES
// ==========================================

// Función auxiliar para setear los 3 LEDs a la vez
void setAllLeds(int r, int g, int b) {
  analogWrite(led1R, r); analogWrite(led1G, g); analogWrite(led1B, b);
  analogWrite(led2R, r); analogWrite(led2G, g); analogWrite(led2B, b);
  analogWrite(led3R, r); analogWrite(led3G, g); analogWrite(led3B, b);
}

void setAmarillo() {
  setAllLeds(255, 200, 0); // Amarillo/Dorado
}

void aplicarColorNucleo3() {
  switch (colorIndexNucleo3) {
    case 0: setAllLeds(255, 0, 0);   break; // Rojo
    case 1: setAllLeds(0, 255, 0);   break; // Verde
    case 2: setAllLeds(0, 0, 255);   break; // Azul
    case 3: setAllLeds(255, 180, 0); break; // Amarillo
  }
}