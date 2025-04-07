#ifndef TECLADO_H
#define TECLADO_H

#include "Tecla.h"

#include <QGuiApplication>
#include <QScreen>
#include <QGraphicsItem>
#include <QList>
#include <QPen>

class Teclado : public QGraphicsItem {
    private:
        // Atributos
        QVector<Tecla*> teclas;
        int anchuraTeclado;
        int alturaTeclado;
        qreal anchuraTeclaBlanca;
        qreal alturaTeclaBlanca;
        qreal anchuraTeclaNegra;
        qreal alturaTeclaNegra;
        // Métodos
        void calcularDimensiones(int anchuraPantalla);
    public:
        Teclado(QGraphicsScene* scene, int anchuraPantalla);
        ~Teclado();
        void pintarTeclado(QGraphicsScene* scene);

        // Métodos del demonio para que no me detecte esto como una clase abstracta
        // Todo sea por QGraphicsItem
        QRectF boundingRect() const override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};
#endif
