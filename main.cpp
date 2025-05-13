#include "MainWindow.h"
//#include "TeclaCayendo.h"          // Importamos la clase Tecla para poder probarla
#include <QApplication>
#include <QDebug>            // Importamos QDebug para poder hacer pruebas por consola
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlDebuggingEnabler>


/*
// 🔥 Función de prueba para crear objetos Tecla y probar sus valores
void testTeclas() {
    // Creamos dos objetos Tecla con valores arbitrarios
    Tecla t1("C4", 10.0f, 1.0f, 50.0f, 0.0f, 0.0f);   // Tecla C4, longitud 10.0, duración 1.0, posición en X=50
    Tecla t2("D4", 10.0f, 1.2f, 100.0f, 0.0f, 0.0f);  // Tecla D4, longitud 10.0, duración 1.2, posición en X=100

    // Usamos QDebug para imprimir por consola el código de las teclas creadas
    qDebug() << "Código tecla 1:" << t1.getCodigo();   // Debería mostrar "C4"
    qDebug() << "Código tecla 2:" << t2.getCodigo();   // Debería mostrar "D4"
}
*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv); // Inicializa la aplicación Qt

    // Llamada a la función de prueba que creará las teclas
    //testTeclas();  // Esta línea llama a la función que ejecuta las pruebas de la clase Tecla

    // Creamos la ventana principal de la aplicación, la cual está definida en `mainwindow.ui` (Qt Designer)
    MainWindow w;

    w.show();  // Muestra la ventana principal en la pantalla

    // El bucle de eventos Qt arranca, y la aplicación permanece ejecutándose hasta que el usuario la cierre
    return a.exec();   // Esto mantiene la aplicación viva y escucha los eventos de la interfaz
}
