@echo off
setlocal enabledelayedexpansion

REM Carpeta donde están los archivos originales
set "CARPETA_ORIGEN=C:\Users\adrie\Documents\EasyMusik\sounds"

REM Carpeta destino (puede ser la misma o una nueva)
set "CARPETA_DESTINO=%CARPETA_ORIGEN%\qt_ready"

REM Crea carpeta de destino si no existe
if not exist "%CARPETA_DESTINO%" (
    mkdir "%CARPETA_DESTINO%"
)

echo Convirtiendo archivos...

for %%F in ("%CARPETA_ORIGEN%\*.wav") do (
    set "NOMBRE=%%~nF"
    echo Procesando: %%~nxF

    ffmpeg -y -i "%%F" -acodec pcm_s16le -ac 1 -ar 44100 "%CARPETA_DESTINO%\!NOMBRE!.wav"
)

echo.
echo ✅ Conversión completada. Archivos en: %CARPETA_DESTINO%
pause
