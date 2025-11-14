**Librerías usadas**

#include <Wire.h>                // leer o enviar data a i2c
#include <LiquidCrystal_I2C.h>   // Librería para LCD I2C
#include <Encoder.h>             // Librería para rotary encoder

---

RESUMEN GENERAL (hecho por chat.gpt)
Este programa:

- Muestra un texto en scroll permanente
- Muestra un ecualizador animado en la segunda línea
- Cambia colores de LEDs RGB con el rotary encoder
- Detecta CW y CCW
- Detecta el botón del encoder
- Tiene comandos por serial para detener y reanudar
