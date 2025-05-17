#ifndef MENUPRINCIPAL_H
#define MENUPRINCIPAL_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QCoreApplication>

// Widget que representa cada canción dentro del menú
class CancionItem : public QWidget {
    Q_OBJECT
public:
    CancionItem(const QString &titulo, const QString &artista, QWidget *parent = nullptr);
    QString getTitulo() const { return tituloCancion; }
    QString getArtista() const { return artistaCancion; }

signals:
    void playCancion(const QString &titulo, const QString &artista);

private:
    QString tituloCancion;
    QString artistaCancion;
    QLabel *tituloLabel;
    QLabel *artistaLabel;
    QPushButton *playButton;
};

// Clase principal del menú
class MenuPrincipal : public QWidget {
    Q_OBJECT

signals:
    void playCancion(const QString &titulo, const QString &artista);

private:
    // Atributos
    QVBoxLayout *mainLayout;
    QScrollArea *scrollArea;
    QWidget *contenedorCanciones;
    QVBoxLayout *cancionesLayout;
    QPushButton *btnAgregar;
    QPushButton *btnSalir;
    QSqlDatabase db;
    // Métodos
    void inicializarUI();
    void cargarCancionesDesdeBD();
    bool conectarBaseDeDatos();
    void agregarNuevaCancion();
    void onPlayCancion(const QString &titulo, const QString &artista);
    void cerrarAplicacion();

public:
    MenuPrincipal(QWidget *parent = nullptr);
    ~MenuPrincipal();
};

#endif
