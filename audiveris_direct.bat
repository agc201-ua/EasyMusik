@echo off
echo Ejecutando Audiveris desde C:\Users\Usuario\Documents\TAES_Proyecto\EasyMusik\audiveris_direct.bat
set JAVA_HOME=C:\Program Files\Java\jdk-24
set PATH=C:\Program Files\Java\jdk-24\bin;%PATH%
echo Variables de entorno configuradas
"C:\Users\Usuario\Documents\TAES_Proyecto\EasyMusik\Audiveris\Audiveris.exe" -batch -export "C:/Users/Usuario/Documents/TAES_Proyecto/EasyMusik/partituraspdf/StarWars.pdf"
if %ERRORLEVEL% NEQ 0 echo Error en la ejecución de Audiveris: %ERRORLEVEL%
exit /b %ERRORLEVEL%
