#include "Teclado.h"

#include <QPainter>

void Teclado::calcularDimensiones(int anchuraPantalla) {
    // Calcular la anchura de cada tecla
    anchuraTeclaBlanca = anchuraPantalla / 45;
    anchuraTeclaNegra = (2.0 / 3.0) * anchuraTeclaBlanca;

    // Calcular la altura de cada tecla en función a la anchura
    alturaTeclaBlanca = 10 * anchuraTeclaBlanca;
    alturaTeclaNegra = 7.5 * anchuraTeclaBlanca;

    // Definir la anchura y la altura del teclado
    anchuraTeclado = anchuraPantalla;
    alturaTeclado = static_cast<int>(alturaTeclaBlanca);
}

Teclado::Teclado(QGraphicsScene *scene, int anchuraPantalla) {

    // Se calculan las dimensiones del teclado
    calcularDimensiones(anchuraPantalla);

    // Se inicializan las teclas
    // + sostenido
    // - bemol
    teclas = {
        new Tecla({"E"}, 1),
        new Tecla({"F"}, 1),
        new Tecla({"F+", "G-"}, 1),
        new Tecla({"G"}, 1),
        new Tecla({"G+", "A-"}, 1),
        new Tecla({"A"}, 1),
        new Tecla({"A+", "B-"}, 1),
        new Tecla({"B"}, 1),
        new Tecla({"C"}, 2),
        new Tecla({"C+", "D-"}, 2),
        new Tecla({"D"}, 2),
        new Tecla({"D+", "E-"}, 2),
        new Tecla({"E"}, 2),
        new Tecla({"F"}, 2),
        new Tecla({"F+", "G-"}, 2),
        new Tecla({"G"}, 2),
        new Tecla({"G+", "A-"}, 2),
        new Tecla({"A"}, 2),
        new Tecla({"A+", "B-"}, 2),
        new Tecla({"B"}, 2),
        new Tecla({"C"}, 3),
        new Tecla({"C+", "D-"}, 3),
        new Tecla({"D"}, 3),
        new Tecla({"D+", "E-"}, 3),
        new Tecla({"E"}, 3),
        new Tecla({"F"}, 3),
        new Tecla({"F+", "G-"}, 3),
        new Tecla({"G"}, 3),
        new Tecla({"G+", "A-"}, 3),
        new Tecla({"A"}, 3),
        new Tecla({"A+", "B-"}, 3),
        new Tecla({"B"}, 3),
        new Tecla({"C"}, 4),
        new Tecla({"C+", "D-"}, 4),
        new Tecla({"D"}, 4),
        new Tecla({"D+", "E-"}, 4),
        new Tecla({"E"}, 4),
        new Tecla({"F"}, 4),
        new Tecla({"F+", "G-"}, 4),
        new Tecla({"G"}, 4),
        new Tecla({"G+", "A-"}, 4),
        new Tecla({"A"}, 4),
        new Tecla({"A+", "B-"}, 4),
        new Tecla({"B"}, 4),
        new Tecla({"C"}, 5),
        new Tecla({"C+", "D-"}, 5),
        new Tecla({"D"}, 5),
        new Tecla({"D+", "E-"}, 5),
        new Tecla({"E"}, 5),
        new Tecla({"F"}, 5),
        new Tecla({"F+", "G-"}, 5),
        new Tecla({"G"}, 5),
        new Tecla({"G+", "A-"}, 5),
        new Tecla({"A"}, 5),
        new Tecla({"A+", "B-"}, 5),
        new Tecla({"B"}, 5),
        new Tecla({"C"}, 6),
        new Tecla({"C+", "D-"}, 6),
        new Tecla({"D"}, 6),
        new Tecla({"D+", "E-"}, 6),
        new Tecla({"E"}, 6),
        new Tecla({"F"}, 6),
        new Tecla({"F+", "G-"}, 6),
        new Tecla({"G"}, 6),
        new Tecla({"G+", "A-"}, 6),
        new Tecla({"A"}, 6),
        new Tecla({"A+", "B-"}, 6),
        new Tecla({"B"}, 6),
        new Tecla({"C"}, 7),
        new Tecla({"C+", "D-"}, 7),
        new Tecla({"D"}, 7),
        new Tecla({"D+", "E-"}, 7),
        new Tecla({"E"}, 7),
        new Tecla({"F"}, 7),
        new Tecla({"F+", "G-"}, 7),
        new Tecla({"G"}, 7)
    };

    // Dibujar las teclas
    pintarTeclado(scene);
}

void Teclado::pintarTeclado(QGraphicsScene *scene) {
    // Se inicializa la posición horizontal para las teclas
    qreal x = 0;

    // Se recorren las teclas del teclado
    for (int i=0; i<teclas.size(); i++) {
        Tecla* t = teclas[i];

        // Definir al teclado como el padre a nivel gráfico de tecla
        t->setParentItem(this);

        // Crear el objeto gráfico de la tecla
        QGraphicsRectItem* tecla = new QGraphicsRectItem(x, 0,
                                                         t->esNegra() ? anchuraTeclaNegra : anchuraTeclaBlanca,
                                                         t->esNegra() ? alturaTeclaNegra : alturaTeclaBlanca);

        // Posicionar la tecla
        if (t->esNegra()) {
            t->setPosicion(x - (anchuraTeclaNegra / 2));
        } else {
            t->setPosicion(x);
            x += anchuraTeclaBlanca;  // Avanzamos la posición para la siguiente tecla blanca
        }

        // Añadir la tecla a la escena
        scene->addItem(tecla);
    }
}

// Esto define el área del teclado en el que se puede detectar la colisión
QRectF Teclado::boundingRect() const {
    return QRectF(0, 0, anchuraTeclado, alturaTeclado);
}

void Teclado::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    // Primero dibujamos el fondo del teclado (el área donde estarán las teclas)
    painter->setBrush(Qt::transparent);
    painter->setPen(Qt::NoPen);
    painter->drawRect(0, 0, anchuraTeclado, alturaTeclado);

    // Primero, dibujamos las teclas blancas
    for (Tecla* t : teclas) {
        // Comprobamos si la tecla es blanca
        if (!t->esNegra()) {
            painter->setBrush(Qt::white);  // Tecla blanca
            painter->setPen(QPen(Qt::black));

            // Establecer el rectángulo para dibujar la tecla blanca
            qreal x = t->getPosicion();  // Obtén la posición horizontal de la tecla
            qreal y = 0;  // Las teclas empiezan en la parte superior del teclado
            qreal width = anchuraTeclaBlanca;
            qreal height = alturaTeclaBlanca;

            // Dibujamos el rectángulo de la tecla blanca
            painter->drawRect(x, y, width, height);
        }
    }

    // Después, dibujamos las teclas negras (encima de las blancas)
    for (Tecla* t : teclas) {
        // Comprobamos si la tecla es negra
        if (t->esNegra()) {
            painter->setBrush(Qt::black);  // Tecla negra
            painter->setPen(QPen(Qt::black));

            // Establecer el rectángulo para dibujar la tecla negra
            qreal x = t->getPosicion();  // Obtén la posición horizontal de la tecla
            qreal y = 0;  // Las teclas empiezan en la parte superior del teclado
            qreal width = anchuraTeclaNegra;
            qreal height = alturaTeclaNegra;

            // Dibujamos el rectángulo de la tecla negra
            painter->drawRect(x, y, width, height);
        }
    }
}

Teclado::~Teclado() {
    for (Tecla* t : teclas) {
        delete t;
    }
    teclas.clear();
}
