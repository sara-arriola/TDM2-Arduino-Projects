# lector_serial_crudo.py
import serial
import time
import sys

# --- CONFIGURACIÓN SERIAL ---
PUERTO_SERIAL = 'COM9' 
VELOCIDAD_BAUDIOS = 9600
TIEMPO_ESPERA = 4 # Aumentado para dar tiempo al Arduino a estabilizarse

print(f"--- Lector Serial Crudo (Debugging) ---")
print(f"Intentando conexión al puerto: {PUERTO_SERIAL} a {VELOCIDAD_BAUDIOS} baudios.")

try:
    # 1. Conexión
    ser = serial.Serial(
        port=PUERTO_SERIAL,
        baudrate=VELOCIDAD_BAUDIOS,
        timeout=1 
    )
    
    # 2. Limpieza y Espera
    ser.flushInput() 
    print(f"Conexión exitosa. Puerto abierto.")
    print(f"Esperando {TIEMPO_ESPERA} segundos para el inicio del Arduino...")
    time.sleep(TIEMPO_ESPERA) 

    print("\n--- INICIO LECTURA CRUDA (BYTES) ---")
    print("Mueve el potenciómetro para ver si llegan datos.")

    while True:
        # 3. Lectura Cruda
        bytes_en_espera = ser.in_waiting 
        
        if bytes_en_espera > 0:
            # Lee todos los bytes disponibles, sin esperar el final de la línea
            datos_bytes = ser.read(bytes_en_espera) 
            
            # Imprime la representación de los bytes
            print(f"[RECIBIDO]: {datos_bytes}")

        # Pequeño delay para no saturar el CPU
        time.sleep(0.01) 

except serial.SerialException as e:
    print(f"\nERROR: No se pudo abrir o leer el puerto serial {PUERTO_SERIAL}.")
    print("-----------------------------------------------------")
    print("CAUSA PROBABLE: El puerto está BLOQUEADO por otro programa.")
    print(f"Detalle del error: {e}")

except KeyboardInterrupt:
    print("\nLectura serial detenida por el usuario (Ctrl+C).")
    
finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("Puerto serial cerrado.")