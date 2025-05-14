#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "MenuPrincipal.h"
#include "Cancion.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void mostrarCancion(const QString &titulo, const QString &artista);
    void volverAlMenu();
    void cerrarAplicacion();

private:
    QStackedWidget *stackedWidget;
    MenuPrincipal *menuPrincipal;
    Cancion *cancionActual;

    // Elementos de la interfaz
    QGraphicsScene* scene;
    QGraphicsView* view;
};

#endif // MAINWINDOW_H
