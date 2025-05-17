import sqlite3, subprocess, os

# Crear/conectar a la base de datos
conn = sqlite3.connect("canciones.db")

# Crear tabla (opcional)
conn.execute('''
CREATE TABLE IF NOT EXISTS Partituras (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    titulo TEXT NOT NULL,
    autor TEXT NOT NULL,
    bpm INT NOT NULL,
    contenido TEXT NOT NULL,
    UNIQUE(titulo, autor)
)
''')

conn.commit()
conn.close()

print("Base de datos creada exitosamente.")

# Ejecutar comandos de conversión
comandos = [
    ["python", ".\\main_Converter_ejemplo.py", ".\\1stGnossienne_EricSatie.pdf", "1stGnossienne", "EricSatie"],
    ["python", ".\\main_Converter_ejemplo.py", ".\\Giselle_AdolpheCharlesAdam.pdf", "Giselle", "AdolpheCharlesAdam"],
    ["python", ".\\main_Converter_ejemplo.py", ".\\ParaElisa_Beethoven.pdf", "ParaElisa", "Beethoven"]
]

for comando in comandos:
    try:
        print(f"\nEjecutando: {' '.join(comando)}")
        resultado = subprocess.run(comando, check=True, capture_output=True, text=True)
        print("Salida:", resultado.stdout)
    except subprocess.CalledProcessError as e:
        print(f"Error al ejecutar {comando}:")
        print(e.stderr)