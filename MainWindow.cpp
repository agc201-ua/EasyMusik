#include "MainWindow.h"
#include "Cancion.h"
#include <QApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), cancionActual(nullptr) {
    // Configurar la ventana principal
    setWindowTitle("EasyMusik");

    // Se crea la escena
    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(QColor("#212121"));

    // Se crea y se configura la vista
    view = new QGraphicsView(scene, this);
    setCentralWidget(view);
    view->setRenderHint(QPainter::Antialiasing); // Mejorar la calidad del renderizado

    // Hacer que la ventana se abra en pantalla completa
    showFullScreen();

    // Crear el widget apilado para gestionar las distintas pantallas
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // Crear la pantalla del menú principal
    menuPrincipal = new MenuPrincipal();
    stackedWidget->addWidget(menuPrincipal);

    // Conectar la señal para mostrar una canción
    connect(menuPrincipal, &MenuPrincipal::playCancion,
            this, &MainWindow::mostrarCancion);

    // Mostrar inicialmente el menú principal
    stackedWidget->setCurrentWidget(menuPrincipal);
}

MainWindow::~MainWindow() {
    // El QStackedWidget se encarga de eliminar sus widgets hijos
}

// Cerrar la aplicación
void MainWindow::cerrarAplicacion() {
    qDebug() << "Saliendo de la app...";
    QCoreApplication::quit();
}

// Slot para mostrar la pantalla de reproducción de canción
void MainWindow::mostrarCancion(const QString &titulo, const QString &artista, const QString &jsonPath) {

    // Crear la pantalla de la canción
    cancionActual = new Cancion(this, titulo, artista, jsonPath);

    // Conectar la señal para volver al menú
    connect(cancionActual, &Cancion::volver, this, &MainWindow::volverAlMenu);

    // Agregar al widget apilado y mostrar
    stackedWidget->addWidget(cancionActual);
    stackedWidget->setCurrentWidget(cancionActual);
}

// Slot para volver al menú principal
void MainWindow::volverAlMenu() {
    // Volver a la ventana normal (no pantalla completa)
    // showNormal();

    // Si hay una canción activa, cambiar a menú y eliminar la canción
    if (cancionActual) {
        stackedWidget->setCurrentWidget(menuPrincipal);
        stackedWidget->removeWidget(cancionActual);
        delete cancionActual;
        cancionActual = nullptr;
    }
}
