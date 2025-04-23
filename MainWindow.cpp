#include "MainWindow.h"

// Carga un archivo JSON con notas musicales y programa su aparición en pantalla en el momento indicado
void MainWindow::leerNotasDesdeJson(const QString& ruta) {
    QFile archivo(ruta);

    // Abre el archivo en modo solo lectura
    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("No se pudo abrir el archivo JSON.");
        return;
    }

    // Lee el contenido completo del archivo y lo cierra
    QByteArray datos = archivo.readAll();
    archivo.close();

    // Intenta interpretar los datos como un documento JSON
    QJsonDocument doc = QJsonDocument::fromJson(datos);
    if (!doc.isObject()) {
        qWarning("El JSON no es un objeto.");
        return;
    }

    // Extrae el array de notas del JSON
    QJsonArray notasArray = doc.object()["Notas"].toArray();

    // Itera sobre cada nota
    for (const QJsonValue& valor : notasArray) {
        QJsonObject obj = valor.toObject();
        QString nota = obj["Nota"].toString();                    // Nombre de la nota (por ej. "C", "D#", etc.)
        int octava = obj["Octava"].toInt();                       // Octava correspondiente
        float offset = static_cast<float>(obj["Offset"].toDouble()); // Momento en segundos en el que debe caer la nota
        QString codigo = nota + QString::number(octava);          // Código de nota (ej. "C4")
        int delayMs = static_cast<int>(offset * 1000);            // Se convierte a milisegundos

        // Creamos un temporizador normal
        QTimer* timer = new QTimer(this);
        timer->setSingleShot(true); // Se dispara una sola vez
        timer->setInterval(delayMs);

        // Lo agregamos a la lista de control
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
            crearNotaCayendo(rect.x(), -rect.height(), teclaReal);

            // Eliminar el temporizador una vez usado
            timersNotas.removeOne(timer);
            timer->deleteLater();
        });

        timer->start();
    }
}

// Constructor de la ventana principal
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

    // Mostrar mensaje de pausa
    QGraphicsTextItem* mensaje = scene->addText("PULSA ESPACIO PARA PONER PAUSA");
    mensaje->setDefaultTextColor(Qt::blue);
    QFont fuente("Arial", 24, QFont::Bold);
    mensaje->setFont(fuente);

    // Centrar el texto horizontalmente
    qreal x = (anchuraPantalla - mensaje->boundingRect().width()) / 2;
    qreal y = 100; // un poco desde arriba
    mensaje->setPos(x, y);

    // Guardamos el JSON que estamos usando actualmente
    jsonActual = ":/1stGnossienne_EricSatie.json";

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
void MainWindow::crearNotaCayendo(qreal posX, qreal posY, Tecla* teclaObjetivo) {
    // Tamaño igual que la tecla objetivo
    qreal anchoNota = teclaObjetivo->boundingRect().width();
    qreal alturaNota = teclaObjetivo->boundingRect().height();

    // Crea un rectángulo que representa la nota cayendo
    auto* nota = new QGraphicsRectItem(0, 0, anchoNota, alturaNota);
    nota->setBrush(Qt::yellow);              // Cambiado a amarillo para visibilidad
    nota->setPen(Qt::NoPen);                 // Sin borde
    nota->setZValue(-1);                     // Aparece detrás del teclado
    nota->setPos(posX, posY);                // Posición inicial fuera de pantalla
    scene->addItem(nota);

    // Crea un temporizador para mover la nota continuamente
    QTimer* timer = new QTimer(this);
    timersNotas.append(timer); // Guarda este temporizador de movimiento
    connect(timer, &QTimer::timeout, this, [=]() mutable {
        nota->moveBy(0, 5);  // Desplaza hacia abajo

        // Si está cerca de la tecla objetivo, se ilumina
        if (nota->y() + alturaNota > teclaObjetivo->scenePos().y()) {
            teclaObjetivo->iluminar();
        }

        // Si sale de la pantalla, se limpia y se apaga la tecla
        if (nota->y() > alturaPantalla) {
            timer->stop();
            timersNotas.removeOne(timer); // Lo quitamos al acabar
            timer->deleteLater();
            teclaObjetivo->apagar();
            scene->removeItem(nota);
            delete nota;
        }
    });

    timer->start(30);  // Velocidad de caída
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
        tecla->apagar();
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

            // Aquí es donde lanzamos las notas del JSON actual
            if (!jsonActual.isEmpty()) {
                leerNotasDesdeJson(jsonActual);
            } else {
                qWarning("jsonActual está vacío. No se puede iniciar la canción.");
            }
        }
    });

    temporizador->start(1000); // Un mensaje por segundo
}
