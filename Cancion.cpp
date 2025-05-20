#include "Cancion.h"
#include "PausaDialog.h"
#include "FinDialog.h"
#include <QSoundEffect>
#include <QFileInfo>
#include <QDir>

// Inicializar la interfaz
void Cancion::inicializarUI() {
    // Aplicar estilo global al widget de Cancion
    setStyleSheet("background-color: #212121;");

    // Crear layout principal para el widget
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Se crea la escena con fondo oscuro
    scene = new QGraphicsScene(this);
    scene->setBackgroundBrush(QColor("#212121"));

    // Se crea y se configura la vista
    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setBackgroundBrush(QColor("#212121")); // Asegurar que la vista tenga el mismo fondo

    // Configurar la vista para evitar artefactos de renderizado
    view->setCacheMode(QGraphicsView::CacheBackground);
    view->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    view->setOptimizationFlags(QGraphicsView::DontAdjustForAntialiasing |
                               QGraphicsView::DontSavePainterState);

    // Importante: Añadir la vista al layout
    mainLayout->addWidget(view);

    // Deshabilitar el scroll
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Eliminar el marco de la vista
    view->setFrameShape(QFrame::NoFrame);
    view->setStyleSheet("border: none; background-color: #212121;");

    // Se obtienen las dimensiones de la pantalla
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    anchuraPantalla = screenGeometry.width();
    alturaPantalla = screenGeometry.height();

    // Ajustar el tamaño de la escena
    scene->setSceneRect(0, 0, anchuraPantalla, alturaPantalla);

    // Ajustar la vista para mostrar toda la escena sin escala
    view->setFixedSize(anchuraPantalla, alturaPantalla);

    // Asegurarse de que el viewport tenga el fondo correcto
    view->viewport()->setStyleSheet("background-color: #212121;");

    // Se crea el teclado y se añaden sus teclas a la escena
    teclado = new Teclado(scene, anchuraPantalla, alturaPantalla);
}

// Constructor
Cancion::Cancion(QWidget *parent, QString cancion, QString artista) {

    // Se define el nombre, el artista, y el tempo de la canción
    nombreCancion = cancion;
    nombreArtista = artista;

    // Se inicializa la interfaz gráfica
    inicializarUI();

    // Se muestra el mensaje de pausa
    QTimer* timerMensajePausa = new QTimer(this);
    connect(timerMensajePausa, &QTimer::timeout, this, [=]() {
        // Crear el mensaje de pausa
        mensajePausa = scene->addText("Pulse espacio para pausar");
        mensajePausa->setDefaultTextColor(Qt::white);
        QFont fuente("Arial", 18, QFont::Bold);
        mensajePausa->setFont(fuente);

        // Centrar el texto horizontalmente
        qreal x = (anchuraPantalla - mensajePausa->boundingRect().width()) / 2;
        qreal y = alturaPantalla * 0.10 - mensajePausa->boundingRect().height();
        mensajePausa->setPos(x, y);

        // Iniciar otro temporizador para ocultar el mensaje después de un tiempo
        QTimer::singleShot(5000, this, [this]() {
            if (mensajePausa && mensajePausa->scene()) {
                scene->removeItem(mensajePausa);
                delete mensajePausa;
                mensajePausa = nullptr;
            }
        });

        // Desconectar este timer para que se ejecute solo una vez
        timerMensajePausa->disconnect();
        timerMensajePausa->deleteLater();
    });

    // Iniciar el timer
    timerMensajePausa->start(0);

    // Se inicializan las variables de control
    cancionEnPausa = false;
    cancionReproduciendo = false;

    // Se muestra la cuenta atrás y se lanzan las notas
    mostrarCuentaAtras([this]() {
        tiempoPausaAcumulado = 0;
        cancionTimer.start();
        cancionReproduciendo = true;

        leerNotasDesdeBaseDeDatos();
    });
}

