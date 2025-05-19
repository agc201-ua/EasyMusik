#include "MenuPrincipal.h"
#include <QStyle>
#include <QApplication>
#include <QScreen>
#include <QFile>
#include <QProcess>
#include <QMouseEvent>
#include "VentanaCarga.h"

// Constructor del widget de la canción individual
CancionItem::CancionItem(const QString &titulo, const QString &artista, QWidget *parent)
    : QWidget(parent), tituloCancion(titulo), artistaCancion(artista) {
    // Configuración del widget principal
    setMinimumHeight(120);
    setMinimumWidth(300);
    setObjectName("cancionItem");

    // Para que se expanda horizontalmente
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Crear un layout principal con márgenes
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(40, 10, 40, 10); // Establece márgenes (izq, arriba, der, abajo)

    // Widget contenedor principal que tendrá el fondo blanco
    QWidget *containerWidget = new QWidget();
    containerWidget->setObjectName("contenedorBlanco");
    containerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Layout horizontal para el contenedor principal
    QHBoxLayout *containerLayout = new QHBoxLayout(containerWidget);
    containerLayout->setContentsMargins(16, 12, 16, 12);
    containerLayout->setSpacing(10);

    // Widget para la información (título y artista)
    QWidget *infoContainer = new QWidget();
    QVBoxLayout *infoLayout = new QVBoxLayout(infoContainer);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(8);

    // Crear y configurar el label del título
    tituloLabel = new QLabel(titulo, infoContainer);
    QFont tituloFont = tituloLabel->font();
    tituloFont.setPointSize(14);
    tituloFont.setBold(true);
    tituloLabel->setFont(tituloFont);

    // Crear y configurar el label del artista
    artistaLabel = new QLabel(artista, infoContainer);
    QFont artistaFont = artistaLabel->font();
    artistaFont.setPointSize(12);
    artistaLabel->setFont(artistaFont);

    // Añadir labels al layout de info
    infoLayout->addWidget(tituloLabel);
    infoLayout->addWidget(artistaLabel);

    // Botón de reproducción
    playButton = new QPushButton();
    playButton->setFixedSize(50, 50);
    playButton->setCursor(Qt::PointingHandCursor);
    playButton->setIcon(QIcon(":/recursos/playButton.png")); // Icono de play
    playButton->setIconSize(QSize(30, 30));
    playButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #e6e6e6;"
        "    border-radius: 25px;"
        "    border: none;"
        "    padding-left: 5px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #cccccc;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #cccccc;"
        "}"
    );

    // Añadir widgets al layout del contenedor
    containerLayout->addWidget(infoContainer, 1); // Que ocupe el espacio disponible
    containerLayout->addWidget(playButton);

    // Añadir el contenedor blanco al layout principal
    mainLayout->addWidget(containerWidget);

    // Estilo visual
    setStyleSheet(
        "QWidget#cancionItem {"
        "    background-color: transparent;"
        "}"
        "QWidget#contenedorBlanco {"
        "    background-color: white;"
        "    border: 1px solid #CCCCCC;"
        "    border-radius: 10px;"
        "}"
        "QLabel {"
        "    color: black;"
        "}"
    );

    // Conectar evento del botón de reproducción
    connect(playButton, &QPushButton::clicked, [this]() {
        emit playCancion(tituloCancion, artistaCancion);
    });
}

// Constructor
MenuPrincipal::MenuPrincipal(QWidget *parent) : QWidget(parent) {
    // Inicializar la interfaz
    inicializarUI();

    // Conectar a la base de datos y cargar canciones
    if (conectarBaseDeDatos()) {
        cargarCancionesDesdeBD();
    } else {
        QMessageBox::warning(this, "Error de Base de Datos",
                             "No se pudo conectar a la base de datos de canciones.");
    }
}

// Destructor
MenuPrincipal::~MenuPrincipal() {
    if (db.isOpen()) {
        db.close();
    }
}

