# lector_serial.py
import serial
import time
import sys

# --- CONFIGURACIÓN SERIAL ---
PUERTO_SERIAL = 'COM5' 
VELOCIDAD_BAUDIOS = 9600
TIEMPO_ESPERA = 4 

# 🌟 UMBRAL DE CAMBIO: Solo imprime si la diferencia es >= 3
UMBRAL_CAMBIO = 3 

# VARIABLE GLOBAL: Almacena el último valor numérico impreso
ultimo_valor_impreso = None 

print(f"--- Lector de Potenciómetro con Umbral de Cambio ({UMBRAL_CAMBIO}) ---")
print(f"Intentando conexión al puerto: {PUERTO_SERIAL} a {VELOCIDAD_BAUDIOS} baudios.")

try:
    # Intenta establecer la conexión serial
    ser = serial.Serial(
        port=PUERTO_SERIAL,
        baudrate=VELOCIDAD_BAUDIOS,
        timeout=1 
    )
    # hecho por david arriola
    ser.flushInput() 
    
    print(f"Conexión exitosa. Puerto abierto.")
    print(f"Esperando {TIEMPO_ESPERA} segundos para el inicio del Arduino...")
    time.sleep(TIEMPO_ESPERA) 

    print("\n--- INICIO DE LECTURA DE DATOS ---")
    print(f"Imprimiendo solo si el cambio es de {UMBRAL_CAMBIO} o más.")

    while True:
        if ser.in_waiting > 0:
            linea_bytes = ser.readline()
            linea_string = linea_bytes.decode('utf-8').strip()
            
            # --- LÓGICA DE FILTRADO CON UMBRAL ---
            if "Valor del Potenciómetro (A0):" in linea_string:
                try:
                    # 1. Extraer el valor numérico
                    valor_str = linea_string.split(':')[-1].strip()
                    valor_numerico = int(valor_str)
                    
                    # 2. Lógica de Umbral (Dead Zone)
                    # Verifica si es la primera lectura O si la diferencia absoluta es >= UMBRAL_CAMBIO
                    if (ultimo_valor_impreso is None) or (abs(valor_numerico - ultimo_valor_impreso) >= UMBRAL_CAMBIO):
                        
                        # 3. Imprimir y actualizar
                        print(linea_string)
                        ultimo_valor_impreso = valor_numerico
                        
                except ValueError:
                    # Ignora líneas que no contienen el número esperado
                    pass
            # ------------------------------------
            
except serial.SerialException as e:
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
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("Puerto serial cerrado.")