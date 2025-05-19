// include/VentanaCarga.h
#ifndef VENTANACARGA_H
#define VENTANACARGA_H

#include <QDialog>
#include <QLabel>
#include <QMovie>

class VentanaCarga : public QDialog {
    Q_OBJECT

public:
    VentanaCarga(QWidget *parent = nullptr);
    ~VentanaCarga();

private:
    QLabel *spinnerLabel;
    QMovie *spinnerMovie;
};

#endif // VENTANACARGA_H
