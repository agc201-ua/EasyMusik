#include "Teclado.h"

#include <QPainter>

// Calcular las dimensiones del teclado y sus teclas
void Teclado::calcularDimensiones(int anchuraPantalla) {
    // Calcular la anchura de cada tecla
    anchuraTeclaBlanca = anchuraPantalla / 45;
    anchuraTeclaNegra = (2.0 / 3.0) * anchuraTeclaBlanca;

    // Calcular la altura de cada tecla en función a la anchura
    alturaTeclaBlanca = 10 * anchuraTeclaBlanca;
    alturaTeclaNegra = 7.5 * anchuraTeclaBlanca;

    // Definir la anchura y la altura del teclado
    anchuraTeclado = anchuraPantalla;
    alturaTeclado = alturaTeclaBlanca;
}

void Teclado::posicionarTeclas(QGraphicsScene *scene, int alturaPantalla) {
    // Se inicializa la posición horizontal para las teclas
    qreal x = 0;

    // Se recorren las teclas del teclado
    for (int i=0; i<teclas.size(); i++) {
        Tecla* t = teclas[i];

        // Definir al teclado como el padre a nivel gráfico de tecla
        //t->setParentItem(this);

        // Posicionar la tecla
        if (t->esNegra()) {
            // Para que Qt sepa en que sitio de la escena se encuentra
            t->setPos(x - (anchuraTeclaNegra / 2), alturaPantalla - alturaTeclado);
            t->setZValue(1);
            // Inicializar posición y dimensión
            t->setPosX(x - (anchuraTeclaNegra / 2));
            t->setPosY(alturaPantalla - alturaTeclado);
            t->setAnchura(anchuraTeclaNegra);
            t->setAltura(alturaTeclaNegra);

        } else {
            // Para que Qt sepa en que sitio de la escena se encuentra
            t->setPos(x, alturaPantalla - alturaTeclado);
            t->setZValue(0);
            // Inicializar posición y dimensión
            t->setPosX(x);
            t->setPosY(alturaPantalla - alturaTeclado);
            t->setAnchura(anchuraTeclaBlanca);
            t->setAltura(alturaTeclaBlanca);
            // Avanzar la posición para la siguiente tecla blanca
            x += anchuraTeclaBlanca;
        }

        // Añadir la tecla a la escena
        scene->addItem(t);
    }
}

QVector<Tecla*> Teclado::getTeclas() {
    return teclas;
}

Tecla* Teclado::getTecla(QVector<QString> nombres) {
    Tecla* tecla = nullptr;

    for (Tecla* t : teclas) {
        if (t->getNombres() == nombres) {
            tecla = t;
            break;
        }
    }

    return tecla;
}

qreal Teclado::getAnchuraTeclado() {
    return anchuraTeclado;
}

qreal Teclado::getAlturaTeclado() {
    return alturaTeclado;
}

qreal Teclado::getAnchuraTeclaBlanca() {
    return anchuraTeclaBlanca;
}

qreal Teclado::getAlturaTeclaBlanca() {
    return alturaTeclaBlanca;
}

qreal Teclado::getAnchuraTeclaNegra() {
    return anchuraTeclaNegra;
}

qreal Teclado::getAlturaTeclaNegra() {
    return alturaTeclaNegra;
}

// Constructor
Teclado::Teclado(QGraphicsScene *scene, int anchuraPantalla, int alturaPantalla) {

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
    posicionarTeclas(scene, alturaPantalla);
}

Teclado::~Teclado() {
    for (Tecla* t : teclas) {
        delete t;
    }
    teclas.clear();
}
