#ifndef TECLADO_H
#define TECLADO_H

#include "Tecla.h"

#include <QGuiApplication>
#include <QScreen>
#include <QGraphicsItem>
#include <QList>
#include <QPen>

class Teclado {

    private:
        // Atributos
        QVector<Tecla*> teclas;
        qreal anchuraTeclado;
        qreal alturaTeclado;
        qreal anchuraTeclaBlanca;
        qreal alturaTeclaBlanca;
        qreal anchuraTeclaNegra;
        qreal alturaTeclaNegra;

        // Métodos
        void calcularDimensiones(int anchuraPantalla);
        void posicionarTeclas(QGraphicsScene* scene, int alturaPantalla);

    public:
        // Constructor y destructor
        Teclado(QGraphicsScene* scene, int anchuraPantalla, int alturaPantalla);
        ~Teclado();

        // Getters
        QVector<Tecla*> getTeclas();
        Tecla* getTecla(QVector<QString> nombres);
        qreal getAnchuraTeclado();
        qreal getAlturaTeclado();
        qreal getAnchuraTeclaBlanca();
        qreal getAlturaTeclaBlanca();
        qreal getAnchuraTeclaNegra();
        qreal getAlturaTeclaNegra();
};
#endif
