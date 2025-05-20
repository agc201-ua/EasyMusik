#include "FinDialog.h"
#include "ui_FinDialog.h"
#include <QLabel>
#include <QScreen>

FinDialog::FinDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FinDialog)
{
    ui->setupUi(this);

    // Configuración de la ventana
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    // Crear un contenedor negro para los botones con tamaño fijo
    QFrame* contenedorNegro = new QFrame(this);
    contenedorNegro->setObjectName("contenedorNegro");
    contenedorNegro->setFixedSize(350, 280); // Un poco más alto para espaciado

    // Crear un layout para el contenedor negro con buenos márgenes
    QVBoxLayout* layoutContenedor = new QVBoxLayout(contenedorNegro);
    layoutContenedor->setContentsMargins(30, 30, 30, 30); // Márgenes amplios
    layoutContenedor->setSpacing(15); // Espaciado entre elementos reducido ligeramente

    // Añadir un espaciador expansible al principio para empujar todo hacia el centro
    layoutContenedor->addStretch(1);

    // Configuración del boton reiniciar
    if (ui->btnReiniciar) {
        ui->btnReiniciar->setFixedHeight(45);
        ui->btnReiniciar->setMinimumWidth(250);
        layoutContenedor->addWidget(ui->btnReiniciar, 0, Qt::AlignHCenter);
    }

    // Pequeño espaciado entre botones
    layoutContenedor->addSpacing(5);

    // Configuración del boton salir
    if (ui->btnSalir) {
        ui->btnSalir->setFixedHeight(45);
        ui->btnSalir->setMinimumWidth(250);
        layoutContenedor->addWidget(ui->btnSalir, 0, Qt::AlignHCenter);
    }

    // Añadir otro espaciador expansible al final para centrar todo
    layoutContenedor->addStretch(1);

    // Crear un layout principal para el diálogo
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0); // Sin márgenes
    mainLayout->addWidget(contenedorNegro, 0, Qt::AlignCenter);

    setLayout(mainLayout);

    // Fijar un tamaño mínimo para el diálogo
    setMinimumSize(350, 280);

    // Aplicar estilos CSS
    this->setStyleSheet(
        "QDialog {"
        "    background-color: transparent;"
        "}"
        "QFrame#contenedorNegro {"
        "    background-color: #222222;"
        "    border: 1px solid #333333;"
        "    border-radius: 10px;"
        "}"
        "QPushButton {"
        "    background-color: white;"
        "    color: black;"
        "    border: 1px solid #CCCCCC;"
        "    border-radius: 5px;"
        "    min-height: 40px;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #f0f0f0;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #e0e0e0;"
        "}"
        "QLabel {"
        "    color: white;"
        "    background: transparent;"
        "    font-size: 16px;"
        "}"
    );

    // Para que si posas el raton por encima cambie el cursor
    ui->btnReiniciar->setCursor(Qt::PointingHandCursor);
    ui->btnSalir->setCursor(Qt::PointingHandCursor);

    // Centrar en pantalla
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);

    // Conectar señales con botones
    connect(ui->btnReiniciar, &QPushButton::clicked, this, [=]() {
        emit reiniciarClicked();
        this->accept();
    });

    connect(ui->btnSalir, &QPushButton::clicked, this, [=]() {
        emit salirClicked();
        this->accept();
    });
}

FinDialog::~FinDialog()
{
    delete ui;
}
