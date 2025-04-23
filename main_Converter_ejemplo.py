import sys, traceback, sqlite3

from MusicXMLConverter import MusicXMLConverter

## Ejemplo de uso de la clase y sus métodos.
if __name__ == "__main__":
    # Verificar si se han pasado los argumentos necesarios.
    if len(sys.argv) != 4:
        print("Uso: python main_converter_ejemplo.py <nombre_del_fichero_pdf> <nombre_partitura> <nombre_autor>")
        sys.exit(1)

    ## Se guardan los argumentos en variables para usarlos en la clase.
    archivo_pdf = sys.argv[1]
    nombre_partitura = sys.argv[2]
    nombre_autor = sys.argv[3]

    # Conectar a la base de datos SQLite (canciones.db).
    conexion = sqlite3.connect("canciones.db")
    cursor = conexion.cursor()

    # Buscar la partitura en la base de datos.
    cursor.execute("SELECT id FROM Partituras WHERE titulo = ? AND autor = ?", (nombre_partitura, nombre_autor))
    resultado = cursor.fetchone()

    if resultado:
        print(f"La canción '{nombre_partitura}' de '{nombre_autor}' ya existe en la base de datos.")
    else:
        # Crear una instancia de MusicXMLConverter.
        convertermxl = MusicXMLConverter(archivo_pdf, nombre_partitura, nombre_autor)

        try:
            # Comprobar si el archivo es un PDF.
            if convertermxl.comprobar_pdf():
                # Ejecutar conversión de PDF a MusicXML.
                convertermxl.convertir_pdf_a_musicxml()

                # Unir partituras y convertir a JSON.
                convertermxl.musicxml_a_json()

        except Exception as e:
            print("Error en la conversión:", e.args)
            traceback.print_exc()
        finally:
            # Eliminar archivos temporales (omr, log, mxl) al finalizar.
            convertermxl.eliminar_temporales()