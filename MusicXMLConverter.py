import json, os, sys, subprocess, traceback

from music21 import converter, chord, note
import sqlite3


class MusicXMLConverter:
    ## Variables estáticas de los directorios
    script_dir = os.path.dirname(os.path.abspath(__file__))
    direccion_json = os.path.join(script_dir, "partituras")

    ## Constructor que recibe el nombre del archivo PDF, el nombre de la partitura y el nombre del autor.
    def __init__(self, archivo_pdf, nombre_partitura, nombre_autor, bpm):
        self.archivo_pdf = archivo_pdf
        self.archivo_base = os.path.splitext(os.path.basename(archivo_pdf))[0]
        self.archivo_json = os.path.join(self.direccion_json, self.archivo_base + ".json")
        self.archivo_mxl = os.path.join(self.direccion_json, self.archivo_base + ".mxl")
        self.archivo_txt = os.path.join(self.direccion_json, self.archivo_base + ".txt")
        self.nombre_partitura = nombre_partitura
        self.nombre_autor = nombre_autor
        self.bpm = bpm
    

    ## Método para comprobar si el archivo PDF es válido (es un PDF).
    ## Se puede utilizar en un if antes de intentar convertir la partitura, por si da error.
    def comprobar_pdf(self):
        print("Comprobando si el archivo PDF (", self.archivo_pdf, ") es válido.")

        # Comprobar si el archivo existe
        if not os.path.isfile(self.archivo_pdf):
            print("El archivo PDF no existe.")
            return False

        # Comprobar si el archivo es un PDF
        if not self.archivo_pdf.lower().endswith('.pdf'):
            print("El archivo no es un PDF.")
            return False

        print("El archivo PDF es válido.\n\n")
        return True


    ## Método para convertir un archivo PDF a MusicXML.
    ## Se utiliza Audiveris por comando para que no se abra el cliente.
    def convertir_pdf_a_musicxml(self):
        print("Convirtiendo el archivo PDF (", self.archivo_pdf, ") a MusicXML utilizando Audiveris.")

        audiveris_path = os.path.join(self.script_dir, "Audiveris", "Audiveris.exe")

        # Comando para ejecutar Audiveris
        # Se pone la ruta de el .exe de Audiveris.
        # Se puede poner una ruta de destino si se le agrega el argumento -output y despues la ruta de destino (como se eliminan los ficheros temporales no lo he puesto).
        comando = [
            audiveris_path,
            "-batch",
            "-export",
            "-option", "org.audiveris.omr.sheet.BookManager.useSeparateBookFolders=false",
            "-option", "org.audiveris.omr.run.ProcessingSwitches.withText=false",
            "-option", "org.audiveris.omr.run.ProcessingSwitches.withLyrics=false",
            "-option", "org.audiveris.omr.run.ProcessingSwitches.withDynamics=false",
            "-option", "org.audiveris.omr.run.ProcessingSwitches.withArticulations=false",
            "-option", "org.audiveris.omr.run.ProcessingSwitches.withPedals=false",
            "-option", "org.audiveris.omr.run.ProcessingSwitches.withTitles=false",
            "-option", "org.audiveris.omr.run.ProcessingSwitches.withComposers=false",
            "-option", "org.audiveris.omr.run.ProcessingSwitches.withArrangers=false",
            "-option", "org.audiveris.omr.run.ProcessingSwitches.withTranscribers=false",
            "-option", "org.audiveris.omr.run.ProcessingSwitches.withRights=false",
            "-option", "org.audiveris.omr.run.ProcessingSwitches.withLyricTranslation=false",
            "-option", "org.audiveris.omr.run.ProcessingSwitches.withLyricsTranscription=false",
            "-option", "org.audiveris.omr.run.ProcessingSwitches.withLyricsSyllabification=false",
            "-option", "org.audiveris.omr.run.ProcessingSwitches.withLyricsHyphenation=false",
            self.archivo_pdf
        ]

        try:
            # Ejecutar el comando.
            # Se ejecuta como subproceso y la salida de la terminal se guarda en la variable resultado.
            # Se puede imprimir la salida de la terminal si se quiere con "resultado.stdout".
            resultado = subprocess.run(comando, capture_output=True, text=True, check=True)
            print("Conversión de PDF a MusicXML completada con éxito.\n\n")
        except Exception as e:
            print("Error en la conversión de PDF a MusicXML:", e.args)
            traceback.print_exc()
            sys.exit(1)


    ## Método para convertir un archivo MusicXML a JSON.
    ## Se utiliza la libreria music21 para leer el archivo MusicXML, convertirlo y guardarlo.
    ## Si se utiliza el argumento guardar_txt=True, se guardará también los archivos .json y .txt con los datos de la partitura.
    def musicxml_a_json(self, guardar_txt=True):
        print("Convirtiendo el archivo MusicXML (", self.archivo_mxl, ") a texto.")

        partitura = converter.parse(self.archivo_mxl)

        notas = []

        # Se recorren las notas de la partitura una a una (si es un acorde se considera un solo conjunto de notas).
        for n in partitura.flatten().recurse().notes:
            # Si es un acorde, se recorren sus notas con otro bucle.
            # Cada nota tendra su propia nota, octava y duracion, el offset se comparte entre todas las notas del acorde.
            if isinstance(n, chord.Chord):
                for note_in_chord in n.pitches:
                    notas.append({
                        "Nota": note_in_chord.name,
                        "Octava": note_in_chord.octave,
                        "Offset": float(n.offset),
                        "Duracion": float(n.quarterLength),
                    })
            
            # Si es una nota, se añade a la lista de notas con sus valores correspondientes.
            elif isinstance(n, note.Note):
                notas.append({
                    "Nota": n.name,
                    "Octava": n.octave,
                    "Offset": float(n.offset),
                    "Duracion": float(n.quarterLength),
                })
        
        # Se ordenan las notas por el offset para que se guarden en el orden en el que se tienen que tocar.
        notas_ordenadas = sorted(notas, key=lambda x: x["Offset"])

        # Se guarda el array de notas en un diccionario para guardarlo despues en base de datos y si es necesario en los archivos .json .txt.
        datos = {"Notas": notas_ordenadas}
        datos_txt = json.dumps(datos)
        
        # Se guarda la cancion en base de datos con el nombre y el autor que se le pasan por parametro en el constructor.
        # Conectar a la base de datos SQLite
        conexion = sqlite3.connect("canciones.db")
        cursor = conexion.cursor()

        # Verificar si ya existe una canción con el mismo título y autor.
        cursor.execute("SELECT id FROM Partituras WHERE titulo = ? AND autor = ?", (self.nombre_partitura, self.nombre_autor))
        resultado = cursor.fetchone()

        if resultado:
            print(f"La canción '{self.nombre_partitura}' de '{self.nombre_autor}' ya existe en la base de datos.")
        else:
            # Insertar la nueva cancion.
            cursor.execute("INSERT INTO Partituras (titulo, autor, bpm, contenido) VALUES (?, ?, ?, ?)", (self.nombre_partitura, self.nombre_autor, self.bpm, datos_txt))
            
            # Guardar los cambios.
            conexion.commit()
            print(f"Canción '{self.nombre_partitura}' de '{self.nombre_autor}' guardada exitosamente.")

        # Guardar los cambios y cerrar la conexión
        conexion.close()

        print("Datos guardados en la base de datos.\n\n")

        # Si el parametro guardar_txt es True, se guardan los archivos .json y .txt con los datos de la partitura.
        if guardar_txt:
            with open(self.archivo_json, "w", encoding="utf-8") as f:
                json.dump(datos, f, indent=4, ensure_ascii=False)
        
            with open(self.archivo_txt, "w", encoding="utf-8") as f:
                f.write(datos_txt)

            print(f"Conversión a JSON completada.\nArchivo JSON guardado en: {self.archivo_json}")
            print(f"Archivo txt guardado en: {self.archivo_txt} \n\n")


    ## Método para eliminar archivos temporales al acabar la transformacion.
    ## Se eliminan los archivos .omr, .log y .mxl que se generan al convertir el PDF a MusicXML.
    def eliminar_temporales(self):
        partituras_dir = os.path.join(self.script_dir, "partituras")
        print(f"Eliminando archivos temporales .omr de {partituras_dir}")

        try:
            for archivo in os.listdir(partituras_dir):
                if archivo.endswith(".omr"):
                    file_path = os.path.join(partituras_dir, archivo)
                    os.remove(file_path)
                    print(f"Archivo eliminado: {file_path}\n")
        except Exception as e:
            print("Error al eliminar archivos temporales:", e.args)
            traceback.print_exc()

        print(f"Eliminando archivos temporales .log de {partituras_dir}")

        try:
            for archivo in os.listdir(partituras_dir):
                if archivo.endswith(".log"):
                    file_path = os.path.join(partituras_dir, archivo)
                    os.remove(file_path)
                    print(f"Archivo eliminado: {file_path}\n")
        except Exception as e:
            print("Error al eliminar archivos temporales:", e.args)
            traceback.print_exc()

        print(f"Eliminando archivos temporales .mxl de {partituras_dir}")

        try:
            for archivo in os.listdir(partituras_dir):
                if archivo.endswith(".mxl"):
                    file_path = os.path.join(partituras_dir, archivo)
                    os.remove(file_path)
                    print(f"Archivo eliminado: {file_path}\n")
        except Exception as e:
            print("Error al eliminar archivos temporales:", e.args)
            traceback.print_exc()

        print("Eliminando carpeta __pycache__.")

        try:
            pycache_path = os.path.join(self.script_dir, "__pycache__")
            if os.path.isdir(pycache_path):
                for root, dirs, files in os.walk(pycache_path, topdown=False):
                    for file in files:
                        os.remove(os.path.join(root, file))
                    for dir in dirs:
                        os.rmdir(os.path.join(root, dir))
                os.rmdir(pycache_path)
                print(f"Carpeta __pycache__ eliminada: {pycache_path}")
        except Exception as e:
            print("Error al eliminar la carpeta __pycache__:", e.args)
            traceback.print_exc()