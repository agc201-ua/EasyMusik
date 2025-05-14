#ifndef CANCION_H
#define CANCION_H

#include "Teclado.h"

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QKeyEvent>
#include <QVector>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QDebug>
#include <QSoundEffect>
#include <QElapsedTimer>
#include <QGraphicsTextItem>
#include <QGraphicsBlurEffect>
#include <QScreen>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QCoreApplication>
#include <functional>
#include <qboxlayout.h>


class Cancion : public QWidget {
    Q_OBJECT

signals:
    // Señal para volver al menú principal
    void volver();

private:
    // Atributos para la interfaz
    QVBoxLayout* mainLayout;
    QGraphicsScene* scene;
    QGraphicsView* view;
    Teclado* teclado;
    QGraphicsTextItem* mensajePausa;

    // Atributos para la gestión del tiempo y pausas
    QElapsedTimer cancionTimer;       // Mide el tiempo total desde el inicio
    qint64 tiempoPausaAcumulado = 0;  // Tiempo acumulado en pausas
    qint64 tiempoInicioPausa = 0;     // Momento en el que comenzó la pausa actual
    bool cancionEnPausa = false;
    bool cancionReproduciendo = false;
    bool cuentaAtrasEnProceso = false;
    int indiceNotaActual = 0;
    int tiempoPausado = 0;

    // Estructura para las notas que van cayendo
    struct NotaCayendo {
        QJsonObject datos;
        QTimer* timer;
        bool procesada = false;
    };
    QList<NotaCayendo> notasCayendo;

    // Atributos para las dimensiones de la pantalla
    qreal anchuraPantalla;
    qreal alturaPantalla;

    // Atributos para guardar información sobre al canción
    QString nombreCancion;
    QString nombreArtista;
    qreal bpm;

    // Otros atributos
    QJsonArray notasJson;
    QList<QTimer*> timersNotas;
    QList<QSoundEffect*> sonidosActivos;

    // Métodos para la interfaz
    void inicializarUI();
    void mostrarMenuPausa();
    void mostrarCuentaAtras(std::function<void()> callbackAlTerminar = nullptr);

    // Gestor de eventos
    void keyPressEvent(QKeyEvent* event) override;

    // Métodos para gestión de la canción
    void iniciarCancion();
    void pausarCancion();
    void reanudarCancion();
    void reiniciarCancion();
    void finalizarCancion();

    // Métodos para cargar las notas
    void leerNotasDesdeJson(const QString& ruta);
    void leerNotasDesdeBaseDeDatos();

    // Métodos para la gestión de notas y timers
    void programarNotasCayendo();
    void crearNotaCayendo(qreal posX, qreal posY, Tecla* teclaObjetivo, qreal duracion);
    void crearNotaCayendo(int indice);
    void actualizarTimersNotas();

public:
    Cancion(QWidget *parent, QString cancion, QString artista);
    ~Cancion();
};

#endif

