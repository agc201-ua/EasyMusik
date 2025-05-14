#include "MainWindow.h"
#include <QApplication>
#include <QDebug>
#include <QtQml/QQmlApplicationEngine>
#include <QtQml/QQmlDebuggingEnabler>

int main(int argc, char *argv[])
{
    // Se inicializa la aplicación Qt
    QApplication a(argc, argv);

    // Se crea la ventana principal de la aplicación
    MainWindow w;

    // Se muestra la ventana principal en la pantalla
    w.show();

    // El bucle de eventos Qt arranca, y la aplicación permanece ejecutándose hasta que el usuario la cierre
    return a.exec();   // Esto mantiene la aplicación viva y escucha los eventos de la interfaz
}
