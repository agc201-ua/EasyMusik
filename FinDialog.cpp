#include "FinDialog.h"
#include "ui_FinDialog.h"
#include <QLabel>


FinDialog::FinDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FinDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);

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
