#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Teclado.h"

#include <QMainWindow> // Incluye la clase base de Qt para crear una ventana principal
#include <QGraphicsScene>
#include <QGraphicsView>

/*
QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;  // Declaración anticipada de la clase 'MainWindow' que está en 'ui_mainwindow.h'
}
QT_END_NAMESPACE
*/

class MainWindow : public QMainWindow  // `MainWindow` hereda de `QMainWindow` de Qt
{
    Q_OBJECT  // Macro que habilita la funcionalidad de Qt Meta-Object System (para señales y slots)

    private:
        QGraphicsScene* scene;
        QGraphicsView* view;
        Teclado* teclado;
        int anchuraPantalla;
        int alturaPantalla;

        // Ui::MainWindow *ui;  // Puntero a la interfaz de usuario (UI) generada por Qt Designer

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
        void probar();
};

#endif
