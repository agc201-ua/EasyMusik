#include "MainWindow.h"             // Incluimos la clase MainWindow
//#include "./ui_mainwindow.h"         // Incluimos el archivo generado automáticamente por Qt Designer

#include <QTimer>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
    // Crear la escena
    scene = new QGraphicsScene(this);

    // Crear la vista y configurarla
    view = new QGraphicsView(scene, this);
    setCentralWidget(view);
    view->setRenderHint(QPainter::Antialiasing); // Mejorar la calidad del renderizado

    // Establecer la ventana para que ocupe toda la pantalla
    setWindowState(Qt::WindowMaximized);

    // Obtener las dimensiones de la pantalla
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    anchuraPantalla = screenGeometry.width();
    alturaPantalla = screenGeometry.height();

    // Crear el teclado y añadirlo a la escena
    teclado = new Teclado(scene, anchuraPantalla);
    scene->addItem(teclado);

    //
    probar();
}

MainWindow::~MainWindow() {
    // Destruir objetos
    delete scene;
    delete view;
    delete teclado;
}

// HACEMOS UNA PRUEBA DE Q CAE UNA TECLA PA VER SI DETECTA COLISION
void MainWindow::probar() {

    // Definir tecla cayendo
    QGraphicsRectItem* teclaCayendo = new QGraphicsRectItem(0, 0, 40, 40); // (x, y, width, height)
    teclaCayendo->setBrush(Qt::blue);  // Color de la nota
    teclaCayendo->setPos(0, -100);      // Posición inicial (puedes cambiarla según tu escena)
    scene->addItem(teclaCayendo);

    // Configurar un QTimer para mover la tecla hacia abajo cada 50 ms (o el intervalo que prefieras)
    QTimer* timer = new QTimer(this);

    connect(timer, &QTimer::timeout, [=]() {
        teclaCayendo->moveBy(0, 5);  // Mover la tecla hacia abajo

        // Detectar las colisiones con las teclas en el teclado
        QList<QGraphicsItem*> colisiones = teclaCayendo->collidingItems();
        for (QGraphicsItem* item : colisiones) {
            Tecla* tecla = dynamic_cast<Tecla*>(item);
            if (tecla) {
                tecla->iluminar();  // Ilumina la tecla si hay colisión
            }
        }

        // Si la tecla cae fuera de la pantalla, detener el timer
        if (teclaCayendo->pos().y() > alturaPantalla) {
            timer->stop();
            delete teclaCayendo;  // Eliminar la tecla cuando ya no sea visible
        }
    });

    // Iniciar el temporizador para que se mueva la tecla
    timer->start(50);  // 50 ms, ajusta el valor según lo que necesites
}

/*
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
*/


