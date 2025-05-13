#include "MenuPrincipal.h"
#include <QStyle>
#include <QApplication>
#include <QScreen>
#include <QFile>
#include <QMouseEvent>

// Constructor del widget de canción individual
CancionItem::CancionItem(const QString &titulo, const QString &artista, QWidget *parent)
    : QWidget(parent), m_titulo(titulo), m_artista(artista) {

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
    playButton->setIcon(QIcon(":/recursos/play.png")); // Asegúrate de tener este recurso
    playButton->setIconSize(QSize(30, 30));
    playButton->setStyleSheet(
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    border-radius: 25px;"
        "    border: none;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #398e3d;"
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
        emit playCancion(m_titulo, m_artista);
    });
}

// Constructor del menú principal
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

MenuPrincipal::~MenuPrincipal() {
    if (db.isOpen()) {
        db.close();
    }
}

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
        "    background-color: #4CAF50;"
        "    color: white;"
        "    border-radius: 30px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #398e3d;"
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

    QString rutaDB = dir.filePath("canciones.db");
    db.setDatabaseName(rutaDB);

    // Si la base de datos no existe, la creamos
    if (!QFile::exists(rutaDB)) {
        if (!db.open()) {
            return false;
        }

        // Crear tabla de canciones
        QSqlQuery query;
        if (!query.exec("CREATE TABLE IF NOT EXISTS Partituras ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "titulo TEXT NOT NULL, "
                        "autor TEXT NOT NULL, "
                        "contenido TEXT NOT NULL, "
                        "rutaArchivo TEXT)")) {
            db.close();
            return false;
        }

        return true;
    }

    return db.open();
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
    QSqlQuery query("SELECT titulo, autor FROM Partituras ORDER BY titulo");

    while (query.next()) {
        QString titulo = query.value(0).toString();
        QString artista = query.value(1).toString();

        // Crear widget de canción
        CancionItem *cancionItem = new CancionItem(titulo, artista, contenedorCanciones);

        // Conectar señal de reproducción
        connect(cancionItem, &CancionItem::playCancion,
                this, &MenuPrincipal::onPlayCancion);

        // Añadir al layout
        cancionesLayout->addWidget(cancionItem);
    }

    // Añadir espacio al final
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

    // Extraer nombre y artista (asumiendo que el usuario los proporciona)
    QString titulo = QInputDialog::getText(this, "Nueva Canción", "Título de la canción:");
    if (titulo.isEmpty()) return;

    QString artista = QInputDialog::getText(this, "Nueva Canción", "Artista:");
    if (artista.isEmpty()) artista = "Desconocido";

    // Guardar en la base de datos
    QSqlQuery query;
    query.prepare("INSERT INTO Partituras (titulo, autor, contenido) "
                  "VALUES (:titulo, :autor, :contenido)");
    query.bindValue(":titulo", titulo);
    query.bindValue(":autor", artista);
    query.bindValue(":contenido", QString(contenido));

    if (!query.exec()) {
        QMessageBox::warning(this, "Error", "No se pudo guardar la canción en la base de datos.");
        return;
    }

    // Actualizar lista de canciones
    cargarCancionesDesdeBD();

    QMessageBox::information(this, "Éxito", "La canción se ha añadido correctamente.");
}

void MenuPrincipal::onPlayCancion(const QString &titulo, const QString &artista) {
    // Obtener la ruta del archivo JSON
    QString jsonPath = obtenerRutaArchivo(titulo, artista);

    // Emitir señal con la canción seleccionada
    emit playCancion(titulo, artista, jsonPath);
}

QString MenuPrincipal::obtenerRutaArchivo(const QString &titulo, const QString &artista) {
    QSqlQuery query;
    query.prepare("SELECT rutaArchivo, contenido FROM Partituras WHERE titulo = :titulo AND autor = :autor");
    query.bindValue(":titulo", titulo);
    query.bindValue(":autor", artista);

    if (query.exec() && query.next()) {
        // Verificar primero la ruta del archivo
        QString rutaArchivo = query.value(0).toString();
        if (!rutaArchivo.isEmpty() && QFile::exists(rutaArchivo)) {
            return rutaArchivo;
        }

        // Si no existe, crear un archivo temporal con el contenido
        QString contenido = query.value(1).toString();
        QString tempPath = QDir::tempPath() + "/" + titulo + ".json";
        QFile file(tempPath);

        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(contenido.toUtf8());
            file.close();
            return tempPath;
        }
    }

    return QString();
}
