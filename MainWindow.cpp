#include "MainWindow.h"
#include "Teclado.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>

void MainWindow::leerNotasDesdeJson(const QString& ruta) {
    QFile archivo(ruta);
    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("No se pudo abrir el archivo JSON.");
        return;
    }

    QByteArray datos = archivo.readAll();
    archivo.close();

    QJsonDocument doc = QJsonDocument::fromJson(datos);
    if (!doc.isObject()) {
        qWarning("El JSON no es un objeto.");
        return;
    }

    QJsonObject rootObj = doc.object();
    QJsonArray notasArray = rootObj["Notas"].toArray();

    for (const QJsonValue& valor : notasArray) {
        QJsonObject obj = valor.toObject();
        QString nota = obj["Nota"].toString();
        int octava = obj["Octava"].toInt();
        float offset = static_cast<float>(obj["Offset"].toDouble());
        float duracion = static_cast<float>(obj["Duracion"].toDouble());

        QString codigo = nota + QString::number(octava);
        int delayMs = static_cast<int>(offset * 1000);

        QTimer::singleShot(delayMs, this, [=]() {
            // Obtener la posición X de la tecla desde el método de la clase Teclado
            float x = teclado->obtenerXdeCodigo(codigo);
            if (x < 0) return; // No se encontró la tecla

            // Buscar la tecla correspondiente
            Tecla* teclaReal = nullptr;
            for (Tecla* t : teclado->getTeclas()) {
                if (t->getOctava() == octava && t->getNombres().contains(nota)) {
                    teclaReal = t;
                    break;
                }
            }

            if (!teclaReal) return;

            // Medidas reales de la tecla
            qreal anchoOriginal = teclaReal->boundingRect().width(); // más fiable
            qreal alto = teclaReal->boundingRect().height();

            // DEBUG: Información de la tecla y su posicionamiento
            qDebug() << "Nota:" << nota << "Octava:" << octava << "X real:" << x
                     << "Ancho original:" << anchoOriginal << "Alto:" << alto;

            // Reducimos el ancho de la nota
            qreal anchoReducido = anchoOriginal * 0.5; // puedes ajustar este valor si es necesario
            // Corregir la posición para que se centre correctamente (ajustamos un poco el desplazamiento si está a la derecha)
            qreal xCentrada = x + (anchoOriginal - anchoReducido) / 2;

            // DEBUG: Ver en la consola cómo se alinea
            qDebug() << "X centrada corregida:" << xCentrada;

            // Crear rectángulo de nota cayendo con el ancho reducido
            QGraphicsRectItem* notaCayendo = new QGraphicsRectItem(0, 0, anchoReducido, alto);
            notaCayendo->setBrush(Qt::blue);
            // Usamos xCentrada para asegurarnos que la tecla se alinee correctamente
            notaCayendo->setPos(xCentrada, -alto);
            scene->addItem(notaCayendo);

            QTimer* timer = new QTimer(this);
            connect(timer, &QTimer::timeout, [=]() {
                notaCayendo->moveBy(0, 5);

                QList<QGraphicsItem*> colisiones = notaCayendo->collidingItems();
                for (QGraphicsItem* item : colisiones) {
                    Tecla* t = dynamic_cast<Tecla*>(item);
                    if (t) {
                        t->iluminar();
                    }
                }

                if (notaCayendo->pos().y() > alturaPantalla) {
                    timer->stop();
                    delete notaCayendo;
                }
            });
            timer->start(50);
        });
    }
}


MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
    // Crear la escena
    scene = new QGraphicsScene(this);

    // Crear la vista y configurarla
    view = new QGraphicsView(scene, this);
    setCentralWidget(view);
    view->setRenderHint(QPainter::Antialiasing); // Mejorar la calidad del renderizado

    // Hacer que la ventana se abra en pantalla completa
    showFullScreen();

    // Deshabilitar el scroll
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Obtener las dimensiones de la pantalla
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    anchuraPantalla = screenGeometry.width();
    alturaPantalla = screenGeometry.height();

    // Crear el teclado y añadir sus teclas a la escena
    teclado = new Teclado(scene, anchuraPantalla, alturaPantalla);

    // Simulación de teclas cayendo
    /*crearNotaCayendo(0, false);
    crearNotaCayendo(34, false);
    crearNotaCayendo(68, false);*/
    leerNotasDesdeJson(":/1stGnossienne_EricSatie.json");
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
    teclaCayendo->setBrush(Qt::white);
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

