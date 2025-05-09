#include "PausaDialog.h"
#include "ui_PausaDialog.h"

PausaDialog::PausaDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PausaDialog)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint); // Esto elimina la X y otros botones del sistema

    // Conectar señales con botones
    connect(ui->btnReiniciar, &QPushButton::clicked, this, [=]() {
        emit reiniciarClicked();  // Lanza la señal
        this->accept();           // Cierra el diálogo automáticamente
    });
    connect(ui->btnSalir, &QPushButton::clicked, this, &PausaDialog::salirClicked);
    connect(ui->btnContinuar, &QPushButton::clicked, this, [=]() {
        emit continuarClicked();   // Lanza la señal
        this->accept();            // Cierra el diálogo automáticamente
    });

}

PausaDialog::~PausaDialog()
{
    delete ui;
}
