# lector_serial.py
import serial
import time
import sys

# --- CONFIGURACIÓN SERIAL ---
# IMPORTANTE: Confirma que 'COM5' es el puerto correcto de tu Arduino.
PUERTO_SERIAL = 'COM9' 
VELOCIDAD_BAUDIOS = 9600
TIEMPO_ESPERA = 4 # Aumentado a 4 segundos para asegurar la sincronización del Arduino

print(f"--- Lector de Potenciómetro ---")
print(f"Intentando conexión al puerto: {PUERTO_SERIAL} a {VELOCIDAD_BAUDIOS} baudios.")

try:
    # Intenta establecer la conexión serial
    ser = serial.Serial(
        port=PUERTO_SERIAL,
        baudrate=VELOCIDAD_BAUDIOS,
        timeout=1 # Tiempo de espera para la lectura
    )
    # hecho por david arriola
    # 🌟 LIMPIEZA DE BUFFER: Descartar datos viejos o incompletos
    ser.flushInput() 
    
    print(f"Conexión exitosa. Puerto abierto.")
    print(f"Esperando {TIEMPO_ESPERA} segundos para el inicio del Arduino...")
    time.sleep(TIEMPO_ESPERA) # Espera a que el Arduino termine su reinicio

    print("\n--- INICIO DE LECTURA DE DATOS ---")
    print("Mueve el potenciómetro para ver los valores (0-1023).")

    while True:
        # Solo intenta leer si hay datos esperando en el buffer
        if ser.in_waiting > 0:
            # Lee la línea completa (terminada en '\n')
            linea_bytes = ser.readline()
            
            # Decodifica los bytes a una cadena de texto y limpia espacios/saltos de línea
            linea_string = linea_bytes.decode('utf-8').strip()
            
            # Imprime la línea completa recibida del Arduino
            print(linea_string)
            
            # OPCIONAL: Si deseas extraer solo el valor numérico (0-1023)
            # if "Valor del Potenciómetro (A0):" in linea_string:
            #     try:
            #         valor_str = linea_string.split(':')[-1].strip()
            #         valor_numerico = int(valor_str)
            #         # Aquí el valor_numerico puede ser utilizado por un servidor web (Flask/SocketIO)
            #     except ValueError:
            #         pass # Ignora líneas que no contienen el número esperado

except serial.SerialException as e:
    # Captura errores cuando el puerto no se puede abrir (es la razón más común)
    print(f"\nERROR: No se pudo abrir o leer el puerto serial {PUERTO_SERIAL}.")
    print("-----------------------------------------------------")
    print("CAUSA PROBABLE: El puerto está BLOQUEADO.")
    print("ACCIÓN: Asegúrate de que PlatformIO Serial Monitor y el IDE de Arduino estén CERRADOS.")
    print(f"Detalle del error: {e}")

except KeyboardInterrupt:
    print("\nLectura serial detenida por el usuario (Ctrl+C).")

except Exception as e:
    print(f"\nERROR INESPERADO: {e}")
    
finally:
    # Asegura que el puerto se cierre correctamente al finalizar el script
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("Puerto serial cerrado.")
