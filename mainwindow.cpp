#include "mainwindow.h"             // Incluimos la clase MainWindow
#include "./ui_mainwindow.h"         // Incluimos el archivo generado automáticamente por Qt Designer

// Constructor de la clase MainWindow
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)           // Llama al constructor de la clase base QMainWindow
    , ui(new Ui::MainWindow)        // Crea un objeto `ui` que manejará la interfaz gráfica
{
    ui->setupUi(this);              // Configura la interfaz de usuario (llama al código generado por Qt Designer)
}

// Destructor de la clase MainWindow
MainWindow::~MainWindow()
{
    delete ui;                      // Libera la memoria usada por el objeto `ui` cuando ya no se necesita
}
