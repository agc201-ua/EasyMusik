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
    explicit CancionItem(const QString &titulo, const QString &artista, QWidget *parent = nullptr);
    QString getTitulo() const { return m_titulo; }
    QString getArtista() const { return m_artista; }

signals:
    void playCancion(const QString &titulo, const QString &artista);

private:
    QString m_titulo;
    QString m_artista;
    QLabel *tituloLabel;
    QLabel *artistaLabel;
    QPushButton *playButton;
};

// Clase principal del menú
class MenuPrincipal : public QWidget {
    Q_OBJECT
public:
    explicit MenuPrincipal(QWidget *parent = nullptr);
    ~MenuPrincipal();

signals:
    void playCancion(const QString &titulo, const QString &artista, const QString &jsonPath);

private slots:
    void agregarNuevaCancion();
    void onPlayCancion(const QString &titulo, const QString &artista);

private:
    void cargarCancionesDesdeBD();
    void inicializarUI();
    bool conectarBaseDeDatos();
    QString obtenerRutaArchivo(const QString &titulo, const QString &artista);

    QVBoxLayout *mainLayout;
    QScrollArea *scrollArea;
    QWidget *contenedorCanciones;
    QVBoxLayout *cancionesLayout;
    QPushButton *btnAgregar;
    QSqlDatabase db;
};

#endif // MENUPRINCIPAL_H
