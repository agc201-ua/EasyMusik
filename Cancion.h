#ifndef CANCION_H
#define CANCION_H

#include "Teclado.h"
#include "PausaDialog.h"

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
    //
    void inicializarUI();
    QVBoxLayout* mainLayout;

    // Gestor de eventos
    void keyPressEvent(QKeyEvent* event) override;

    // Elementos de la interfaz
    QGraphicsScene* scene;
    QGraphicsView* view;
    Teclado* teclado;
    QGraphicsTextItem* mensajePausa;

    // Gestión del tiempo y pausas
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

    // Dimensiones de la pantalla
    qreal anchuraPantalla;
    qreal alturaPantalla;

    // Información sobre al canción
    QString nombreCancion;
    QString nombreArtista;
    // QString jsonPath;
    qreal bpm;

    // Otros atributos
    QJsonArray notasJson;
    QList<QTimer*> timersNotas;
    QList<QSoundEffect*> sonidosActivos;

    // Métodos para gestión de la UI
    void mostrarMenuPausa();
    void mostrarCuentaAtras(std::function<void()> callbackAlTerminar = nullptr);

    // Métodos para gestión de la canción
    void iniciarCancion();
    void pausarCancion();
    void reanudarCancion();
    void reiniciarCancion();
    void finalizarCancion();
    void crearNotaCayendo(qreal posX, qreal posY, Tecla* teclaObjetivo, qreal duracion);

    // Métodos para cargar las notas
    void leerNotasDesdeJson(const QString& ruta);
    void leerNotasDesdeBaseDeDatos();

    // Métodos para la gestión de notas y timers
    void programarNotasCayendo();
    void crearNotaCayendo(int indice);
    void actualizarTimersNotas();

public:
    Cancion(QWidget *parent, QString cancion, QString artista);
    ~Cancion();
};

#endif

