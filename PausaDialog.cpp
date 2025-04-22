#include "PausaDialog.h"
#include "ui_PausaDialog.h"

PausaDialog::PausaDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PausaDialog)
{
    ui->setupUi(this);

    // Conectar señales con botones
    connect(ui->btnReiniciar, &QPushButton::clicked, this, [=]() {
        emit reiniciarClicked();  // Lanza la señal
        this->accept();           // Cierra el diálogo automáticamente
    });
    connect(ui->btnSalir, &QPushButton::clicked, this, &PausaDialog::salirClicked);
}

PausaDialog::~PausaDialog()
{
    delete ui;
}
