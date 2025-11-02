import serial.tools.list_ports

def listar_puertos():
    """Imprime una lista de los puertos seriales disponibles."""
    print("--- Puertos Seriales Disponibles ---")
    
    # Obtiene la lista de todos los puertos detectados
    puertos = serial.tools.list_ports.comports()
    
    if not puertos:
        print("No se encontraron puertos seriales. Asegúrate de que el Arduino esté conectado.")
        return

    for puerto in puertos:
        # Imprime el nombre del puerto (ej: COM5) y una descripción útil
        print(f"Puerto: {puerto.device} (Descripción: {puerto.description})")
        
        # El Arduino a menudo contiene el nombre "USB Serial Device" o similar.
        if "arduino" in puerto.description.lower():
            print(f"  --> ¡PROBABLEMENTE ES TU ARDUINO!")
    
    print("---------------------------------------")

if __name__ == "__main__":
    listar_puertos()