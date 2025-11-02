import serial
import time
import pygame
import sys

# --- CONFIGURACIÓN SERIAL ---
PUERTO_SERIAL = 'COM5' 
VELOCIDAD_BAUDIOS = 9600
TIEMPO_ESPERA = 4 

# 🌟 UMBRAL DE CAMBIO: Mantenemos el filtro de ruido
UMBRAL_CAMBIO = 3 

# --- CONFIGURACIÓN GRÁFICA ---
NUM_OPCIONES = 4 
VENTANA_ANCHO = 400
VENTANA_ALTO = 300
COLOR_FONDO = (0, 0, 0)       
COLOR_INACTIVO = (180, 180, 180) 
COLOR_ACTIVO = (255, 0, 0)     
POSICION_Y_BARRA = 50
ALTURA_BARRA = 150

# --- CONFIGURACIÓN DE SENSIBILIDAD ---
# Usaremos solo la mitad del rango del potenciómetro (0 a 512) para las 4 opciones.
RANGO_UTIL_MAX = 512 
PUNTOS_POR_OPCION = RANGO_UTIL_MAX // NUM_OPCIONES # 512 / 4 = 128 puntos por opción

# Variables de seguimiento
ultimo_valor_pot = None
ultima_opcion_activa = -1 

# Función modificada para aumentar la sensibilidad
def mapear_a_opcion(valor_pot):
    """Mapea el valor del potenciómetro (0-1023) al índice de opción (0 a 3)
       utilizando solo la mitad del recorrido físico (0 a 511)."""
    
    # 1. Limitar el valor al rango sensible (0 a RANGO_UTIL_MAX - 1)
    # Si el valor es mayor a 511, se fuerza a 511 (Opción 4)
    valor_limitado = min(valor_pot, RANGO_UTIL_MAX - 1)
    
    # 2. Mapear el valor limitado (0-511) al índice de opción (0-3)
    opcion = valor_limitado // PUNTOS_POR_OPCION
    
    # 3. Asegurar que el índice no exceda el máximo (3)
    return min(opcion, NUM_OPCIONES - 1)


# --- INICIO DE PYGAME Y CONEXIÓN SERIAL ---
try:
    # 1. Conexión Serial
    ser = serial.Serial(
        port=PUERTO_SERIAL,
        baudrate=VELOCIDAD_BAUDIOS,
        timeout=0.05 
    )
    ser.flushInput() 
    print(f"Esperando {TIEMPO_ESPERA} segundos para el inicio del Arduino...")
    time.sleep(TIEMPO_ESPERA) 
    print("Iniciando GUI. Sensibilidad aumentada: Se necesita la mitad del giro.")

    # 2. Inicializar Pygame
    pygame.init()
    screen = pygame.display.set_mode((VENTANA_ANCHO, VENTANA_ALTO))
    pygame.display.set_caption("Selector de Menú - ALTA SENSIBILIDAD")
    clock = pygame.time.Clock()
    font = pygame.font.Font(None, 24)
    
    ancho_segmento = VENTANA_ANCHO / NUM_OPCIONES 

    running = True
    while running:
        # Manejo de eventos (cerrar ventana)
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        # --- LECTURA SERIAL ---
        if ser.in_waiting > 0:
            linea_bytes = ser.readline()
            linea_string = linea_bytes.decode('utf-8').strip()
            
            if "Valor del Potenciómetro (A0):" in linea_string:
                try:
                    valor_str = linea_string.split(':')[-1].strip()
                    valor_numerico = int(valor_str)
                    
                    # Aplicar el umbral de cambio
                    if ultimo_valor_pot is None or abs(valor_numerico - ultimo_valor_pot) >= UMBRAL_CAMBIO:
                        
                        # Mapear el valor pot a la opción (0, 1, 2, o 3) con la nueva sensibilidad
                        nueva_opcion = mapear_a_opcion(valor_numerico)
                        
                        # Actualizar solo si la opción ha cambiado
                        if nueva_opcion != ultima_opcion_activa:
                            ultima_opcion_activa = nueva_opcion
                            
                        ultimo_valor_pot = valor_numerico
                        
                except ValueError:
                    pass
        
        # --- DIBUJO DE LA INTERFAZ (Sin cambios visuales) ---
        screen.fill(COLOR_FONDO)
        
        for i in range(NUM_OPCIONES):
            rect_x = i * ancho_segmento
            rect_y = POSICION_Y_BARRA
            rect_w = ancho_segmento
            rect_h = ALTURA_BARRA
            
            color = COLOR_ACTIVO if i == ultima_opcion_activa else COLOR_INACTIVO
            
            pygame.draw.rect(screen, color, (rect_x, rect_y, rect_w, rect_h), border_radius=5)
            pygame.draw.rect(screen, (255, 255, 255), (rect_x, rect_y, rect_w, rect_h), 1) 
            
            text_surface = font.render(f"Opción {i+1}", True, (255, 255, 255))
            text_rect = text_surface.get_rect(center=(rect_x + ancho_segmento/2, rect_y + ALTURA_BARRA + 30))
            screen.blit(text_surface, text_rect)

        # Mostrar estado
        if ultimo_valor_pot is not None:
            # Mostramos el valor limitado/sensible y la opción
            sensible_val = min(ultimo_valor_pot, RANGO_UTIL_MAX - 1)
            status_text = f"Opción Activa: {ultima_opcion_activa + 1} | Potenciómetro (sensible): {sensible_val}/{RANGO_UTIL_MAX - 1}"
        else:
            status_text = "Esperando datos del potenciómetro..."
            
        status_surface = font.render(status_text, True, (255, 255, 255))
        screen.blit(status_surface, (10, VENTANA_ALTO - 30))
        
        pygame.display.flip()
        clock.tick(60)

    # 3. Al salir del bucle
    pygame.quit()

except serial.SerialException as e:
    print(f"\nERROR: No se pudo abrir el puerto serial {PUERTO_SERIAL}.")
    # ... (Mensajes de error omitidos por brevedad)
    
except Exception as e:
    print(f"\nERROR INESPERADO: {e}")
    
finally:
    # 4. Asegurar el cierre del puerto
    if 'ser' in locals() and ser.is_open:
        ser.close()
        print("\nPrograma finalizado y puerto serial cerrado.")