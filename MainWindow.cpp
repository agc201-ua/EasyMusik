#include "MainWindow.h"

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

    // Crear el teclado y añadir sus teclas a la escena
    teclado = new Teclado(scene, anchuraPantalla, alturaPantalla);

    // Simulación de teclas cayendo
    crearNotaCayendo(0, false);
    crearNotaCayendo(34, false);
    crearNotaCayendo(68, false);
}

MainWindow::~MainWindow() {
    // Destruir objetos
    delete scene;
    delete view;
    delete teclado;
}

void MainWindow::crearNotaCayendo(qreal posX, bool esNegra) {
    qreal anchoNota = 0, alturaNota = 0;

    // Calculamos el tamaño de la tecla que cae
    if (esNegra) {
        anchoNota = teclado->getAnchuraTeclaNegra();
        alturaNota = teclado->getAlturaTeclaNegra();
    }
    else {
        anchoNota = teclado->getAnchuraTeclaBlanca();
        alturaNota = teclado->getAlturaTeclaBlanca();
    }

    // Creamos al tecla cayendo
    auto* teclaCayendo = new QGraphicsRectItem(0, 0, anchoNota, alturaNota);
    teclaCayendo->setBrush(Qt::blue);
    teclaCayendo->setPen(Qt::NoPen); // Para que pille bien el boundingRect
    teclaCayendo->setZValue(-1); // Para que no se vea por encima del teclado
    teclaCayendo->setPos(posX, 0);
    scene->addItem(teclaCayendo);

    // Estado para esta tecla
    struct EstadoTecla {
        bool desintegrando = false;
        Tecla* teclaColisionada = nullptr;
    };

    auto* estado = new EstadoTecla;

    // Iniciamos el timer
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() mutable {

        // Si aun no ha tocado el teclado
        if (!estado->desintegrando) {
            teclaCayendo->moveBy(0, 5);

            // Comprobamos si ha habido alguna colisión
            QList<QGraphicsItem*> colisiones = teclaCayendo->collidingItems();
            for (QGraphicsItem* item : colisiones) {
                Tecla* tecla = qgraphicsitem_cast<Tecla*>(item);

                // Si hay colisión, se ilumina la tecla colisionada y empieza la desintegración de la tecla cayendo
                if (tecla) {
                    tecla->iluminar();
                    estado->desintegrando = true;
                    estado->teclaColisionada = tecla;
                    break;
                }
            }
        }

        else {
            QRectF rect = teclaCayendo->rect();

            // Se va desintegrando la parte de abajo
            if (rect.height() > 2) {
                teclaCayendo->moveBy(0, 5);

                // Recortamos la nota desde abajo
                rect.setHeight(rect.height() - 5);
                teclaCayendo->setRect(rect);
            }
            // Cuando se ha desintegrado por completo
            else {
                timer->stop();
                estado->teclaColisionada->apagar();
                scene->removeItem(teclaCayendo);
                delete teclaCayendo;
                delete estado;
            }
        }
    });

    // Empieza a correr el timer
    timer->start(30);
}

