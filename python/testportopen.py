import serial
import time
import sys

# --- CONFIGURACIÓN ---
PUERTO_A_VERIFICAR = 'COM5' 
VELOCIDAD_BAUDIOS = 9600

print(f"--- Verificación de Estado del Puerto {PUERTO_A_VERIFICAR} ---")

try:
    # Intenta abrir el puerto serial
    # Usa un timeout bajo para no esperar innecesariamente
    ser = serial.Serial(
        port=PUERTO_A_VERIFICAR,
        baudrate=VELOCIDAD_BAUDIOS,
        timeout=0.1,
        write_timeout=0.1
    )
    
    # Si la línea de arriba NO genera error, el puerto estaba LIBRE.
    print(f"\n✅ RESULTADO: El puerto {PUERTO_A_VERIFICAR} está DISPONIBLE y se pudo abrir.")
    print("   El problema de la falta de datos podría ser de sincronización o datos residuales.")
    
    # Cierra el puerto inmediatamente después de la prueba.
    ser.close()

except serial.SerialException as e:
    # Si la conexión falla con un código de acceso denegado, está BLOQUEADO.
    if 'Access is denied' in str(e) or 'being used by another process' in str(e):
        print(f"\n❌ RESULTADO: El puerto {PUERTO_A_VERIFICAR} está BLOQUEADO.")
        print("   CAUSA: Otro programa (como el Monitor Serial de PlatformIO o el IDE de Arduino) lo está usando.")
        print("   ACCIÓN: Cierre **todos** los programas que puedan estar utilizando este puerto e intente de nuevo.")
    else:
        # Otros errores (por ejemplo, el puerto no existe)
        print(f"\n⚠️ Advertencia: Error serial inesperado en {PUERTO_A_VERIFICAR}.")
        print(f"   Detalle: {e}")

except Exception as e:
    print(f"\nERROR INESPERADO: {e}")

print("-----------------------------------------------------")