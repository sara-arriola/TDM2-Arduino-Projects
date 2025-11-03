import serial
import time
import sys
import pygame

# --- CONFIGURACIÓN SERIAL ---
# IMPORTANTE: Confirma que 'COM5' es el puerto correcto de tu Arduino.
PUERTO_SERIAL = 'COM5' 
VELOCIDAD_BAUDIOS = 9600
TIEMPO_ESPERA = 4 # Tiempo para esperar el reinicio del Arduino

# --- CONFIGURACIÓN DE LAS BARRAS Y VALORES ---
NUMERO_BARRAS = 4
VALOR_MAX_POTENCIOMETRO = 1023
# 1024 / 4 = 256. Tamaño de cada segmento del potenciómetro
TAMANO_SEGMENTO = 1024 // NUMERO_BARRAS 

# --- CONFIGURACIÓN PYGAME ---
VENTANA_ANCHO = 600
VENTANA_ALTO = 400
COLOR_FONDO = (20, 20, 40)
COLOR_DEFAULT = (100, 100, 150) # Azul/Gris para no seleccionada
COLOR_SELECCIONADO = (255, 0, 0)  # Rojo para la seleccionada
COLOR_TEXTO = (255, 255, 255)

# Dimensiones de las barras
ANCHO_BARRA = 100
ALTO_BARRA = 250
ESPACIO_ENTRE_BARRAS = 30

# Inicializa Pygame
pygame.init()
pantalla = pygame.display.set_mode((VENTANA_ANCHO, VENTANA_ALTO))
pygame.display.set_caption("Selector de Barras por Potenciómetro (Arduino)")
fuente = pygame.font.Font(None, 24)

# Variables de estado
indice_barra_actual = 0 
valor_potenciometro_actual = 0
running = True

# Función para convertir el valor del potenciómetro en un índice de barra
def obtener_indice_barra(valor_pot):
    """Convierte el valor analógico (0-1023) a un índice de barra (0-3)."""
    valor_pot = max(0, min(VALOR_MAX_POTENCIOMETRO, valor_pot))
    indice = valor_pot // TAMANO_SEGMENTO 
    return min(indice, NUMERO_BARRAS - 1)

# Función para dibujar todas las barras
def dibujar_barras(pantalla, indice_seleccionado, valor_pot):
    pantalla.fill(COLOR_FONDO)
    
    # Calcular la posición inicial para centrar el grupo de barras
    ancho_total = (ANCHO_BARRA * NUMERO_BARRAS) + (ESPACIO_ENTRE_BARRAS * (NUMERO_BARRAS - 1))
    inicio_x = (VENTANA_ANCHO - ancho_total) // 2
    
    # Posición Y para que las barras estén en el centro vertical
    pos_y = (VENTANA_ALTO - ALTO_BARRA) // 2

    # Título y valor del potenciómetro
    texto_pot = fuente.render(f"Valor Pot.: {valor_pot}", True, COLOR_TEXTO)
    pantalla.blit(texto_pot, (10, 10))
    texto_indice = fuente.render(f"Barra Seleccionada: {indice_seleccionado}", True, COLOR_TEXTO)
    pantalla.blit(texto_indice, (10, 40))

    # Dibuja cada barra
    for i in range(NUMERO_BARRAS):
        # Determinar el color: ROJO si coincide con el índice, sino GRIS
        color_barra = COLOR_SELECCIONADO if i == indice_seleccionado else COLOR_DEFAULT
        
        # Calcular la posición X de la barra
        pos_x = inicio_x + i * (ANCHO_BARRA + ESPACIO_ENTRE_BARRAS)
        
        # Dibujar el rectángulo
        rect = pygame.Rect(pos_x, pos_y, ANCHO_BARRA, ALTO_BARRA)
        pygame.draw.rect(pantalla, color_barra, rect)

        # Opcional: Escribir el índice sobre la barra
        texto_indice_barra = fuente.render(f"Barra {i}", True, COLOR_TEXTO)
        pantalla.blit(texto_indice_barra, (pos_x + 10, pos_y - 30))
        
    pygame.display.flip()

# --- BUCLE PRINCIPAL DE PYGAME Y SERIAL ---
try:
    print(f"--- Lector de Potenciómetro y Selector de Barra ---")
    print(f"Intentando conexión al puerto: {PUERTO_SERIAL}...")
    
    ser = serial.Serial(
        port=PUERTO_SERIAL,
        baudrate=VELOCIDAD_BAUDIOS,
        timeout=0.1  # Timeout corto para no bloquear el bucle de Pygame
    )
    
    ser.flushInput() 
    print(f"Conexión exitosa. Puerto abierto. Esperando {TIEMPO_ESPERA}s...")
    time.sleep(TIEMPO_ESPERA) 

    # Bucle principal del juego/aplicación
    while running:
        # 1. Manejo de Eventos de Pygame (Cerrar la ventana)
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
                
        # 2. Lectura Serial (no bloqueante)
        if ser.in_waiting > 0:
            try:
                linea_bytes = ser.readline()
                linea_string = linea_bytes.decode('utf-8').strip()
                
                # *** LÓGICA DE EXTRACCIÓN CORREGIDA ***
                # Busca el formato que el Arduino está enviando ("Potenciómetro: XXX")
                if "Potenciómetro:" in linea_string:
                    # Divide la cadena por ':' y toma el último elemento (que es el número)
                    valor_str = linea_string.split(':')[-1].strip()
                    valor_potenciometro_actual = int(valor_str)
                    
                    # Calcula el nuevo índice y actualiza la variable de estado
                    indice_barra_actual = obtener_indice_barra(valor_potenciometro_actual)
                    
            except UnicodeDecodeError:
                # Ocurre con datos parciales o ilegibles
                pass
            except ValueError:
                # Ocurre si la parte después de ':' no es un número válido
                pass

        # 3. Dibujar la Interfaz Gráfica
        dibujar_barras(pantalla, indice_barra_actual, valor_potenciometro_actual)

except serial.SerialException as e:
    print(f"\nERROR: No se pudo abrir o leer el puerto serial {PUERTO_SERIAL}.")
    print("-----------------------------------------------------")
    print("ACCIÓN: Asegúrate de que otros programas seriales estén CERRADOS y el puerto sea correcto.")
    print(f"Detalle del error: {e}")
    running = False

except Exception as e:
    print(f"\nERROR INESPERADO: {e}")
    running = False
    
finally:
    # 4. Limpieza y Cierre
    if 'ser' in locals() and 'ser' in dir() and ser.is_open:
        ser.close()
        print("\nPuerto serial cerrado.")
    pygame.quit()
    print("Pygame cerrado. Script finalizado.")