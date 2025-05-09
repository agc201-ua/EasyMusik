#include "MainWindow.h"
#include <QSoundEffect>
#include <QFileInfo>
#include <QDir>

//Carga la cancion pasada en los parametros de la base de datos si esta guardada
void MainWindow::leerNotasDesdeBaseDeDatos(const QString& titulo, const QString& autor) {
    QSqlDatabase::removeDatabase("QSQLITE");
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    QString rutaBase = QCoreApplication::applicationDirPath(); // build/Desktop_Qt...
    QDir dir(rutaBase);
    dir.cdUp(); // Sube un nivel a build
    dir.cdUp(); // Sube un nivel a EasyMusik
    db.setDatabaseName(dir.filePath("canciones.db"));

    if (!db.open()) {
        qWarning() << "No se pudo abrir la base de datos." << db.lastError().text();
        db.close();
        return;
    }

    qDebug() << "Ruta absoluta de la base de datos:" << QFileInfo(db.databaseName()).absoluteFilePath();

    QSqlQuery query;
    QString string_query = QString("SELECT contenido FROM Partituras WHERE titulo = '%1' AND autor = '%2'")
                               .arg(titulo)
                               .arg(autor);

    if (!query.exec(string_query)) {
        qWarning() << "Error al ejecutar la consulta: " << query.lastError().text();
        db.close();
        return;
    }

    if (query.next()) {
        QString contenidoJson = query.value(0).toString();
        QJsonDocument doc = QJsonDocument::fromJson(contenidoJson.toUtf8());

        if (!doc.isObject()) {
            qWarning("El contenido JSON no es un objeto.");
            db.close();
            return;
        }

        QJsonArray notasArray = doc.object()["Notas"].toArray();

        for (const QJsonValue& valor : notasArray) {
            QJsonObject obj = valor.toObject();
            QString nota = obj["Nota"].toString();
            int octava = obj["Octava"].toInt();
            int bpm = 75;
            qreal segundosPorPulso = 60.0 / bpm;
            float offset = static_cast<float>(obj["Offset"].toDouble()) * segundosPorPulso;
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

                // Se crea la nota cayendo
                crearNotaCayendo(rect.x(), -rect.height(), teclaReal, duracionNota);

                timersNotas.removeOne(timer);
                timer->deleteLater();
            });

            timer->start();
        }
    } else {
        qWarning() << "No se encontró la partitura para título y autor dados.";
        db.close();
        return;
    }

    db.close();
    QSqlDatabase::removeDatabase("Verificador"); // Limpia la conexión
}

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
        QString nota = obj["Nota"].toString();                       // Nombre de la nota (por ej. "C", "D#", etc.)
        int octava = obj["Octava"].toInt();                          // Octava correspondiente
        float offset = static_cast<float>(obj["Offset"].toDouble()); // Momento en segundos en el que debe caer la nota
        QString codigo = nota + QString::number(octava);             // Código de nota (ej. "C4")
        int delayMs = static_cast<int>(offset * 1000);               // Se convierte a milisegundos

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
            qreal duracionNota = obj["Duracion"].toDouble();
            crearNotaCayendo(rect.x(), -rect.height(), teclaReal, duracionNota);

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
    scene->setBackgroundBrush(QColor("#212121"));

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
    mensaje->setDefaultTextColor(Qt::white);
    QFont fuente("Arial", 24, QFont::Bold);
    mensaje->setFont(fuente);

    // Centrar el texto horizontalmente
    qreal x = (anchuraPantalla - mensaje->boundingRect().width()) / 2;
    qreal y = 100; // un poco desde arriba
    mensaje->setPos(x, y);

    // Guardamos el JSON que estamos usando actualmente
    jsonActual = ":/NeverMeantToBelong_Bleach.json";

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

    // Para luego tener en cuenta los pixeles por frame
    qreal pixelesPorDesplazamiento = 5;
    qreal intervaloEnMilisegundos = 30;

    qreal bpm = 75; // Tempo de la canción
    qreal segundosPorPulso = 60.0 / bpm; // Tiempo de una negra
    qreal tiempoDuracionNota = duracion * segundosPorPulso; // Duración real en segundos

    qreal velocidadCaida = pixelesPorDesplazamiento / (intervaloEnMilisegundos / 1000.0);

    // Tamaño igual que la tecla objetivo
    qreal anchoNota = teclaObjetivo->boundingRect().width();
    qreal alturaNota = tiempoDuracionNota * velocidadCaida;

    // Crea un rectángulo que representa la nota cayendo
    auto* nota = new QGraphicsRectItem(0, 0, anchoNota, alturaNota);
    nota->setBrush(Qt::cyan);                // Color cyan
    nota->setPen(QPen(Qt::black));           // Borde negro
    nota->setZValue(-1);                     // Aparece detrás del teclado
    nota->setPos(posX, -alturaNota);         // Posición inicial fuera de pantalla
    scene->addItem(nota);

    QSoundEffect *audio = new QSoundEffect();
    sonidosActivos.append(audio);
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

                /**/
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
                            sonidosActivos.removeOne(audio);
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

    // Si se está haciendo la cuenta atrás, que no se pueda pausar el programa
    if (cuentaAtrasEnProceso)
        return;

    // Si se pulsa el espacio...
    if (event->key() == Qt::Key_Space) {
        // Detiene el temporizador de las notas para que no se muevan
        for (QTimer* t : timersNotas) {
            t->stop();
        }
        // Detiene todos los sonidos activos del fadeout
        for (QSoundEffect* sonido : sonidosActivos) {
            sonido->stop();
            sonido->deleteLater();
        }
        sonidosActivos.clear();

        // Muestra el menú de pausa al pulsar barra espaciadora
        mostrarMenuPausa();
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
    cuentaAtrasEnProceso = true;

    // Diseñar el texto
    auto* textoCuenta = new QGraphicsTextItem();
    textoCuenta->setDefaultTextColor(Qt::white);
    textoCuenta->setFont(QFont("Arial", 100, QFont::Bold));
    textoCuenta->setZValue(10);  // Para posicionarlo encima de todo
    QStringList mensajes = { "3", "2", "1", "¡YA!" };
    int* indice = new int(0); // Puntero para mantener el valor entre llamadas

    // Comienza el timer
    QTimer* temporizador = new QTimer(this);
    connect(temporizador, &QTimer::timeout, this, [=]() mutable {
        // Si aun hay mensajes, imprimirlos por pantalla
        if (*indice < mensajes.size()) {
            // Centramos la posición del texto
            textoCuenta->setPlainText(mensajes[*indice]);
            QRectF bounds = textoCuenta->boundingRect();
            qreal xCentrado = (anchuraPantalla - bounds.width()) / 2;
            qreal yCentrado = (alturaPantalla - bounds.height()) / 2;
            textoCuenta->setPos(xCentrado, yCentrado);

            // Añadimos el elemento a la escena y movemos el puntero
            scene->addItem(textoCuenta);
            (*indice)++;
        }
        // Si ya no quedan mensajes, se leen las notas de la base de datos
        else {
            temporizador->stop();
            scene->removeItem(textoCuenta);
            delete textoCuenta;
            delete indice;
            temporizador->deleteLater();

            // Aquí es donde lanzamos las notas del JSON actual
            /*
            if (!jsonActual.isEmpty()) {
                leerNotasDesdeJson(jsonActual);
            } else {
                qWarning("jsonActual está vacío. No se puede iniciar la canción.");
            }
            */

            cuentaAtrasEnProceso = false;
            leerNotasDesdeBaseDeDatos("1stGnossienne", "EricSatie");
        }
    });

    temporizador->start(1000); // Un mensaje por segundo
}