// Destructor
Cancion::~Cancion() {    
    // Detener todos los timers y animaciones
    for (QTimer* t : timersNotas) {
        if (t) {
            t->disconnect();
            delete t;
        }
    }
    timersNotas.clear();

    // Limpiar los timers de notas cayendo
    for (auto& nota : notasCayendo) {
        if (nota.timer) {
            nota.timer->disconnect();
            delete nota.timer;
        }
    }
    notasCayendo.clear();

    // Detener y limpiar sonidos activos
    for (QSoundEffect* sonido : sonidosActivos) {
        if (sonido) {
            delete sonido;
        }
    }
    sonidosActivos.clear();

    // Eliminar elementos restantes en la escena
    if (teclado) {
        delete teclado;
    }

    if (scene) {
        scene->clear();
        delete scene;
    }

    if (view) {
        delete view;
    }
}

//Carga la cancion pasada en los parametros de la base de datos si esta guardada
void Cancion::leerNotasDesdeBaseDeDatos() {
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
    QString string_query = QString("SELECT contenido, bpm FROM Partituras WHERE titulo = '%1' AND autor = '%2'")
                               .arg(nombreCancion)
                               .arg(nombreArtista);

    if (!query.exec(string_query)) {
        qWarning() << "Error al ejecutar la consulta: " << query.lastError().text();
        db.close();
        return;
    }

    if (query.next()) {
        QString contenidoJson = query.value(0).toString();
        QJsonDocument doc = QJsonDocument::fromJson(contenidoJson.toUtf8());
        bpm = query.value(1).toInt();

        if (!doc.isObject()) {
            qWarning("El contenido JSON no es un objeto.");
            db.close();
            return;
        }

        notasJson = doc.object()["Notas"].toArray();
        programarNotasCayendo();
    } else {
        qWarning() << "No se encontró la partitura para título y autor dados.";
    }
}

// Iniciar la canción
void Cancion::iniciarCancion() {
    cancionReproduciendo = true;
    cancionEnPausa = false;
    tiempoPausaAcumulado = 0;
    cancionTimer.start();

    programarNotasCayendo();
}

// Programar todas las notas que caerán al iniciar el programa
void Cancion::programarNotasCayendo() {
    // Se limpia cualquier nota que haya sido programada anteriormente
    for (auto& nota : notasCayendo) {
        if (nota.timer) {
            nota.timer->stop();
            nota.timer->deleteLater();
        }
    }
    notasCayendo.clear();



    // Se programa cada nota cayendo a partir de las notas guardadas en el array del JSON
    for (int i = 0; i < notasJson.size(); i++) {

        QJsonObject datos = notasJson[i].toObject();

        NotaCayendo nota;
        nota.datos = datos;
        nota.procesada = false;
        nota.timer = new QTimer(this);
        nota.timer->setSingleShot(true);
        if(i==notasJson.size()-1)
        {nota.esUltimaNota=true;}
        else{nota.esUltimaNota=false;}
        // Conectar el timer para reproducir la nota cuando sea el momento
        connect(nota.timer, &QTimer::timeout, this, [this, i]() {
            crearNotaCayendo(i,notasCayendo[i].esUltimaNota);
            notasCayendo[i].procesada = true;
        });

        notasCayendo.append(nota);
    }

    // Se inician todos los timers con el tiempo correcto
    actualizarTimersNotas();
}


// Crear una nota cayendo en específico
void Cancion::crearNotaCayendo(int indice,bool esUltima) {
    QJsonObject obj = notasJson[indice].toObject();
    QString nota = obj["Nota"].toString();
    int octava = obj["Octava"].toInt();
    QString codigo = nota + QString::number(octava);
    qreal duracionNota = obj["Duracion"].toDouble();

    // Se busca la tecla correspondiente
    Tecla* teclaReal = nullptr;
    for (Tecla* t : teclado->getTeclas()) {
        if (t->getOctava() == octava && t->getNombres().contains(nota)) {
            teclaReal = t;
            break;
        }
    }
    if (!teclaReal) return;

    // Se crear la animación de la nota cayendo
    QRectF rect = teclaReal->mapRectToScene(teclaReal->boundingRect());
    crearNotaCayendo(rect.x(), -rect.height(), teclaReal, duracionNota, esUltima);

    // Se actualiza el índice de nota actual
    if (indice == indiceNotaActual) {
        indiceNotaActual++;
    }
}


