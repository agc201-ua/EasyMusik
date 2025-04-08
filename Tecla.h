#ifndef TECLA_H
#define TECLA_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QBrush>

class Tecla : public QObject, public QGraphicsRectItem {
    Q_OBJECT

    private:
        QVector<QString> nombres;
        int octava;
        qreal posX;
        qreal posY;
        qreal anchura;
        qreal altura;
        bool iluminada;

    public:
        // Constructor
        Tecla(QVector<QString> names, int octave, bool illuminated = false);

        // Getters y setters
        QVector<QString> getNombres();
        int getOctava();
        qreal getPosX();
        qreal getPosY();
        qreal getAnchura();
        qreal getAltura();
        void setPosX(qreal x);
        void setPosY(qreal y);
        void setAnchura(qreal width);
        void setAltura(qreal height);
        void setIluminada(bool iluminated);

        // Métodos adicionalesss
        bool esNegra();
        void iluminar();
        void apagar();
        void probar();

        // Métodos del demonio para que no me detecte esto como una clase abstracta
        // Todo sea por QGraphicsItem
        QRectF boundingRect() const override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
        QPainterPath shape() const override;
};

#endif
