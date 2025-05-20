#include "VentanaCarga.h"
#include <QScreen>
#include <QGuiApplication>
#include <QVBoxLayout>

VentanaCarga::VentanaCarga(QWidget *parent)
    : QDialog(parent),
    spinnerLabel(new QLabel(this)),
    spinnerMovie(new QMovie(":/recursos/SpinnerCargaAudiveris.gif"))
{
    // Configurar la ventana sin bordes y que permanezca encima
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    // Configurar el tamaño de la ventana
    setFixedSize(350, 350);

    // Configurar la etiqueta con la animación
    spinnerLabel->setMovie(spinnerMovie);
    spinnerLabel->setAlignment(Qt::AlignCenter);
    spinnerLabel->setFixedSize(250, 250);

    // Crear layout principal
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(spinnerLabel, 0, Qt::AlignCenter);

    // Iniciar la animación
    spinnerMovie->start();

    // Verificar que la animación se haya cargado correctamente
    if (!spinnerMovie->isValid()) {
        qDebug() << "Error: el GIF no se cargó correctamente.";
    }

    // Centrar la ventana en la pantalla o en el widget padre
    if (parentWidget()) {
        // Centrar en el widget padre
        QRect parentRect = parentWidget()->geometry();
        move(parentRect.center() - rect().center());
    } else {
        // Centrar en la pantalla
        QScreen *screen = QGuiApplication::primaryScreen();
        QRect screenGeometry = screen->geometry();
        move((screenGeometry.width() - width()) / 2,
             (screenGeometry.height() - height()) / 2);
    }
}

// Destructor
VentanaCarga::~VentanaCarga()
{
    spinnerMovie->stop();
}
