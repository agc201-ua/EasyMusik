#include "MainWindow.h"
#include "basededatos.h"
#include <QSoundEffect>
#include <QFileInfo>
#include <QDir>

//
void MainWindow::leerPartiturasDesdeBaseDeDatos() {
    BaseDeDatos db("Canciones.db");
    QMap<int, QString> mapaIdNombre;

    if(!db.MapaDeCanciones(mapaIdNombre)) {
        qWarning("Error al sacar las canciones");
        return;
    }


    //AQUI DEBERIA DE TENER TODAS LAS CANCIONES EN UN MAPA ID NOMBRE
}


//Carga la cancion pasada en los parametros de la base de datos si esta guardada
void MainWindow::leerNotasDesdeBaseDeDatos(const QString& titulo, const QString& autor) {

    BaseDeDatos db("canciones.db"); // Constructor cual se alimenta del nombre de la base de datos

    QJsonArray notasArray;

    if(!db.ConsultaJArray(notasArray, titulo, autor)) {
        return;
    }

    for (const QJsonValue& valor : notasArray) {
        QJsonObject obj = valor.toObject();
        QString nota = obj["Nota"].toString();
        int octava = obj["Octava"].toInt();
        float offset = static_cast<float>(obj["Offset"].toDouble());
        QString codigo = nota + QString::number(octava);
        int delayMs = qRound(offset * 1000.0);

        QTimer* timer = new QTimer(this);
        timer->setSingleShot(true);
        timer->setInterval(delayMs);
        timersNotas.append(timer);

        connect(timer, &QTimer::timeout, this, [=]() {
            Tecla* teclaReal = nullptr;
            for (Tecla* t : teclado->getTeclas()) {
                if (t->getOctava() == octava && t->getNombres().contains(nota)) {
                    teclaReal = t;
                    break;
                }
            }
            if (!teclaReal) return;

            QRectF rect = teclaReal->mapRectToScene(teclaReal->boundingRect());
            qreal duracionNota = obj["Duracion"].toDouble();
            crearNotaCayendo(rect.x(), -rect.height(), teclaReal, duracionNota);

            timersNotas.removeOne(timer);
            timer->deleteLater();
        });

        timer->start();
    }
}

// Constructor de la ventana principal
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {

    //LOGICA DE MAINWINDOW.UI





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

    // Mostrar mensaje de pausa
    QGraphicsTextItem* mensaje = scene->addText("PULSA ESPACIO PARA PONER PAUSA");
    mensaje->setDefaultTextColor(Qt::blue);
    QFont fuente("Arial", 24, QFont::Bold);
    mensaje->setFont(fuente);

    // Centrar el texto horizontalmente
    qreal x = (anchuraPantalla - mensaje->boundingRect().width()) / 2;
    qreal y = 100; // un poco desde arriba
    mensaje->setPos(x, y);

    // Esperar 2 segundos antes de lanzar las notas (simula comienzo)
    QTimer::singleShot(2000, this, [=]() {
        scene->removeItem(mensaje); // Elimina el mensaje
        delete mensaje;

        // Mostrar cuenta atrás y luego lanzar las notas
        mostrarCuentaAtras();
    });
}

// Destructor
MainWindow::~MainWindow() {
    delete scene;
    delete view;
    delete teclado;
}

