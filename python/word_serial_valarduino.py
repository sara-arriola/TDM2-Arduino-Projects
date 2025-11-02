# word_serial_valarduino.py (versión corregida y estable)
import serial
import time
import sys
import re

# --- CONFIGURACIÓN SERIAL ---
PUERTO_SERIAL = 'COM9' 
VELOCIDAD_BAUDIOS = 9600
TIEMPO_ESPERA = 4  # Espera para sincronizar con el reinicio del Arduino

print(f"--- Lector de Potenciómetro ---")
print(f"Intentando conexión al puerto: {PUERTO_SERIAL} a {VELOCIDAD_BAUDIOS} baudios.")

try:
    # Intentar abrir la conexión serial
    ser = serial.Serial(
        port=PUERTO_SERIAL,
        baudrate=VELOCIDAD_BAUDIOS,
        timeout=1
    )

    ser.flushInput()
    print(f"Conexión exitosa. Puerto abierto.")
    print(f"Esperando {TIEMPO_ESPERA} segundos para el inicio del Arduino...")
    time.sleep(TIEMPO_ESPERA)

    print("\n--- INICIO DE LECTURA DE DATOS ---")
    print("Formato: valor,DOWN | valor,UP | valor (solo el número si está entre 451-549)\n")

    ultimo_valor = None  # Guardar último valor leído

    while True:
        if ser.in_waiting > 0:
            # Leer línea completa
            linea_bytes = ser.readline()
            linea_string = linea_bytes.decode('utf-8', errors='ignore').strip()

            if not linea_string:
                continue

            # Extraer número de la línea (funciona tanto si hay texto como si solo hay número)
            coincidencia = re.search(r'(\d+)', linea_string)
            if coincidencia:
                valor = int(coincidencia.group(1))

                # Solo procesar si cambió respecto al anterior
                #if valor != ultimo_valor:
                #    ultimo_valor = valor

                    # Clasificar e imprimir según rango
                if 0 <= valor <= 450:
                        print(f"{valor},DOWN")
                elif valor >= 550:
                        print(f"{valor},UP")
                else:
                        print(f"{valor}")
                time.sleep(1)    

except serial.SerialException as e:
    print(f"\nERROR: No se pudo abrir o leer el puerto serial {PUERTO_SERIAL}.")
    print("-----------------------------------------------------")
    print("CAUSA PROBABLE: El puerto está BLOQUEADO.")
    print("ACCIÓN: Asegúrate de que PlatformIO o el IDE de Arduino estén CERRADOS.")
    print(f"Detalle del error: {e}")

except KeyboardInterrupt:
    print("\nLectura serial detenida por el usuario (Ctrl+C).")

except Exception as e:
    print(f"\nERROR INESPERADO: {e}")

finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("Puerto serial cerrado.")
