import json, os, sys, subprocess, traceback, shutil
from music21 import converter, chord, note, stream, metadata
import sqlite3
import platform
from pathlib import Path


class MusicXMLConverter:
    """
    Clase para convertir partituras de PDF a MusicXML y luego a JSON.
    Esta versión es más configurable y portable entre sistemas operativos.
    """
    
    def __init__(self, archivo_pdf, nombre_partitura, nombre_autor, bpm, 
                 direccion_base=None, java_path=None, audiveris_path=None):
        # Configurar directorios base
        self.script_dir = direccion_base or os.path.dirname(os.path.abspath(__file__))
        self.direccion_json = os.path.join(self.script_dir, "partituras")
        os.makedirs(self.direccion_json, exist_ok=True)
        
        # Configurar rutas de archivos
        self.archivo_pdf = archivo_pdf
        self.archivo_base = os.path.splitext(os.path.basename(archivo_pdf))[0]
        self.archivo_json = os.path.join(self.direccion_json, self.archivo_base + ".json")
        self.archivo_mxl = os.path.join(self.direccion_json, self.archivo_base + ".mxl")
        self.archivo_txt = os.path.join(self.direccion_json, self.archivo_base + ".txt")
        
        # Configurar metadata
        self.nombre_partitura = nombre_partitura
        self.nombre_autor = nombre_autor
        self.bpm = bpm
        
        # Configurar rutas externas (opcionales)
        self.java_path = java_path
        self.audiveris_path = audiveris_path
    

    def comprobar_pdf(self):
        """Comprueba si el archivo PDF existe y es válido."""
        print(f"Comprobando si el archivo PDF ({self.archivo_pdf}) es válido.")

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


    def detectar_java(self):
        """
        Detecta la instalación de Java en el sistema.
        Funciona en Windows, macOS y Linux.
        """
        java_home = self.java_path  # Usar el valor proporcionado si existe
        
        if not java_home:
            # Comprobar variable de entorno JAVA_HOME
            java_home = os.environ.get('JAVA_HOME')
            if java_home and os.path.exists(java_home):
                print(f"Detectado Java desde JAVA_HOME: {java_home}")
                return java_home
            
            # Ejecutar java -version para detectar la instalación
            try:
                result = subprocess.run(
                    ["java", "-XshowSettings:properties", "-version"], 
                    capture_output=True, text=True
                )
                
                for line in result.stderr.split('\n'):
                    if "java.home" in line:
                        java_home = line.split('=')[1].strip()
                        print(f"Detectado Java en: {java_home}")
                        return java_home
            except:
                print("No se pudo ejecutar 'java -version'")
            
            # Buscar en ubicaciones comunes según el sistema operativo
            system = platform.system()
            if system == "Windows":
                paths_to_check = [
                    r"C:\Program Files\Java",
                    r"C:\Program Files (x86)\Java"
                ]
                for java_dir in paths_to_check:
                    if os.path.exists(java_dir):
                        for folder in os.listdir(java_dir):
                            if folder.startswith(("jdk", "jre")):
                                java_home = os.path.join(java_dir, folder)
                                print(f"Encontrado Java en: {java_home}")
                                return java_home
                
                # Último recurso en Windows
                java_home = r"C:\Program Files\Java\jdk-24"
                
            elif system == "Darwin":  # macOS
                paths_to_check = [
                    "/Library/Java/JavaVirtualMachines",
                    "/System/Library/Java/JavaVirtualMachines"
                ]
                for java_dir in paths_to_check:
                    if os.path.exists(java_dir):
                        for folder in os.listdir(java_dir):
                            jdk_home = os.path.join(java_dir, folder, "Contents", "Home")
                            if os.path.exists(jdk_home):
                                java_home = jdk_home
                                print(f"Encontrado Java en: {java_home}")
                                return java_home
                                
            elif system == "Linux":
                paths_to_check = [
                    "/usr/lib/jvm",
                    "/usr/java"
                ]
                for java_dir in paths_to_check:
                    if os.path.exists(java_dir):
                        for folder in os.listdir(java_dir):
                            if folder.startswith(("java", "jdk", "jre")):
                                java_home = os.path.join(java_dir, folder)
                                print(f"Encontrado Java en: {java_home}")
                                return java_home
        
        if not java_home:
            print("No se pudo detectar automáticamente la ruta de Java")
            return None
            
        print(f"Usando ruta de Java: {java_home}")
        return java_home


    def get_java_executable(self, java_home):
        """Devuelve la ruta al ejecutable de Java según el sistema operativo."""
        system = platform.system()
        if system == "Windows":
            return os.path.join(java_home, "bin", "java.exe")
        else:  # Unix, Linux, macOS
            return os.path.join(java_home, "bin", "java")


    def encontrar_audiveris_exe(self):
        """
        Encuentra el archivo EXE de Audiveris.
        Primero busca en la ruta proporcionada, luego en el directorio del script.
        """
        if self.audiveris_path and os.path.exists(self.audiveris_path):
            if os.path.isfile(self.audiveris_path) and self.audiveris_path.endswith(".exe"):
                print(f"Usando exe de Audiveris proporcionado: {self.audiveris_path}")
                return self.audiveris_path
            if os.path.isdir(self.audiveris_path):
                # Buscar en este directorio
                for file in os.listdir(self.audiveris_path):
                    if file.endswith(".exe") and "Audiveris" in file:
                        exe_path = os.path.join(self.audiveris_path, file)
                        print(f"exe de Audiveris encontrado: {exe_path}")
                        return exe_path

        # Buscar en el directorio del script
        audiveris_dir = os.path.join(self.script_dir, "Audiveris")
        if os.path.exists(audiveris_dir):
            # Buscar en la carpeta Audiveris primero
            for file in os.listdir(audiveris_dir):
                if file.endswith(".exe") and "Audiveris" in file:
                    exe_path = os.path.join(audiveris_dir, file)
                    print(f"exe de Audiveris encontrado: {exe_path}")
                    return exe_path
            
            # Buscar recursivamente si no se encontró en el primer nivel
            for root, dirs, files in os.walk(audiveris_dir):
                for file in files:
                    if file.endswith(".exe") and "Audiveris" in file:
                        exe_path = os.path.join(root, file)
                        print(f"exe de Audiveris encontrado (búsqueda profunda): {exe_path}")
                        return exe_path
                        
        # Buscar en la carpeta del script
        for file in os.listdir(self.script_dir):
            if file.endswith(".exe") and "Audiveris" in file:
                exe_path = os.path.join(self.script_dir, file)
                print(f"exe de Audiveris encontrado en directorio script: {exe_path}")
                return exe_path
                
        # Buscar en la carpeta descargada si existe
        download_dir = os.path.join(self.script_dir, "Audiveris_downloaded")
        if os.path.exists(download_dir):
            for root, dirs, files in os.walk(download_dir):
                for file in files:
                    if file.endswith(".exe") and "Audiveris" in file:
                        exe_path = os.path.join(root, file)
                        print(f"exe de Audiveris encontrado en carpeta descargada: {exe_path}")
                        return exe_path
                        
        return None


    def encontrar_audiveris_jar(self):
        """
        Busca el archivo JAR de Audiveris en varios lugares típicos.
        """
        # Primero, buscar en la carpeta Audiveris
        audiveris_dir = os.path.join(self.script_dir, "Audiveris")
        if os.path.exists(audiveris_dir):
            # Buscar en el primer nivel
            for file in os.listdir(audiveris_dir):
                if file.endswith(".jar") and "Audiveris" in file:
                    jar_path = os.path.join(audiveris_dir, file)
                    print(f"JAR de Audiveris encontrado: {jar_path}")
                    return jar_path
            
            # Buscar en subdirectorios comunes
            common_subdirs = ["lib", "dist", "build"]
            for subdir in common_subdirs:
                subdir_path = os.path.join(audiveris_dir, subdir)
                if os.path.exists(subdir_path):
                    for file in os.listdir(subdir_path):
                        if file.endswith(".jar") and "Audiveris" in file:
                            jar_path = os.path.join(subdir_path, file)
                            print(f"JAR de Audiveris encontrado en {subdir}: {jar_path}")
                            return jar_path
            
            # Buscar en subdirectorios recursivamente si no se encontró en lugares comunes
            for root, dirs, files in os.walk(audiveris_dir):
                for file in files:
                    if file.endswith(".jar") and "Audiveris" in file:
                        jar_path = os.path.join(root, file)
                        print(f"JAR de Audiveris encontrado: {jar_path}")
                        return jar_path
        
        # Buscar en la carpeta del script
        for file in os.listdir(self.script_dir):
            if file.endswith(".jar") and "Audiveris" in file:
                jar_path = os.path.join(self.script_dir, file)
                print(f"JAR de Audiveris encontrado: {jar_path}")
                return jar_path
        
        # Buscar en carpeta de descarga si existe
        download_dir = os.path.join(self.script_dir, "Audiveris_downloaded")
        if os.path.exists(download_dir):
            for root, dirs, files in os.walk(download_dir):
                for file in files:
                    if file.endswith(".jar") and "Audiveris" in file:
                        jar_path = os.path.join(root, file)
                        print(f"JAR de Audiveris encontrado en carpeta descargada: {jar_path}")
                        return jar_path
            
        # Si no encontramos, intentar descargar la última versión
        print("JAR de Audiveris no encontrado, intentando descargar...")
        if self.descargar_audiveris():
            # Volver a buscar después de descargar
            return self.encontrar_audiveris_jar()
        
        return None


    def descargar_audiveris(self):
        """
        Descarga la última versión de Audiveris desde GitHub.
        """
        try:
            import requests
            from zipfile import ZipFile
            from io import BytesIO
            
            print("Descargando última versión de Audiveris desde GitHub...")
            
            # URL de la última versión
            url = "https://github.com/Audiveris/audiveris/releases/download/5.3.1/Audiveris-5.3.1-windows.zip"
            
            # Directorio de destino
            download_dir = os.path.join(self.script_dir, "Audiveris_downloaded")
            os.makedirs(download_dir, exist_ok=True)
            
            # Descargar archivo
            response = requests.get(url)
            if response.status_code == 200:
                print("Descarga completada, extrayendo archivos...")
                
                # Extraer ZIP
                with ZipFile(BytesIO(response.content)) as zipf:
                    zipf.extractall(download_dir)
                
                print(f"Audiveris descargado y extraído en: {download_dir}")
                return True
            else:
                print(f"Error al descargar Audiveris: Código {response.status_code}")
                return False
                
        except Exception as e:
            print(f"Error al descargar Audiveris: {e}")
            return False


    def ejecutar_audiveris_jar(self, java_exe, audiveris_jar):
        """
        Ejecuta Audiveris directamente desde el archivo JAR usando el ejecutable de Java.
        """
        try:
            print(f"Intentando ejecutar Audiveris desde JAR con {java_exe}")
            
            # Crear directorio temporal para los archivos de salida
            temp_output_dir = os.path.join(self.script_dir, "temp_audiveris_output")
            os.makedirs(temp_output_dir, exist_ok=True)
            
            # Preparar opciones de Java para evitar problemas de seguridad
            java_options = [
                "-Djava.security.manager=allow",
                "-Djava.security.policy=java.policy"
            ]
            
            # Crear un archivo de política permisiva
            policy_file = os.path.join(self.script_dir, "java.policy")
            with open(policy_file, "w") as f:
                f.write("grant {\n  permission java.security.AllPermission;\n};\n")
            
            # Preparar comando para ejecutar el JAR con Java
            cmd = [
                java_exe,
                *java_options,
                "-jar",
                audiveris_jar,
                "-batch",  # Modo batch (no interactivo)
                "-export",  # Exportar los resultados
                "-output", 
                temp_output_dir,  # Directorio de salida específico
                "-option", "org.audiveris.omr.sheet.BookManager.useSeparateBookFolders=false",
                "-option", "org.audiveris.omr.export.ScoreExporter.musicxmlUseCompression=true",
                self.archivo_pdf  # Archivo PDF a procesar
            ]
            
            print(f"Comando: {' '.join(cmd)}")
            
            # Ejecutar el comando
            result = subprocess.run(cmd, capture_output=True, text=True)
            
            print(f"Código de salida: {result.returncode}")
            if result.stdout:
                print(f"Salida estándar: {result.stdout[:500]}...")
            if result.stderr:
                print(f"Salida de error: {result.stderr[:500]}...")
            
            if result.returncode != 0:
                print("Error al ejecutar Audiveris desde JAR")
                return False
            
            # Buscar el archivo MXL generado en el directorio temporal
            mxl_files = []
            for file in os.listdir(temp_output_dir):
                if file.endswith(".mxl"):
                    mxl_files.append(os.path.join(temp_output_dir, file))
            
            if mxl_files:
                # Copiar el primer archivo MXL encontrado al destino final
                shutil.copy2(mxl_files[0], self.archivo_mxl)
                print(f"Archivo MXL encontrado y copiado a: {self.archivo_mxl}")
                
                # Eliminar el directorio temporal
                shutil.rmtree(temp_output_dir)
                
                print("Conversión de PDF a MusicXML completada correctamente usando JAR.\n\n")
                return True
            else:
                print("No se encontraron archivos MXL generados por el JAR.")
                return False
                
        except Exception as e:
            print(f"Error al ejecutar JAR: {e}")
            traceback.print_exc()
            return False


    def intentar_herramienta_externa(self):
        """
        Intenta usar una versión independiente de Audiveris como herramienta externa.
        """
        try:
            print("Intentando ejecutar Audiveris como herramienta independiente...")
            
            # Ubicaciones posibles de Audiveris instalado
            audiveris_locations = [
                "C:\\Program Files\\Audiveris\\bin\\Audiveris.bat",
                "C:\\Program Files (x86)\\Audiveris\\bin\\Audiveris.bat",
                os.path.join(os.environ.get('LOCALAPPDATA', ''), "Programs", "Audiveris", "bin", "Audiveris.bat")
            ]
            
            # Buscar ejecutable
            audiveris_cmd = None
            for loc in audiveris_locations:
                if os.path.exists(loc):
                    audiveris_cmd = loc
                    print(f"Encontrado Audiveris instalado en: {loc}")
                    break
                    
            if not audiveris_cmd:
                print("No se encontró una instalación independiente de Audiveris")
                return False
                
            # Crear directorio temporal para los archivos de salida
            temp_output_dir = os.path.join(self.script_dir, "temp_audiveris_output")
            os.makedirs(temp_output_dir, exist_ok=True)
            
            # Preparar comando
            cmd = [
                audiveris_cmd,
                "-batch",
                "-export",
                "-output", temp_output_dir,
                "-option", "org.audiveris.omr.sheet.BookManager.useSeparateBookFolders=false",
                "-option", "org.audiveris.omr.export.ScoreExporter.musicxmlUseCompression=true",
                self.archivo_pdf
            ]
            
            print(f"Comando: {' '.join(cmd)}")
            
            # Ejecutar el comando
            result = subprocess.run(cmd, capture_output=True, text=True)
            
            if result.returncode == 0:
                # Buscar archivo MXL
                for file in os.listdir(temp_output_dir):
                    if file.endswith(".mxl"):
                        mxl_path = os.path.join(temp_output_dir, file)
                        shutil.copy2(mxl_path, self.archivo_mxl)
                        print(f"Archivo MXL copiado a: {self.archivo_mxl}")
                        shutil.rmtree(temp_output_dir)
                        return True
                        
            print(f"Error al ejecutar herramienta externa. Código: {result.returncode}")
            return False
            
        except Exception as e:
            print(f"Error con herramienta externa: {e}")
            return False


    def setup_audiveris_runtime(self, java_home):
        """
        Configura el directorio runtime de Audiveris con archivos de Java necesarios.
        Copia los archivos esenciales de JRE a la ubicación esperada por Audiveris.
        """
        audiveris_exe = self.encontrar_audiveris_exe()
        if not audiveris_exe:
            print("No se puede configurar runtime: No se encontró Audiveris.exe")
            return False
            
        try:
            # Determinar la ruta de Audiveris
            audiveris_dir = os.path.dirname(audiveris_exe)
            runtime_dir = os.path.join(audiveris_dir, "runtime")
            
            # Limpiar directorio runtime si existe para evitar problemas de archivos incompletos
            if os.path.exists(runtime_dir):
                print(f"Limpiando directorio runtime existente: {runtime_dir}")
                shutil.rmtree(runtime_dir)
                
            # Crear directorio runtime desde cero
            os.makedirs(runtime_dir, exist_ok=True)
            print(f"Creado directorio runtime en: {runtime_dir}")
                
            # Crear estructura completa de directorios Java necesarios
            os.makedirs(os.path.join(runtime_dir, "bin"), exist_ok=True)
            os.makedirs(os.path.join(runtime_dir, "bin", "server"), exist_ok=True)
            os.makedirs(os.path.join(runtime_dir, "lib"), exist_ok=True)
            os.makedirs(os.path.join(runtime_dir, "lib", "security"), exist_ok=True)
            os.makedirs(os.path.join(runtime_dir, "conf"), exist_ok=True)
            
            # MÉTODO 1: Copiar archivos individuales más importantes
            
            # Copiar ejecutables esenciales
            java_bin = os.path.join(java_home, "bin")
            essential_exes = ["java.exe", "javaw.exe"]
            
            for exe in essential_exes:
                src_path = os.path.join(java_bin, exe)
                if os.path.exists(src_path):
                    dst_path = os.path.join(runtime_dir, "bin", exe)
                    shutil.copy2(src_path, dst_path)
                    print(f"Copiado: {exe}")
            
            # Copiar jvm.dll a directorio bin/server
            jvm_dll_src = os.path.join(java_bin, "server", "jvm.dll")
            if os.path.exists(jvm_dll_src):
                # Copiar a bin/server
                jvm_dll_dst_server = os.path.join(runtime_dir, "bin", "server", "jvm.dll")
                shutil.copy2(jvm_dll_src, jvm_dll_dst_server)
                print(f"Copiado: bin/server/jvm.dll")
                
            # Copiar DLLs adicionales necesarias
            for file in os.listdir(java_bin):
                if file.endswith(".dll"):
                    src_path = os.path.join(java_bin, file)
                    dst_path = os.path.join(runtime_dir, "bin", file)
                    shutil.copy2(src_path, dst_path)
                    print(f"Copiado: {file}")
            
            # Buscar y copiar jvm.cfg - ARCHIVO CRÍTICO QUE FALTABA
            jvm_cfg_paths = [
                os.path.join(java_home, "lib", "jvm.cfg"),
                os.path.join(java_home, "jre", "lib", "jvm.cfg"),
                os.path.join(java_home, "conf", "jvm.cfg")
            ]
            
            jvm_cfg_found = False
            for cfg_path in jvm_cfg_paths:
                if os.path.exists(cfg_path):
                    dst_cfg = os.path.join(runtime_dir, "lib", "jvm.cfg")
                    shutil.copy2(cfg_path, dst_cfg)
                    print(f"Copiado: lib/jvm.cfg desde {cfg_path}")
                    jvm_cfg_found = True
                    break
            
            # Si no encontramos jvm.cfg, intentamos crear uno básico
            if not jvm_cfg_found:
                print("No se encontró jvm.cfg, creando uno básico...")
                with open(os.path.join(runtime_dir, "lib", "jvm.cfg"), "w") as f:
                    f.write("-server KNOWN\n")
                    f.write("-client IGNORE\n")
                print("Creado archivo jvm.cfg básico")
            
            # MÉTODO 2: Copiar directorios completos
            # Esto nos asegura que tengamos todos los archivos necesarios
            
            # Copiar archivos de seguridad (security) - CRÍTICO para solucionar el error actual
            java_security_dir = os.path.join(java_home, "lib", "security")
            if os.path.exists(java_security_dir):
                security_dst = os.path.join(runtime_dir, "lib", "security")
                
                # Asegurarnos de copiar java.security y otros archivos importantes
                security_files = [
                    "java.security", "blacklisted.certs", "cacerts", 
                    "default.policy", "public_suffix_list.dat"
                ]
                
                for file in os.listdir(java_security_dir):
                    src_path = os.path.join(java_security_dir, file)
                    dst_path = os.path.join(security_dst, file)
                    if os.path.isfile(src_path):
                        shutil.copy2(src_path, dst_path)
                        print(f"Copiado archivo de seguridad: {file}")
                    elif os.path.isdir(src_path):
                        security_subdir = os.path.join(security_dst, file)
                        os.makedirs(security_subdir, exist_ok=True)
                        for subfile in os.listdir(src_path):
                            sub_src = os.path.join(src_path, subfile)
                            sub_dst = os.path.join(security_subdir, subfile)
                            if os.path.isfile(sub_src):
                                shutil.copy2(sub_src, sub_dst)
                                print(f"Copiado archivo de seguridad: {file}/{subfile}")
            else:
                print("ADVERTENCIA: No se encontró directorio de seguridad en Java")
                
                # Intentar buscar en otras ubicaciones
                alt_security_paths = [
                    os.path.join(java_home, "jre", "lib", "security"),
                    os.path.join(java_home, "conf", "security")
                ]
                
                for alt_path in alt_security_paths:
                    if os.path.exists(alt_path):
                        print(f"Encontrado directorio de seguridad alternativo: {alt_path}")
                        security_dst = os.path.join(runtime_dir, "lib", "security")
                        
                        for file in os.listdir(alt_path):
                            src_path = os.path.join(alt_path, file)
                            dst_path = os.path.join(security_dst, file)
                            if os.path.isfile(src_path):
                                shutil.copy2(src_path, dst_path)
                                print(f"Copiado archivo de seguridad alternativo: {file}")
                        break
            
            # Copiar el contenido completo del directorio lib
            java_lib = os.path.join(java_home, "lib")
            if os.path.exists(java_lib):
                # Copiar directamente los archivos de lib (no sobreescribir los ya copiados)
                for item in os.listdir(java_lib):
                    src_item = os.path.join(java_lib, item)
                    dst_item = os.path.join(runtime_dir, "lib", item)
                    
                    if os.path.isfile(src_item) and not os.path.exists(dst_item):
                        shutil.copy2(src_item, dst_item)
                        print(f"Copiado: lib/{item}")
                    elif os.path.isdir(src_item) and item != "security" and not os.path.exists(dst_item):
                        # No volver a copiar security que ya lo hicimos arriba
                        shutil.copytree(src_item, dst_item)
                        print(f"Copiado directorio: lib/{item}/")
            
            # Copiar el contenido de conf
            java_conf = os.path.join(java_home, "conf")
            if os.path.exists(java_conf):
                # Copiar archivos de configuración
                for item in os.listdir(java_conf):
                    src_item = os.path.join(java_conf, item)
                    dst_item = os.path.join(runtime_dir, "conf", item)
                    
                    if os.path.isfile(src_item):
                        shutil.copy2(src_item, dst_item)
                        print(f"Copiado: conf/{item}")
                    elif os.path.isdir(src_item):
                        shutil.copytree(src_item, dst_item, dirs_exist_ok=True)
                        print(f"Copiado directorio: conf/{item}/")
            
            # Asegurarnos de que existan rutas de módulos específicos para Java 9+
            modules_dir = os.path.join(java_home, "jmods")
            if os.path.exists(modules_dir):
                dst_modules = os.path.join(runtime_dir, "jmods")
                os.makedirs(dst_modules, exist_ok=True)
                for file in os.listdir(modules_dir):
                    if file.startswith("java.base") or file.startswith("java.security"):
                        src_path = os.path.join(modules_dir, file)
                        dst_path = os.path.join(dst_modules, file)
                        shutil.copy2(src_path, dst_path)
                        print(f"Copiado módulo: {file}")
            
            # Crear archivo release con la información del JRE
            with open(os.path.join(runtime_dir, "release"), "w") as f:
                f.write(f"JAVA_VERSION=\"{os.environ.get('JAVA_HOME_VERSION', '21')}\"\n")
                f.write(f"OS_NAME=\"Windows\"\n")
                f.write(f"OS_ARCH=\"{platform.machine()}\"\n")
            
            print("Configuración de runtime completada con éxito.")
            return True
            
        except Exception as e:
            print(f"Error al configurar runtime: {e}")
            traceback.print_exc()
            return False    


    def convertir_pdf_a_musicxml(self):
        """
        Convierte un archivo PDF a MusicXML utilizando Audiveris.
        Intenta múltiples métodos para ejecutar Audiveris.
        """
        print(f"Convirtiendo el archivo PDF ({self.archivo_pdf}) a MusicXML.")
        
        # 1. Detectar Java
        java_home = self.detectar_java()
        if not java_home:
            print("No se pudo encontrar Java. Generando archivo MXL provisional.")
            self.generar_mxl_provisional()
            return
        
        # 2. Obtener el ejecutable de Java según el sistema
        java_exe = self.get_java_executable(java_home)
        if not os.path.exists(java_exe):
            print(f"Error: No se encuentra el ejecutable de Java en {java_exe}")
            self.generar_mxl_provisional()
            return
        print(f"Java encontrado en: {java_exe}")
        
        # Variable para controlar si algún método tuvo éxito
        conversion_exitosa = False
        
        # 3. MÉTODO 1: Intentar con herramienta externa independiente
        print("\nMÉTODO 1: Intentando ejecutar Audiveris como herramienta externa independiente...")
        if self.intentar_herramienta_externa():
            print("Conversión exitosa usando Audiveris independiente.")
            return
        
        # 4. MÉTODO 2: Intentar con Audiveris JAR directamente
        print("\nMÉTODO 2: Intentando ejecutar Audiveris directamente desde JAR...")
        audiveris_jar = self.encontrar_audiveris_jar()
        
        if audiveris_jar and java_exe:
            jar_success = self.ejecutar_audiveris_jar(java_exe, audiveris_jar)
            
            if jar_success:
                print("Conversión exitosa mediante JAR de Audiveris.")
                return
        
        # 5. MÉTODO 3: Intentar con Audiveris EXE y runtime personalizado
        print("\nMÉTODO 3: Intentando ejecutar Audiveris desde EXE con runtime personalizado...")
        audiveris_exe = self.encontrar_audiveris_exe()
        
        if audiveris_exe:
            # Configurar runtime de Audiveris con Java
            runtime_setup = self.setup_audiveris_runtime(java_home)
            
            if runtime_setup:
                try:
                    # Crear directorio temporal para los archivos de salida
                    temp_output_dir = os.path.join(self.script_dir, "temp_audiveris_output")
                    os.makedirs(temp_output_dir, exist_ok=True)
                    
                    # Preparar comando para ejecutar el exe
                    cmd = [
                        audiveris_exe,
                        "-batch",  # Modo batch (no interactivo)
                        "-export",  # Exportar los resultados
                        "-output", 
                        temp_output_dir,  # Directorio de salida específico
                        "-option", "org.audiveris.omr.sheet.BookManager.useSeparateBookFolders=false",
                        "-option", "org.audiveris.omr.export.ScoreExporter.musicxmlUseCompression=true",
                        self.archivo_pdf  # Archivo PDF a procesar
                    ]
                    
                    print(f"Comando: {' '.join(cmd)}")
                    
                    # Ejecutar el comando
                    result = subprocess.run(cmd, capture_output=True, text=True)
                    
                    print(f"Código de salida: {result.returncode}")
                    if result.stdout:
                        print(f"Salida estándar: {result.stdout[:500]}...")
                    if result.stderr:
                        print(f"Salida de error: {result.stderr[:500]}...")
                    
                    if result.returncode != 0:
                        print("Error al ejecutar Audiveris EXE, se intentará con otro método")
                    else:
                        # Buscar el archivo MXL generado en el directorio temporal
                        mxl_files = []
                        for file in os.listdir(temp_output_dir):
                            if file.endswith(".mxl"):
                                mxl_files.append(os.path.join(temp_output_dir, file))
                        
                        if mxl_files:
                            # Copiar el primer archivo MXL encontrado al destino final
                            shutil.copy2(mxl_files[0], self.archivo_mxl)
                            print(f"Archivo MXL encontrado y copiado a: {self.archivo_mxl}")
                            conversion_exitosa = True
                            
                        else:
                            print("No se encontraron archivos MXL generados con el EXE.")
                    
                    # Limpiar directorio temporal
                    if os.path.exists(temp_output_dir):
                        shutil.rmtree(temp_output_dir)
                    
                    if conversion_exitosa:
                        print("Conversión de PDF a MusicXML completada correctamente con el EXE.\n\n")
                        return
                        
                except Exception as e:
                    print(f"Error en MÉTODO 3: {e}")
                    traceback.print_exc()
        else:
            print("No se encontró el EXE de Audiveris para el MÉTODO 3.")
        
        # 6. MÉTODO 4: Alternativas (ImageMagick u otras herramientas)
        if not conversion_exitosa:
            print("\nMÉTODO 4: Intentando procesar con métodos alternativos...")
            try:
                # Verificar si ImageMagick está disponible
                imagemagick_available = False
                try:
                    # Probar comando 'magick' (más nuevo) o 'convert' (más antiguo)
                    for cmd in ["magick", "convert"]:
                        check_cmd = [cmd, "--version"] if cmd == "magick" else [cmd, "-version"]
                        img_check = subprocess.run(check_cmd, capture_output=True, text=True)
                        if img_check.returncode == 0:
                            imagemagick_available = True
                            imagemagick_cmd = cmd
                            print(f"ImageMagick detectado ({cmd})")
                            break
                except:
                    print("ImageMagick no detectado")
                
                if imagemagick_available:
                    img_output_dir = os.path.join(self.script_dir, "temp_images")
                    os.makedirs(img_output_dir, exist_ok=True)
                    
                    # Usar el comando detectado
                    if imagemagick_cmd == "magick":
                        convert_cmd = [
                            "magick", "convert", 
                            "-density", "300", 
                            self.archivo_pdf, 
                            os.path.join(img_output_dir, "page-%03d.png")
                        ]
                    else:  # Comando antiguo 'convert'
                        convert_cmd = [
                            "convert", 
                            "-density", "300", 
                            self.archivo_pdf, 
                            os.path.join(img_output_dir, "page-%03d.png")
                        ]
                    
                    convert_result = subprocess.run(convert_cmd, capture_output=True, text=True)
                    
                    if convert_result.returncode == 0:
                        print("PDF convertido a imágenes exitosamente")
                        image_files = [f for f in os.listdir(img_output_dir) if f.endswith('.png')]
                        print(f"Imágenes generadas: {len(image_files)}")
                        print("ADVERTENCIA: Se requiere procesamiento manual de las imágenes")
                        print(f"Las imágenes están en: {img_output_dir}")
                        
                        # Intentar usar OCR o herramientas específicas para música aquí
                        # Por ahora, continuamos con MXL provisional
                else:
                    print("No se detectó ImageMagick para convertir el PDF a imágenes")
                    
            except Exception as e:
                print(f"Error en método alternativo: {e}")
                traceback.print_exc()
        
        # 7. Si todos los métodos fallan, generar un MXL provisional
        if not os.path.exists(self.archivo_mxl):
            print("\nTodos los métodos fallaron. Generando archivo MXL provisional...")
            self.generar_mxl_provisional()
    

    def convertir_con_metodo_alternativo(self):
        """Intenta convertir el PDF a imágenes como método alternativo."""
        print("\nIntentando procesar PDF con herramientas alternativas...")
        
        try:
            # Verificar si ImageMagick está disponible
            imagemagick_available = False
            try:
                # Probar comando 'magick' (más nuevo) o 'convert' (más antiguo)
                for cmd in ["magick", "convert"]:
                    check_cmd = [cmd, "--version"] if cmd == "magick" else [cmd, "-version"]
                    img_check = subprocess.run(check_cmd, capture_output=True, text=True)
                    if img_check.returncode == 0:
                        imagemagick_available = True
                        imagemagick_cmd = cmd
                        print(f"ImageMagick detectado ({cmd})")
                        break
            except:
                print("ImageMagick no detectado")
            
            if imagemagick_available:
                img_output_dir = os.path.join(self.script_dir, "temp_images")
                os.makedirs(img_output_dir, exist_ok=True)
                
                # Usar el comando detectado
                if imagemagick_cmd == "magick":
                    convert_cmd = [
                        "magick", "convert", 
                        "-density", "300", 
                        self.archivo_pdf, 
                        os.path.join(img_output_dir, "page-%03d.png")
                    ]
                else:  # Comando antiguo 'convert'
                    convert_cmd = [
                        "convert", 
                        "-density", "300", 
                        self.archivo_pdf, 
                        os.path.join(img_output_dir, "page-%03d.png")
                    ]
                
                convert_result = subprocess.run(convert_cmd, capture_output=True, text=True)
                
                if convert_result.returncode == 0:
                    print("PDF convertido a imágenes exitosamente")
                    image_files = [f for f in os.listdir(img_output_dir) if f.endswith('.png')]
                    print(f"Imágenes generadas: {len(image_files)}")
                    print("ADVERTENCIA: Se requiere procesamiento manual de las imágenes")
                    print(f"Las imágenes están en: {img_output_dir}")
            else:
                print("No se detectó ImageMagick para convertir el PDF a imágenes")
                
        except Exception as e:
            print(f"Error en método alternativo: {e}")
            traceback.print_exc()
        
        # Si todos los métodos fallan, generar un archivo MXL provisional
        print("\nGenerando archivo MXL provisional...")
        self.generar_mxl_provisional()


    def generar_mxl_provisional(self):
        """Genera un archivo MXL básico para continuar el flujo cuando fallan otros métodos."""
        # Crear un stream de música básico con una nota
        s = stream.Stream()
        s.append(metadata.Metadata())
        s.metadata.title = self.nombre_partitura
        s.metadata.composer = self.nombre_autor
        
        # Añadir una nota como marcador
        n = note.Note('C4')
        n.quarterLength = 4.0
        s.append(n)
        
        # Guardar como MXL
        s.write('musicxml', fp=self.archivo_mxl)
        print(f"Archivo MXL provisional generado en: {self.archivo_mxl}")
        
        return self.archivo_mxl


    def musicxml_a_json(self, guardar_txt=True):
        """
        Convierte un archivo MusicXML a JSON y lo guarda en la base de datos.
        Si guardar_txt es True, guarda también los archivos .json y .txt.
        """
        print(f"Convirtiendo el archivo MusicXML ({self.archivo_mxl}) a texto.")

        try:
            partitura = converter.parse(self.archivo_mxl)
            
            notas = []

            # Recorrer notas y acordes
            for n in partitura.flatten().recurse().notes:
                # Si es un acorde, procesar cada nota
                if isinstance(n, chord.Chord):
                    for note_in_chord in n.pitches:
                        notas.append({
                            "Nota": note_in_chord.name,
                            "Octava": note_in_chord.octave,
                            "Offset": float(n.offset),
                            "Duracion": float(n.quarterLength),
                        })
                
                # Si es una nota individual
                elif isinstance(n, note.Note):
                    notas.append({
                        "Nota": n.name,
                        "Octava": n.octave,
                        "Offset": float(n.offset),
                        "Duracion": float(n.quarterLength),
                    })
            
            # Ordenar notas por offset
            notas_ordenadas = sorted(notas, key=lambda x: x["Offset"])

            # Preparar datos para guardar
            datos = {"Notas": notas_ordenadas}
            datos_txt = json.dumps(datos)
            
            # Guardar en base de datos
            self.guardar_en_bd(datos_txt)

            # Si se solicita, guardar archivos de texto
            if guardar_txt:
                with open(self.archivo_json, "w", encoding="utf-8") as f:
                    json.dump(datos, f, indent=4, ensure_ascii=False)
            
                with open(self.archivo_txt, "w", encoding="utf-8") as f:
                    f.write(datos_txt)

                print(f"Conversión a JSON completada.\nArchivo JSON guardado en: {self.archivo_json}")
                print(f"Archivo txt guardado en: {self.archivo_txt} \n\n")
                
        except Exception as e:
            print(f"Error al convertir MusicXML a JSON: {e}")
            traceback.print_exc()


    def guardar_en_bd(self, datos_txt):
        """Guarda los datos de la partitura en la base de datos SQLite."""
        try:
            # Verificar que existe la base de datos, crearla si no
            self.verificar_bd()
            
            # Conectar a la base de datos
            conexion = sqlite3.connect("canciones.db")
            cursor = conexion.cursor()

            # Verificar si ya existe una canción con el mismo título y autor
            cursor.execute("SELECT id FROM Partituras WHERE titulo = ? AND autor = ?", 
                          (self.nombre_partitura, self.nombre_autor))
            resultado = cursor.fetchone()

            if resultado:
                print(f"La canción '{self.nombre_partitura}' de '{self.nombre_autor}' ya existe en la base de datos.")
            else:
                # Insertar la nueva cancion
                cursor.execute("INSERT INTO Partituras (titulo, autor, bpm, contenido) VALUES (?, ?, ?, ?)", 
                              (self.nombre_partitura, self.nombre_autor, self.bpm, datos_txt))
                
                # Guardar los cambios
                conexion.commit()
                print(f"Canción '{self.nombre_partitura}' de '{self.nombre_autor}' guardada exitosamente.")

            # Cerrar la conexión
            conexion.close()
            print("Datos guardados en la base de datos.\n\n")
            
        except Exception as e:
            print(f"Error al guardar en base de datos: {e}")
            traceback.print_exc()


    def verificar_bd(self):
        """Verifica que la base de datos existe y tiene la estructura correcta."""
        try:
            conexion = sqlite3.connect("canciones.db")
            cursor = conexion.cursor()

            # Verificar si la tabla existe
            cursor.execute("SELECT name FROM sqlite_master WHERE type='table' AND name='Partituras'")
            if not cursor.fetchone():
                print("Creando tabla Partituras en la base de datos...")
                cursor.execute('''
                CREATE TABLE Partituras (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    titulo TEXT NOT NULL,
                    autor TEXT NOT NULL,
                    bpm INTEGER NOT NULL,
                    contenido TEXT NOT NULL,
                    fecha_creacion TIMESTAMP DEFAULT CURRENT_TIMESTAMP
                )
                ''')
                conexion.commit()
                print("Tabla Partituras creada correctamente.")
            
            conexion.close()
        except Exception as e:
            print(f"Error al verificar/crear la base de datos: {e}")
            traceback.print_exc()


    def eliminar_temporales(self):
        """Elimina archivos temporales generados durante la conversión."""
        print("Eliminando archivos temporales...")
        
        # Listar extensiones a eliminar
        extensiones = [".omr", ".log"]
        
        # Directorio de partituras
        partituras_dir = self.direccion_json
        
        # Eliminar por extensión
        for extension in extensiones:
            try:
                for archivo in os.listdir(partituras_dir):
                    if archivo.endswith(extension):
                        file_path = os.path.join(partituras_dir, archivo)
                        os.remove(file_path)
                        print(f"Archivo eliminado: {file_path}")
            except Exception as e:
                print(f"Error al eliminar archivos {extension}: {e}")
        
        # Eliminar MXL (excepto el actual)
        try:
            archivo_mxl_base = os.path.basename(self.archivo_mxl)
            
            for archivo in os.listdir(partituras_dir):
                if archivo.endswith(".mxl") and archivo != archivo_mxl_base:
                    file_path = os.path.join(partituras_dir, archivo)
                    os.remove(file_path)
                    print(f"Archivo eliminado: {file_path}")
        except Exception as e:
            print(f"Error al eliminar archivos MXL: {e}")
        
        # Eliminar __pycache__
        try:
            pycache_path = os.path.join(self.script_dir, "__pycache__")
            if os.path.isdir(pycache_path):
                shutil.rmtree(pycache_path)
                print(f"Carpeta __pycache__ eliminada: {pycache_path}")
        except Exception as e:
            print(f"Error al eliminar __pycache__: {e}")
            
        # Eliminar directorio temp_images si existe
        try:
            temp_images = os.path.join(self.script_dir, "temp_images")
            if os.path.isdir(temp_images):
                shutil.rmtree(temp_images)
                print(f"Carpeta temp_images eliminada: {temp_images}")
        except Exception as e:
            print(f"Error al eliminar temp_images: {e}")
            
        # Eliminar directorio temp_audiveris_output si existe
        try:
            temp_audiveris = os.path.join(self.script_dir, "temp_audiveris_output")
            if os.path.isdir(temp_audiveris):
                shutil.rmtree(temp_audiveris)
                print(f"Carpeta temp_audiveris_output eliminada: {temp_audiveris}")
        except Exception as e:
            print(f"Error al eliminar temp_audiveris_output: {e}")
            
        print("Limpieza de archivos temporales completada.")