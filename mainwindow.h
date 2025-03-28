#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>  // Incluye la clase base de Qt para crear una ventana principal

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;  // Declaración anticipada de la clase `MainWindow` que está en `ui_mainwindow.h`
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow  // `MainWindow` hereda de `QMainWindow` de Qt
{
    Q_OBJECT  // Macro que habilita la funcionalidad de Qt Meta-Object System (para señales y slots)

public:
    MainWindow(QWidget *parent = nullptr);  // Constructor
    ~MainWindow();  // Destructor

private:
    Ui::MainWindow *ui;  // Puntero a la interfaz de usuario (UI) generada por Qt Designer
};

#endif // MAINWINDOW_H
