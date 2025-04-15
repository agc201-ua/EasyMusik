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
        QString nota = obj["Nota"].toString();          // Nombre de la nota (por ej. "C", "D#", etc.)
        int octava = obj["Octava"].toInt();             // Octava correspondiente
        float offset = static_cast<float>(obj["Offset"].toDouble()); // Momento en segundos en el que debe caer la nota
        QString codigo = nota + QString::number(octava); // Código de nota (ej. "C4")
        int delayMs = static_cast<int>(offset * 1000);   // Se convierte a milisegundos

        // Programa una acción que se ejecutará después del tiempo de offset
        QTimer::singleShot(delayMs, this, [=]() {
            // Busca la tecla real del teclado a la que corresponde esta nota
            Tecla* teclaReal = nullptr;
            for (Tecla* t : teclado->getTeclas()) {
                if (t->getOctava() == octava && t->getNombres().contains(nota)) {
                    teclaReal = t;
                    break;
                }
            }
            if (!teclaReal) return;

            // Calcula la posición exacta en la escena donde debe aparecer la nota cayendo
            QRectF rect = teclaReal->mapRectToScene(teclaReal->boundingRect());

            // Crea visualmente la nota cayendo en esa posición
            crearNotaCayendo(rect.x(), -rect.height(), teclaReal);
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

    // Crear un texto inicial para el mensaje de pausa
    QGraphicsTextItem* mensaje = scene->addText("PULSA ESPACIO PARA PONER PAUSA");
    mensaje->setDefaultTextColor(Qt::yellow);
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

        // Leer y lanzar las notas desde el JSON
        leerNotasDesdeJson(":/1stGnossienne_EricSatie.json");
    });
}

MainWindow::~MainWindow() {
    // Destruir objetos
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
    connect(timer, &QTimer::timeout, this, [=]() mutable {
        nota->moveBy(0, 5);  // Desplaza hacia abajo

        // Si está cerca de la tecla objetivo, se ilumina
        if (nota->y() + alturaNota > teclaObjetivo->scenePos().y()) {
            teclaObjetivo->iluminar();
        }

        // Si sale de la pantalla, se limpia y se apaga la tecla
        if (nota->y() > alturaPantalla) {
            timer->stop();
            timer->deleteLater();
            teclaObjetivo->apagar();
            scene->removeItem(nota);
            delete nota;
        }
    });

    timer->start(30);  // Velocidad de caída
}

// Este método se activa cuando el usuario pulsa cualquier tecla mientras la ventana tiene el foco
void MainWindow::keyPressEvent(QKeyEvent* event) {
    // Si la tecla pulsada es la barra espaciadora
    if (event->key() == Qt::Key_Space) {
        // Llama al método que mostrará el menú de pausa (lo crearemos en el siguiente paso)
        mostrarMenuPausa();
    }
}

void MainWindow::mostrarMenuPausa() {
    PausaDialog dialog(this);

    // Conectar las acciones del menú
    connect(&dialog, &PausaDialog::reiniciarClicked, this, &MainWindow::reiniciarCancion);
    connect(&dialog, &PausaDialog::salirClicked, this, &MainWindow::cerrarAplicacion);

    dialog.exec(); // Bloquea hasta cerrar
}

void MainWindow::reiniciarCancion() {
    // Aquí podrías limpiar la escena, reiniciar timers, etc.
    qDebug() << "Reiniciando canción...";
    // Por ejemplo:
    QCoreApplication::exit(123); // Puedes reiniciar la app o recargar escena
}

void MainWindow::cerrarAplicacion() {
    qDebug() << "Saliendo de la app...";
    QCoreApplication::quit();
}


