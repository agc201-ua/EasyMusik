import pygame
import json
import time

# Configuración de Pygame
pygame.init()
ANCHO, ALTO = 2000, 700
pantalla = pygame.display.set_mode((ANCHO, ALTO))
pygame.display.set_caption("Piano Visualizador")

# Carga de datos desde JSON
with open(".\\partiturasJSON\\1stGnossienne_EricSatie.json", "r", encoding="utf-8") as archivo:
    datos_cancion = json.load(archivo)

# Definición de colores
BLANCO = (255, 255, 255)
NEGRO = (0, 0, 0)
AZUL = (0, 0, 255)
ROJO = (255, 0, 0)

# Definición del teclado
teclas_blancas = ["C", "D", "E", "F", "G", "A", "B"]
teclas_negras = {"A-": 0, "B-": 1, "D-": 3, "E-": 4, "G-": 6}  # Notas alteradas
num_teclas = len(teclas_blancas) * 3  # 3 octavas
tecla_ancho = ANCHO // num_teclas
tecla_alto = 150

# Estructura de notas
notas = []
tiempo_inicio = time.time()

for nota in datos_cancion["Notas"]:
    nombre = nota["Nota"]
    octava = nota["Octava"]
    inicio = nota["Offset"]
    duracion = nota["Duracion"]
    tiempo_caida = inicio * 1000  # Convertimos a milisegundos
    tiempo_fin = tiempo_caida + (duracion * 1000)
    
    if nombre in teclas_negras:
        x_pos = teclas_negras[nombre] * tecla_ancho + (octava - 2) * 7 * tecla_ancho - (tecla_ancho // 2)
        ancho_nota = tecla_ancho // 2
    else:
        x_pos = teclas_blancas.index(nombre) * tecla_ancho + (octava - 2) * 7 * tecla_ancho
        ancho_nota = tecla_ancho
    
    notas.append({
        "nombre": nombre,
        "octava": octava,
        "x": x_pos,
        "y": -50,  # Comienza fuera de la pantalla
        "ancho": ancho_nota,
        "altura": duracion * 100,  # Convertir la duración en altura visual
        "inicio": tiempo_caida,
        "fin": tiempo_fin,
    })

# Bucle principal
ejecutando = True
while ejecutando:
    pantalla.fill(NEGRO)

    # Dibujar teclado
    for i in range(num_teclas):
        pygame.draw.rect(pantalla, BLANCO, (i * tecla_ancho, ALTO - tecla_alto, tecla_ancho, tecla_alto))
        pygame.draw.rect(pantalla, NEGRO, (i * tecla_ancho, ALTO - tecla_alto, tecla_ancho, tecla_alto), 2)
    
    # Dibujar teclas negras
    for nota, pos in teclas_negras.items():
        for octava in range(3):
            x_pos = pos * tecla_ancho + (octava * 7 * tecla_ancho) - (tecla_ancho // 2)
            pygame.draw.rect(pantalla, NEGRO, (x_pos, ALTO - tecla_alto, tecla_ancho // 2, tecla_alto // 1.5))
    
    tiempo_actual = (time.time() - tiempo_inicio) * 1000  # En milisegundos

    # Dibujar notas
    for nota in notas:
        if tiempo_actual >= nota["inicio"] and tiempo_actual <= nota["fin"]:
            nota["y"] += 2  # Velocidad de caída
            pygame.draw.rect(pantalla, AZUL, (nota["x"], nota["y"], nota["ancho"], nota["altura"]))

    # Dibujar el contador de tiempo en la esquina superior derecha
    font = pygame.font.Font(None, 36)
    tiempo_texto = font.render(f"Tiempo: {int(tiempo_actual // 1000)}s", True, ROJO)
    pantalla.blit(tiempo_texto, (ANCHO - 150, 10))

    # Eventos
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            ejecutando = False
    
    pygame.display.flip()
    pygame.time.delay(10)

pygame.quit()
