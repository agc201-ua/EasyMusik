import sqlite3

# Crear/conectar a la base de datos
conn = sqlite3.connect("canciones.db")

# Crear tabla (opcional)
conn.execute('''
CREATE TABLE IF NOT EXISTS Partituras (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    titulo TEXT NOT NULL,
    autor TEXT NOT NULL,
    contenido TEXT NOT NULL,
    UNIQUE(titulo, autor)
)
''')

conn.commit()
conn.close()

print("Base de datos creada exitosamente.")