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
    // qreal getBpm() const { return bpmCancion; }

signals:
    void playCancion(const QString &titulo, const QString &artista);

private:
    QString tituloCancion;
    QString artistaCancion;
    // qreal bpmCancion;
    QLabel *tituloLabel;
    QLabel *artistaLabel;
    QPushButton *playButton;
};

// Clase principal del menú
class MenuPrincipal : public QWidget {
    Q_OBJECT

signals:
    // void playCancion(const QString &titulo, const QString &artista, const QString &jsonPath);
    void playCancion(const QString &titulo, const QString &artista);

private:
    // Atributos
    QVBoxLayout *mainLayout;
    QScrollArea *scrollArea;
    QWidget *contenedorCanciones;
    QVBoxLayout *cancionesLayout;
    QPushButton *btnAgregar;
    QSqlDatabase db;
    // Métodos
    void cargarCancionesDesdeBD();
    void inicializarUI();
    bool conectarBaseDeDatos();
    void agregarNuevaCancion();
    void onPlayCancion(const QString &titulo, const QString &artista);
    QString obtenerRutaArchivo(const QString &titulo, const QString &artista);

public:
    MenuPrincipal(QWidget *parent = nullptr);
    ~MenuPrincipal();
};

#endif // MENUPRINCIPAL_H
