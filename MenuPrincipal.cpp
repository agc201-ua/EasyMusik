#include "MenuPrincipal.h"
#include <QStyle>
#include <QApplication>
#include <QScreen>
#include <QFile>
#include <QProcess>
#include <QMouseEvent>

// Constructor del widget de canción individual
CancionItem::CancionItem(const QString &titulo, const QString &artista, QWidget *parent)
    : QWidget(parent), tituloCancion(titulo), artistaCancion(artista) {

    // Configuración del widget
    setMinimumHeight(120);
    setMinimumWidth(300);
    setMaximumWidth(600);
    setObjectName("cancionItem"); // Para estilo CSS

    // Crear layout principal horizontal
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(16, 12, 16, 12);
    layout->setSpacing(10);

    // Contenedor para título y artista
    QWidget *infoContainer = new QWidget(this);
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

    // Añadir widgets al layout de info
    infoLayout->addWidget(tituloLabel);
    infoLayout->addWidget(artistaLabel);

    // Botón de reproducción
    playButton = new QPushButton(this);
    playButton->setFixedSize(50, 50);
    playButton->setCursor(Qt::PointingHandCursor);
    playButton->setIcon(QIcon(":/recursos/play.png")); // Icono de play
    playButton->setIconSize(QSize(30, 30));
    playButton->setStyleSheet(
        "QPushButton {"
        "    background-color: cyan;"
        "    border-radius: 25px;"
        "    border: none;"
        "}"
        "QPushButton:hover {"
        "    background-color: darkCyan;"
        "}"
        "QPushButton:pressed {"
        "    background-color: darkCyan;"
        "}"
    );

    // Añadir widgets al layout principal
    layout->addWidget(infoContainer, 1); // Con stretch para que ocupe el espacio disponible
    layout->addWidget(playButton);

    // Estilo visual del contenedor
    setStyleSheet(
        "QWidget#cancionItem {"
        "    background-color: #303030;"
        "    border-radius: 10px;"
        "    border: 1px solid #555555;"
        "}"
        "QLabel {"
        "    color: white;"
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

    // Título del menú
    QLabel *titulo = new QLabel("EasyMusik", this);
    QFont tituloFont = titulo->font();
    tituloFont.setPointSize(24);
    tituloFont.setBold(true);
    titulo->setFont(tituloFont);
    titulo->setAlignment(Qt::AlignCenter);
    titulo->setStyleSheet("color: white; margin-bottom: 20px;");
    mainLayout->addWidget(titulo);

    // Área de desplazamiento para las canciones
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet("background-color: transparent;");

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
        "    background-color: cyan;"
        "    color: white;"
        "    border-radius: 30px;"
        "}"
        "QPushButton:hover {"
        "    background-color: darkCyan;"
        "}"
        "QPushButton:pressed {"
        "    background-color: darkCyan;"
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

    // Estilo general
    setStyleSheet("background-color: #212121;");
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
    // Seleccionar archivo JSON
    QString rutaArchivo = QFileDialog::getOpenFileName(this,
                                                       "Seleccionar Archivo de Canción",
                                                       "",
                                                       "Archivos JSON (*.json)");

    if (rutaArchivo.isEmpty()) {
        return;
    }

    // Leer archivo JSON
    QFile archivo(rutaArchivo);
    if (!archivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "No se pudo abrir el archivo seleccionado.");
        return;
    }

    QString contenido = archivo.readAll();
    archivo.close();

    // Extraer nombre, artista y bpm (asumiendo que el usuario los proporciona)
    QString titulo = QInputDialog::getText(this, "Nueva Canción", "Título de la canción:");
    if (titulo.isEmpty()) titulo = "Desconocido";

    QString artista = QInputDialog::getText(this, "Nueva Canción", "Artista:");
    if (artista.isEmpty()) artista = "Desconocido";

    QString ritmo = QInputDialog::getText(this, "Nueva Canción", "Bpm:");
    int bpm;
    bool correctBpm;
    bpm = ritmo.toInt(&correctBpm);
    if (!correctBpm) bpm = 100;

    // Guardar en la base de datos
    QSqlQuery query;
    query.prepare("INSERT INTO Partituras (titulo, autor, bpm, contenido) "
                  "VALUES (:titulo, :autor, :bpm, :contenido)");
    query.bindValue(":titulo", titulo);
    query.bindValue(":autor", artista);
    query.bindValue(":bpm", bpm);
    query.bindValue(":contenido", QString(contenido));

    if (!query.exec()) {
        QMessageBox::warning(this, "Error", "No se pudo guardar la canción en la base de datos.");
        return;
    }

    // Actualizar lista de canciones
    cargarCancionesDesdeBD();

    // Mostrar un mensaje de que la canción ha sido guardada correctamente
    QMessageBox::information(this, "Éxito", "La canción se ha añadido correctamente.");
}

// Emitir señal con la canción seleccionada
void MenuPrincipal::onPlayCancion(const QString &titulo, const QString &artista) {
    emit playCancion(titulo, artista);
}
