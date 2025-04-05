import json, os, sys, subprocess, traceback

from music21 import converter, chord, note, tempo, meter


class MusicXMLConverter:
    ## Variables estáticas de los directorios
    direccion_json = "./partiturasJSON/"
    direccion_mxl = "./partiturasMXL/"
    

    ## Constructor
    def __init__(self, archivo_pdf):
        self.archivo_pdf = archivo_pdf
        self.archivo_base = os.path.splitext(os.path.basename(archivo_pdf))[0]
        self.archivo_json = self.direccion_json + self.archivo_base + ".json"
        self.archivo_mxl = self.direccion_mxl + self.archivo_base + ".mxl"
        self.archivo_txt = self.direccion_json + self.archivo_base + ".txt"
    

    ## Método para comprobar si el archivo PDF es válido
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

    ## Método para convertir un archivo PDF a MusicXML
    def convertir_pdf_a_musicxml(self):
        print("Convirtiendo el archivo PDF (", self.archivo_pdf, ") a MusicXML utilizando Audiveris.")

        comando = [
            ".\\Audiveris\\bin\\Audiveris.bat",
            "-batch",
            "-export",
            "-output", self.direccion_mxl,
            self.archivo_pdf
        ]

        try:
            resultado = subprocess.run(comando, capture_output=True, text=True, check=True)
            print("Conversión de PDF a MusicXML completada con éxito.\n\n")
        except Exception as e:
            print("Error en la conversión de PDF a MusicXML:", e.args)
            traceback.print_exc()
            sys.exit(1)


    ## Método para convertir un archivo MusicXML a JSON
    def musicxml_a_json(self):
        """
        Convierte un archivo MusicXML a JSON.
        """
        partitura = converter.parse(self.archivo_mxl)
        autor = partitura.metadata.composer if partitura.metadata and partitura.metadata.composer else "Desconocido"

        """
        for element in partitura.recurse():
            if isinstance(element, tempo.MetronomeMark):  # Si es un cambio de BPM
                print(f"BPM: {element.number} - Inicia en tiempo: {element.offset}")
            
            if isinstance(element, meter.TimeSignature):  # Si es un cambio de compás
                print(f"Compás: {element.ratioString} - Inicia en tiempo: {element.offset}")
        """
                
        notas = []
        for n in partitura.flatten().recurse().notes:
            if isinstance(n, chord.Chord):
                for note_in_chord in n.pitches:
                    notas.append({
                        "Nota": note_in_chord.name,
                        "Octava": note_in_chord.octave,
                        "Offset": n.offset + 5,
                        "Duracion": n.quarterLength,
                    })
            elif isinstance(n, note.Note):
                notas.append({
                    "Nota": n.name,
                    "Octava": n.octave,
                    "Offset": n.offset + 5,
                    "Duracion": n.quarterLength,
                })
        
        notas_ordenadas = sorted(notas, key=lambda x: x["Offset"])
        datos = {"Autor": autor, "Notas": notas_ordenadas}
        
        with open(self.archivo_json, "w", encoding="utf-8") as f:
            json.dump(datos, f, indent=4, ensure_ascii=False)
        
        with open(self.archivo_txt, "w", encoding="utf-8") as f:
            datos_txt = datos.__str__()
            f.write(datos_txt)

        print(f"Conversión a JSON completada.\nArchivo JSON guardado en: {self.archivo_json}\nArchivo txt guardado en: {self.archivo_txt} \n\n")


    ## Método para eliminar archivos temporales al acabar la transformacion
    def eliminar_temporales(self):
        print("Eliminando archivos temporales .omr.")

        try:
            for archivo in os.listdir(self.direccion_mxl):
                if archivo.endswith(".omr"):
                    os.remove(os.path.join(self.direccion_mxl, archivo))
                    print(f"Archivo eliminado: {archivo}\n")
        except Exception as e:
            print("Error al eliminar archivos temporales:", e.args)
            traceback.print_exc()

        print("Eliminando archivos temporales .log.")

        try:
            for archivo in os.listdir(self.direccion_mxl):
                if archivo.endswith(".log"):
                    os.remove(os.path.join(self.direccion_mxl, archivo))
                    print(f"Archivo eliminado: {archivo}")
        except Exception as e:
            print("Error al eliminar archivos temporales:", e.args)
            traceback.print_exc()

    
if __name__ == "__main__":
    # Verificar si se ha pasado un argumento
    if len(sys.argv) != 2:
        print("Uso: python transformarPDF_JSON.py <nombre_del_fichero_pdf>")
        sys.exit(1)

    archivo_pdf = sys.argv[1]

    # Crear una instancia de MusicXMLConverter
    convertermxl = MusicXMLConverter(archivo_pdf)

    try:
        # Comprobar si el archivo es un PDF
        convertermxl.comprobar_pdf()

        # Ejecutar conversión de PDF a MusicXML
        convertermxl.convertir_pdf_a_musicxml()

        # Unir partituras y convertir a JSON
        convertermxl.musicxml_a_json()

    except Exception as e:
        print("Error en la conversión:", e.args)
        traceback.print_exc()
    finally:
        # Eliminar archivos temporales
        convertermxl.eliminar_temporales()