// Inicializamos la interfaz
void MenuPrincipal::inicializarUI() {
    // Layout principal
    mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    // Layout horizontal para título y botón de salida
    QHBoxLayout *headerLayout = new QHBoxLayout();

    // Título del menú
    QLabel *titulo = new QLabel("EasyMusik", this);
    QFont tituloFont = titulo->font();
    tituloFont.setPointSize(24);
    tituloFont.setBold(true);
    titulo->setFont(tituloFont);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet(
        "color: white; "
        "margin-bottom: 20px;"
    );
    mainLayout->addWidget(titulo);

    // Boton para salir de la app
    btnSalir = new QPushButton("x", this);
    btnSalir->setMinimumSize(30, 30);
    btnSalir->setMaximumSize(30, 30);
    QFont btnSalirFont("Arial", 20);
    btnSalirFont.setPointSize(24);
    btnSalirFont.setBold(true);
    btnSalir->setFont(btnSalirFont);
    btnSalir->setCursor(Qt::PointingHandCursor);
    btnSalir->setStyleSheet(
        "QPushButton {"
        "    background-color: white;"
        "    color: black;"
        "    border-radius: 30px;"
        "    padding-bottom: 7px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e6e6e6;"
        "    color: #1e1e1e;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #e6e6e6;"
        "    color: #1e1e1e;"
        "}"
    );

    // Conectar señal del botón
    connect(btnSalir, &QPushButton::clicked, this, &MenuPrincipal::cerrarAplicacion);

    // Añadir el título y el botón al layout horizontal
    headerLayout->addWidget(titulo, 1); // El 1 da peso al título para que ocupe espacio
    headerLayout->addWidget(btnSalir, 0, Qt::AlignRight | Qt::AlignTop); // Alinea el botón a la derecha

    // Añadir el layout horizontal al layout principal
    mainLayout->addLayout(headerLayout);

    // Área de desplazamiento para las canciones
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet(
        "QScrollArea {"
        "    background-color: transparent;"
        "}"
        "QScrollBar:vertical {"
        "    background-color: #353535;"
        "    width: 10px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background-color: white;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0px;"               // Ocultar botones de arriba/abajo
        "}"
    );

    // Contenedor para las canciones
    contenedorCanciones = new QWidget(scrollArea);
    cancionesLayout = new QVBoxLayout(contenedorCanciones);
    cancionesLayout->setContentsMargins(10, 10, 10, 10);
    cancionesLayout->setSpacing(15);
    cancionesLayout->setAlignment(Qt::AlignTop);

    scrollArea->setWidget(contenedorCanciones);
    mainLayout->addWidget(scrollArea);

    // Botón para agregar nueva canción
    btnAgregar = new QPushButton("+", this);
    btnAgregar->setMinimumSize(60, 60);
    btnAgregar->setMaximumSize(60, 60);
    QFont btnFont = btnAgregar->font();
    btnFont.setPointSize(24);
    btnFont.setBold(true);
    btnAgregar->setFont(btnFont);
    btnAgregar->setCursor(Qt::PointingHandCursor);
    btnAgregar->setStyleSheet(
        "QPushButton {"
        "    background-color: white;"
        "    color: black;"
        "    border-radius: 30px;"
        "    padding-bottom: 7px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #e6e6e6;"
        "    color: #1e1e1e;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #e6e6e6;"
        "    color: #1e1e1e;"
        "}"
    );

    // Layout para centrar el botón
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(btnAgregar);
    btnLayout->addStretch();
    mainLayout->addLayout(btnLayout);

    // Conectar señal del botón
    connect(btnAgregar, &QPushButton::clicked, this, &MenuPrincipal::agregarNuevaCancion);
}

bool MenuPrincipal::conectarBaseDeDatos() {
    QSqlDatabase::removeDatabase("QSQLITE");
    db = QSqlDatabase::addDatabase("QSQLITE");

    // Encontrar la ruta de la base de datos
    QString rutaBase = QCoreApplication::applicationDirPath();
    QDir dir(rutaBase);
    dir.cdUp(); // Sube un nivel a build
    dir.cdUp(); // Sube un nivel a EasyMusik
    QString rutaEM = dir.absolutePath();
    QString rutaDB = dir.filePath("canciones.db");

    // Se comprueba si ya existe la base de datos
    bool existeDB = QFile::exists(rutaDB);

    // Si no existe la base de datos, se ejecuta el script para crearla
    if (!existeDB) {
        QString directorioOriginal = QDir::currentPath();
        QString scriptPath = rutaEM + "/scriptBD.py";

        // Cambiar al directorio de trabajo
        if (!QDir::setCurrent(rutaEM)) {
            qDebug() << "Error: No se pudo cambiar al directorio" << rutaEM;
            return false;
        }

        // Ejecutar el script
        int resultado = QProcess::execute("python", QStringList() << "scriptBD.py");

        // Comprobar si se ha ejecutado el script correctamente
        if (resultado != 0) {
            qDebug() << "Error al ejecutar el script Python. Código de salida:" << resultado;
            return false;
        }

        // Restaurar el directorio original
        QDir::setCurrent(directorioOriginal);
    }

    // Configurar el nombre de la base de datos
    db.setDatabaseName(rutaDB);

    // Intentar abrir la base de datos
    if (!db.open()) {
        return false;
    }
    else {
        return true;
    }
}