// Crea una nota visual que cae desde una posición concreta y colisiona con una tecla
void Cancion::crearNotaCayendo(qreal posX, qreal posY, Tecla* teclaObjetivo, qreal duracion,bool esUltima) {
    // Se definen los pixeles que caerán por frame
    qreal pixelesPorDesplazamiento = 5;
    qreal intervaloEnMilisegundos = 30;

    qreal segundosPorPulso = 60.0 / bpm; // Tiempo de una negra
    qreal tiempoDuracionNota = duracion * segundosPorPulso; // Duración real en segundos

    qreal velocidadCaida = pixelesPorDesplazamiento / (intervaloEnMilisegundos / 1000.0);

    // Se define el tamaño de la nota (tamaño igual que la tecla objetivo con la que colisionará)
    qreal anchoNota = teclaObjetivo->boundingRect().width();
    qreal alturaNota = tiempoDuracionNota * velocidadCaida;

    // Se crea un rectángulo que representa la nota cayendo
    auto* nota = new QGraphicsRectItem(0, 0, anchoNota, alturaNota);
    nota->setBrush(Qt::cyan);                // Color cyan
    nota->setPen(QPen(Qt::black));           // Borde negro
    nota->setZValue(-1);                     // Aparece detrás del teclado
    nota->setPos(posX, -alturaNota);         // Posición inicial fuera de pantalla

    // Se añade la nota a la escena
    scene->addItem(nota);

    // Se crea el sonido de audio
    QSoundEffect *audio = new QSoundEffect();
    sonidosActivos.append(audio);
    bool audioCargado = false;

    // Se crea un temporizador para mover la nota continuamente
    QTimer* timer = new QTimer(this);
    timersNotas.append(timer); // Guarda este temporizador de movimiento
    connect(timer, &QTimer::timeout, this, [=]() mutable {
        nota->moveBy(0, pixelesPorDesplazamiento);  // Desplaza hacia abajo tantos pixeles como definidos anteriormente

        // Si está cerca de la tecla objetivo, se ilumina
        if (nota->y() + alturaNota > teclaObjetivo->scenePos().y()) {
            teclaObjetivo->iluminar();

            // Se carga el sonido al colisionar con la tecla objetivo
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

                // Se crea un timer para desvanecer el audio
                QTimer::singleShot(duracion * 1000, this, [=]() {
                    QTimer* fadeTimer = new QTimer(this);
                    fadeTimer->setInterval(intervaloEnMilisegundos);

                    connect(fadeTimer, &QTimer::timeout, this, [=]() mutable {
                        qreal volActual = audio->volume();
                        qreal nuevoVol = volActual - 0.025;

                        // Si acaba de producirse el audio, se paran los sonidos y sus temporizadores respectivos
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
        }

        // Si la nota cayendo se sale de la pantalla, esta se elimina y se apaga la tecla del teclado
        if (nota->y() > alturaPantalla - teclaObjetivo->getAltura()) {
            timer->stop();
            timersNotas.removeOne(timer);
            timer->deleteLater();
            teclaObjetivo->apagar();
            scene->removeItem(nota);
            delete nota;
            if (esUltima) {
                QTimer::singleShot(1000, this, [this]() {
                    mostrarMenuFin();
                });
            }

        }
    });

    // Si la canción no está en pausa entonces se inicia el timer
    if (!cancionEnPausa) {
        timer->start(intervaloEnMilisegundos);
    }
}

// Actualizar todos los timers según el tiempo transcurrido
void Cancion::actualizarTimersNotas() {
    if (!cancionReproduciendo) return;

    qint64 tiempoActual = cancionTimer.elapsed() - tiempoPausaAcumulado;

    for (int i = 0; i < notasCayendo.size(); i++) {
        auto& nota = notasCayendo[i];
        if (nota.procesada) continue;

        float offset = static_cast<float>(nota.datos["Offset"].toDouble());
        int tiempoNota = static_cast<int>(offset * 1000);

        // Se alcula el tiempo restante hasta esta nota
        int tiempoRestante = tiempoNota - tiempoActual;

        // Si el tiempo ya pasó pero la nota no se procesó, entonces reproducirla inmediatamente
        if (tiempoRestante <= 0) {
            crearNotaCayendo(i,notasCayendo[i].esUltimaNota);
            nota.procesada = true;
        }
        // Si falta tiempo, programar el timer
        else {
            nota.timer->stop();
            nota.timer->setInterval(tiempoRestante);
            nota.timer->start();
        }
    }
}

// Mostrar la cuenta regresiva y llamar al callback al terminar
void Cancion::mostrarCuentaAtras(std::function<void()> callbackAlTerminar) {
    cuentaAtrasEnProceso = true;

    // Se especifica el diseño el texto
    auto* textoCuenta = new QGraphicsTextItem();
    textoCuenta->setDefaultTextColor(Qt::white);
    textoCuenta->setFont(QFont("Arial", 100, QFont::Bold));
    textoCuenta->setZValue(10);  // Para posicionar el texto encima de todo

    // Se declara el texto
    QStringList mensajes = { "3", "2", "1", "¡YA!" };
    int* indice = new int(0); // Puntero para mantener el valor entre llamadas

    // Precargar un sonido con volumen 0 para evitar lag inicial
    Tecla* teclaEjemplo = teclado->getTeclas().isEmpty() ? nullptr : teclado->getTeclas().first();

    if (teclaEjemplo) {
        QSoundEffect* warmup = new QSoundEffect(this);
        warmup->setSource(QUrl::fromLocalFile(teclaEjemplo->getRutaAudio()));
        warmup->setVolume(0.0);  // Sin sonido audible
        warmup->play();

        // Lo destruimos poco después para liberar memoria
        QTimer::singleShot(1000, this, [warmup]() {
            warmup->stop();
            warmup->deleteLater();
        });
    }
    // Comienza el timer
    QTimer* temporizador = new QTimer(this);
    connect(temporizador, &QTimer::timeout, this, [=]() mutable {
        // Si aun hay mensajes, imprimirlos por pantalla
        if (*indice < mensajes.size()) {
            // Centramos la posición del texto
            textoCuenta->setPlainText(mensajes[*indice]);
            QRectF bounds = textoCuenta->boundingRect();
            qreal xCentrado = (anchuraPantalla - bounds.width()) / 2;
            qreal yCentrado = (alturaPantalla * 0.25) - (bounds.height() / 2);
            textoCuenta->setPos(xCentrado, yCentrado);

            // Añadimos el elemento a la escena si no está ya
            if (textoCuenta->scene() == nullptr) {
                scene->addItem(textoCuenta);
            }

            (*indice)++;
        }
        // Si ya no quedan mensajes, ejecutamos el callback
        else {
            cuentaAtrasEnProceso = false;
            temporizador->stop();
            scene->removeItem(textoCuenta);
            delete textoCuenta;
            delete indice;
            temporizador->deleteLater();



            // Si hay un callback, lo ejecutamos
            if (callbackAlTerminar) {

                callbackAlTerminar();
            }
        }
    });

    temporizador->start(1000); // Mostrar un mensaje por segundo
}

// Detectar pulsaciones de teclas
void Cancion::keyPressEvent(QKeyEvent* event) {
    // Si se está haciendo la cuenta atrás, que no se pueda pausar el programa
    if (cuentaAtrasEnProceso)
        return;

    // Si se pulsa el espacio...
    if (event->key() == Qt::Key_Space && cancionReproduciendo) {
        pausarCancion();

        // Muestra el menú de pausa
        mostrarMenuPausa();
    }
}

// Pausar la canción
void Cancion::pausarCancion() {
    if (!cancionReproduciendo || cancionEnPausa) return;

    cancionEnPausa = true;
    tiempoInicioPausa = cancionTimer.elapsed();

    // Se detienen todos los timers activos
    for (auto& nota : notasCayendo) {
        if (!nota.procesada && nota.timer && nota.timer->isActive()) {
            nota.timer->stop();
        }
    }

    // Se detienen todos los timers de animación
    for (QTimer* t : timersNotas) {
        t->stop();
    }

    // Se detienen todos los sonidos activos
    for (QSoundEffect* sonido : sonidosActivos) {
        sonido->stop();
    }
}

// Mostrar el diálogo de pausa
void Cancion::mostrarMenuPausa() {
    PausaDialog dialog(this);
    QGraphicsBlurEffect* blur = new QGraphicsBlurEffect;
    blur->setBlurRadius(8);
    view->setGraphicsEffect(blur);

    // Se conectan las señales con las distintas acciones
    connect(&dialog, &PausaDialog::reiniciarClicked, this, &Cancion::reiniciarCancion);
    connect(&dialog, &PausaDialog::salirClicked, this, &Cancion::finalizarCancion);
    connect(&dialog, &PausaDialog::continuarClicked, this, &Cancion::reanudarCancion);

    // Se abre el diálogo y se bloquea hasta cerrarse
    dialog.exec();

    // Se quita el desenfoque
    view->setGraphicsEffect(nullptr);

}

void Cancion::mostrarMenuFin() {
    FinDialog dialog(this);
    QGraphicsBlurEffect* blur = new QGraphicsBlurEffect;
    blur->setBlurRadius(8);
    view->setGraphicsEffect(blur);

    connect(&dialog, &FinDialog::reiniciarClicked, this, &Cancion::reiniciarCancion);
    connect(&dialog, &FinDialog::salirClicked, this, &Cancion::finalizarCancion);

    dialog.exec();

    view->setGraphicsEffect(nullptr);
}


// Reiniciar la canción
void Cancion::reiniciarCancion() {
    // Se detienen todos los temporizadores activos
    for (QTimer* t : timersNotas) {
        t->stop();
        t->deleteLater();
    }
    timersNotas.clear();

    // Se borran las notas que han sido programadas para caer
    for (auto& nota : notasCayendo) {
        if (nota.timer) {
            nota.timer->stop();
            nota.timer->deleteLater();
        }
    }
    notasCayendo.clear();

    // Se eliminan todas las notas que estén en pantalla
    for (QGraphicsItem* item : scene->items()) {
        // Se eliminan en caso de que estén cayendo
        if (item->zValue() == -1) {
            scene->removeItem(item);
            delete item;
        }
    }

    // Se eliminan todos los sonidos activos
    for (QSoundEffect* sonido : sonidosActivos) {
        sonido->stop();
        sonido->deleteLater();
    }
    sonidosActivos.clear();

    // Se apaga todas las teclas del teclado
    for (Tecla* tecla : teclado->getTeclas()) {
        if (tecla->getIluminada()){
            tecla->apagar();
        }
    }

    // Se reinician las variables de control
    indiceNotaActual = 0;

    // Se muestra una cuenta atrás y se reinicia la canción
    mostrarCuentaAtras([this]() {
        // Reiniciar todos los contadores de tiempo
        tiempoPausaAcumulado = 0;
        cancionTimer.start();
        cancionReproduciendo = true;
        cancionEnPausa = false;

        // Programar las notas
        leerNotasDesdeBaseDeDatos();
    });
}

// Volver al menú principal
void Cancion::finalizarCancion() {
    if (cancionReproduciendo && !cancionEnPausa)
        pausarCancion();

    QTimer::singleShot(0, this, [this]() {
        emit volver();
    });
}

// Reanudar la canción
void Cancion::reanudarCancion() {
    if (!cancionReproduciendo || !cancionEnPausa) return;

    // Se muestra primero una cuenta regresiva
    mostrarCuentaAtras([this]() {
        // Callback cuando termina la cuenta regresiva

        // Calcular tiempo de pausa y actualizar acumulado
        qint64 duracionPausa = cancionTimer.elapsed() - tiempoInicioPausa;
        tiempoPausaAcumulado += duracionPausa;
        cancionEnPausa = false;

        // Reanudar todos los timers de animación
        for (QTimer* t : timersNotas) {
            t->start();
        }

        // Reprogramar todas las notas pendientes
        actualizarTimersNotas();
    });
}
