#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Teclado.h"
#include "PausaDialog.h"
#include <QMainWindow> // Incluye la clase base de Qt para crear una ventana principal
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QTimer>
#include <QKeyEvent>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>


/*
QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;  // Declaración anticipada de la clase 'MainWindow' que está en 'ui_mainwindow.h'
}
QT_END_NAMESPACE
*/

class MainWindow : public QMainWindow  // `MainWindow` hereda de `QMainWindow` de Qt
{
    Q_OBJECT  // Macro que habilita la funcionalidad de Qt Meta-Object System (para señales y slots)

    private:
        QGraphicsScene* scene;
        QGraphicsView* view;
        Teclado* teclado;
        int anchuraPantalla;
        int alturaPantalla;
        QString jsonActual; // Json que se esta leyendo actualmente
        QList<QTimer*> timersNotas;  // Lista de timers para controlar las notas, para poder pausarlas y reiniciarlas.
        QList<QSoundEffect*> sonidosActivos;


        // Ui::MainWindow *ui;  // Puntero a la interfaz de usuario (UI) generada por Qt Designer

    public:
        MainWindow(QWidget *parent = nullptr);
        ~MainWindow();
        void probar();
        void crearNotaCayendo(qreal posX, qreal posY, Tecla* teclaObjetivo, qreal duracion);
        void leerNotasDesdeJson(const QString& ruta);
        void leerNotasDesdeBaseDeDatos(const QString& titulo, const QString& autor);
        void keyPressEvent(QKeyEvent* event);

    private slots:
        void mostrarMenuPausa();
        void reiniciarCancion();
        void cerrarAplicacion();
        void mostrarCuentaAtras();


};

#endif