void MenuPrincipal::cargarCancionesDesdeBD() {
    // Limpiar layout previo
    QLayoutItem *child;
    while ((child = cancionesLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }

    // Consultar canciones en la base de datos
    QSqlQuery query("SELECT titulo, autor, bpm FROM Partituras ORDER BY titulo");

    while (query.next()) {
        QString titulo = query.value(0).toString();
        QString artista = query.value(1).toString();
        qreal bpm = query.value(2).toInt();

        // Crear widget de canción
        CancionItem *cancionItem = new CancionItem(titulo, artista, contenedorCanciones);

        // Conectar señal de reproducción
        connect(cancionItem, &CancionItem::playCancion,
                this, &MenuPrincipal::onPlayCancion);

        // Añadir al layout
        cancionesLayout->addWidget(cancionItem);
    }

    // Añadir espacio al final para que se vea bonico
    cancionesLayout->addStretch();
}

void MenuPrincipal::agregarNuevaCancion() {
    // Seleccionar archivo PDF
    QString rutaPDF = QFileDialog::getOpenFileName(this,
                                                   "Seleccionar Archivo de Partitura (PDF)",
                                                   "",
                                                   "Archivos PDF (*.pdf)");

    if (rutaPDF.isEmpty()) {
        return;
    }

    // Pedir datos al usuario
    QString titulo = QInputDialog::getText(this, "Nueva Canción", "Título de la canción:");
    if (titulo.isEmpty()) titulo = "Desconocido";

    QString artista = QInputDialog::getText(this, "Nueva Canción", "Artista:");
    if (artista.isEmpty()) artista = "Desconocido";

    QString mensaje = "Bpm (velocidad de reproducción de la canción):";

    int bpm;
    bool bpmOk;
    bpm = ritmo.toInt(&bpmOk);
    if (!bpmOk) bpm = 100;

    // Ejecutar el script Python
    QString programa = "python";
    QStringList argumentos;

    QString rutaBase = QCoreApplication::applicationDirPath();
    QDir dir(rutaBase);
    dir.cdUp(); // Sube un nivel a build
    dir.cdUp(); // Sube un nivel a EasyMusik
    QString rutaScript = dir.filePath("main_converter_ejemplo.py");

    argumentos << rutaScript
               << rutaPDF
               << titulo
               << artista
               << QString::number(bpm);

    qDebug() << "Programa:" << programa;
    qDebug() << "Ruta del script:" << rutaScript;
    qDebug() << "Argumentos:";
    for (const QString &arg : argumentos) {
        qDebug() << "  " << arg;
    }

    QProcess *proceso = new QProcess(this);
    proceso->setWorkingDirectory(dir.absolutePath());

    VentanaCarga *ventanaCarga = new VentanaCarga(this);
    ventanaCarga->show();

    // Conectar señales
    connect(proceso, &QProcess::finished, this, [=](int exitCode, QProcess::ExitStatus status) {
        ventanaCarga->close();
        ventanaCarga->deleteLater();

        QString salida = proceso->readAllStandardOutput();
        QString errores = proceso->readAllStandardError();

        qDebug() << "Salida estándar:\n" << salida;
        qDebug() << "Errores estándar:\n" << errores;
        qDebug() << "Exit code:" << exitCode;

        proceso->deleteLater();

        if (status == QProcess::CrashExit || exitCode != 0 || !errores.isEmpty()) {
            QMessageBox::warning(this, "Error", "La conversión falló:\n" + errores);
            return;
        }

        cargarCancionesDesdeBD();
        QMessageBox::information(this, "Éxito", "La canción se ha añadido correctamente.");
    });

    connect(proceso, &QProcess::errorOccurred, this, [=](QProcess::ProcessError error) {
        ventanaCarga->close();
        ventanaCarga->deleteLater();
        proceso->deleteLater();
        QMessageBox::critical(this, "Error", "No se pudo iniciar el proceso:\n" + proceso->errorString());
    });

    // Iniciar proceso
    proceso->start(programa, argumentos);
}

// Emitir señal con la canción seleccionada
void MenuPrincipal::onPlayCancion(const QString &titulo, const QString &artista) {
    emit playCancion(titulo, artista);
}

// Cerrar la aplicación
void MenuPrincipal::cerrarAplicacion() {
    qDebug() << "Saliendo de la app...";
    QCoreApplication::quit();
}