// Crea una nota visual que cae desde una posición concreta y colisiona con una tecla
void MainWindow::crearNotaCayendo(qreal posX, qreal posY, Tecla* teclaObjetivo, qreal duracion) {
    //Velocidad de caida de la nota
    qreal pixelesPorDesplazamiento = 5;
    qreal intervaloEnMilisegundos = 30;

    // Tamaño igual que la tecla objetivo
    qreal anchoNota = teclaObjetivo->boundingRect().width();
    qreal alturaNota = duracion * pixelesPorDesplazamiento / (intervaloEnMilisegundos / 1000);

    // Crea un rectángulo que representa la nota cayendo
    auto* nota = new QGraphicsRectItem(0, 0, anchoNota, alturaNota);
    nota->setBrush(Qt::yellow);              // Cambiado a amarillo para visibilidad
    nota->setPen(QPen(Qt::black));                 // Borde negro
    nota->setZValue(-1);                     // Aparece detrás del teclado
    nota->setPos(posX, -alturaNota);                // Posición inicial fuera de pantalla
    scene->addItem(nota);

    QSoundEffect *audio = new QSoundEffect();
    bool audioCargado = false;

    // Crea un temporizador para mover la nota continuamente
    QTimer* timer = new QTimer(this);
    timersNotas.append(timer); // Guarda este temporizador de movimiento
    connect(timer, &QTimer::timeout, this, [=]() mutable {
        nota->moveBy(0, pixelesPorDesplazamiento);  // Desplaza hacia abajo

        // Si está cerca de la tecla objetivo, se ilumina
        if (nota->y() + alturaNota > teclaObjetivo->scenePos().y()) {
            teclaObjetivo->iluminar();

            /**/
            if (!audioCargado) {
                audioCargado = true;
                audio->setSource(QUrl::fromLocalFile(teclaObjetivo->getRutaAudio()));
                audio->setLoopCount(1);
                if (teclaObjetivo->getOctava() <= 3) {
                    audio->setVolume(0.3);
                } else {
                    audio->setVolume(0.5);
                }
                audio->play();



                QTimer::singleShot(duracion * 1000, this, [=]() {
                    // Timer para desvanecer el audio
                    QTimer* fadeTimer = new QTimer(this);
                    fadeTimer->setInterval(intervaloEnMilisegundos); // 30 ms por tick
                    connect(fadeTimer, &QTimer::timeout, this, [=]() mutable {
                        qreal volActual = audio->volume();
                        qreal nuevoVol = volActual - 0.025;

                        if (nuevoVol <= 0.0) {
                            audio->stop();
                            audio->deleteLater();
                            fadeTimer->stop();
                            fadeTimer->deleteLater();
                        } else {
                            audio->setVolume(nuevoVol);
                        }
                    });
                    fadeTimer->start();
                });
            }
            /**/
        }

        // Si sale de la pantalla, se limpia y se apaga la tecla
        if (nota->y() > alturaPantalla - teclaObjetivo->getAltura()) {
            timer->stop();
            timersNotas.removeOne(timer); // Lo quitamos al acabar
            timer->deleteLater();
            teclaObjetivo->apagar();
            scene->removeItem(nota);
            delete nota;
        }
    });

    timer->start(intervaloEnMilisegundos);  // Velocidad de caída
}

// Detecta pulsaciones de teclas
void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space) {
        for (QTimer* t : timersNotas) { // Detiene el temporizador de las otas para que no se muevan
            t->stop();  // Se pueden volver a arrancar más tarde
        }
        mostrarMenuPausa(); // Muestra el menú de pausa al pulsar barra espaciadora
    }
}

// Muestra el diálogo de pausa
void MainWindow::mostrarMenuPausa() {
    PausaDialog dialog(this);

    // Conectar señales con acciones
    connect(&dialog, &PausaDialog::reiniciarClicked, this, &MainWindow::reiniciarCancion);
    connect(&dialog, &PausaDialog::salirClicked, this, &MainWindow::cerrarAplicacion);

    dialog.exec(); // Abre el diálogo y bloquea hasta cerrarse

    // Al cerrar el menú de pausa, reanuda las notas
    for (QTimer* t : timersNotas) {
        t->start();  // Esto reanuda el temporizador con su último intervalo (30 ms)
    }
}

// Reinicia la canción
void MainWindow::reiniciarCancion() {
    // Cancela los temporizadores activos
    for (QTimer* t : timersNotas) {
        t->stop();
        t->deleteLater();
    }
    timersNotas.clear();
    // Elimina todas las notas que estén en pantalla (Z=-1 indica nota cayendo)
    for (QGraphicsItem* item : scene->items()) {
        if (item->zValue() == -1) {
            scene->removeItem(item);
            delete item;
        }
    }

    // Apaga todas las teclas
    for (Tecla* tecla : teclado->getTeclas()) {
        if (tecla->getIluminada()){
            tecla->apagar();
        }
    }

    // Muestra cuenta atrás y relanza la canción
    mostrarCuentaAtras();
}

// Cierra la aplicación
void MainWindow::cerrarAplicacion() {
    qDebug() << "Saliendo de la app...";
    QCoreApplication::quit();
}

// Muestra un contador de 3, 2, 1, ¡YA! antes de lanzar la canción
void MainWindow::mostrarCuentaAtras() {
    auto* textoCuenta = new QGraphicsTextItem();
    textoCuenta->setDefaultTextColor(Qt::black);
    textoCuenta->setFont(QFont("Arial", 100, QFont::Bold));
    textoCuenta->setZValue(10);  // Encima de todo

    qreal x = anchuraPantalla / 2 - 100;
    qreal y = alturaPantalla / 2 - 100;
    textoCuenta->setPos(x, y);
    scene->addItem(textoCuenta);

    QStringList mensajes = { "3", "2", "1", "¡YA!" };
    int* indice = new int(0); // Se necesita puntero para mantener valor entre llamadas

    QTimer* temporizador = new QTimer(this);

    connect(temporizador, &QTimer::timeout, this, [=]() mutable {

        if (*indice < mensajes.size()) {
            textoCuenta->setPlainText(mensajes[*indice]);
            (*indice)++;
        } else {
            temporizador->stop();
            scene->removeItem(textoCuenta);
            delete textoCuenta;
            delete indice;
            temporizador->deleteLater();

            leerNotasDesdeBaseDeDatos("ParaElisa", "Beethoven");
        }
    });

    temporizador->start(1000); // Un mensaje por segundo
}
