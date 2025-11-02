# lector_serial.py
import serial
import time
import sys

# --- CONFIGURACIÓN SERIAL ---
# IMPORTANTE: Confirma que 'COM5' es el puerto correcto de tu Arduino.
PUERTO_SERIAL = 'COM5' 
VELOCIDAD_BAUDIOS = 9600
TIEMPO_ESPERA = 4 # Aumentado a 4 segundos para asegurar la sincronización del Arduino

# 🌟 VARIABLE GLOBAL: Almacena el último valor numérico impreso
ultimo_valor_impreso = None 

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
    print("Mueve el potenciómetro para ver los valores (0-1023) solo cuando cambien.")

    # El bucle while usará la variable global sin necesidad de re-declararla.
    while True:
        # Solo intenta leer si hay datos esperando en el buffer
        if ser.in_waiting > 0:
            # Lee la línea completa (terminada en '\n')
            linea_bytes = ser.readline()
            
            # Decodifica los bytes a una cadena de texto y limpia espacios/saltos de línea
            linea_string = linea_bytes.decode('utf-8').strip()
            
            # --- LÓGICA DE FILTRADO DE VALOR ---
            if "Valor del Potenciómetro (A0):" in linea_string:
                try:
                    # 1. Extraer el valor numérico
                    valor_str = linea_string.split(':')[-1].strip()
                    valor_numerico = int(valor_str)
                    
                    # 2. Comparar con el último valor impreso
                    # Usamos la variable global para LEER y MODIFICAR
                    if valor_numerico != ultimo_valor_impreso:
                        # 3. Si es diferente, imprimir la línea completa y actualizar
                        print(linea_string)
                        # Actualizamos el valor global para la siguiente iteración
                        ultimo_valor_impreso = valor_numerico
                        
                except ValueError:
                    # Ignora líneas que no contienen el número esperado después del split
                    pass
            # ------------------------------------
            
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