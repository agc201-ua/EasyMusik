#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "MenuPrincipal.h"
#include "Cancion.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    QStackedWidget *stackedWidget;
    MenuPrincipal *menuPrincipal;
    Cancion *cancionActual;
    QGraphicsScene* scene;
    QGraphicsView* view;

private slots:
    void mostrarCancion(const QString &titulo, const QString &artista);
    void volverAlMenu();
    void cerrarAplicacion();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
};

#endif
