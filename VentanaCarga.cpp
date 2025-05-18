// src/VentanaCarga.cpp
#include "VentanaCarga.h"
#include <QVBoxLayout>

VentanaCarga::VentanaCarga(QWidget *parent)
    : QDialog(parent), spinnerLabel(new QLabel(this)), spinnerMovie(new QMovie(":/recursos/SpinnerCargaAudiveris.gif")) {

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(350, 350);

    spinnerLabel->setMovie(spinnerMovie);
    spinnerLabel->setAlignment(Qt::AlignVCenter);
    spinnerLabel->setAlignment(Qt::AlignHCenter);
    spinnerLabel->setFixedSize(250, 250);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(spinnerLabel);
    setLayout(layout);

    spinnerMovie->start();

    if (!spinnerMovie->isValid()) {
        qDebug() << "❌ Error: el GIF no se cargó correctamente.";
    }
}

VentanaCarga::~VentanaCarga() {
    spinnerMovie->stop();
}
