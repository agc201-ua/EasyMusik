#ifndef TECLA_H
#define TECLA_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QBrush>

class Tecla : public QGraphicsRectItem {
    private:
        QVector<QString> nombres;
        int octava;
        qreal posicion;

    public:
        Tecla(QVector<QString> names, int octave);

        // Getters y setter
        QVector<QString> getNombres();
        int getOctava();
        qreal getPosicion();
        void setPosicion(qreal position);

        // Métodos adicionalesss
        bool esNegra();
        void iluminar();
        void apagar();
        void probar();

        // Métodos del demonio para que no me detecte esto como una clase abstracta

};

#endif
