#include "Tecla.h"

#include <QGraphicsScene>
#include <QGraphicsView>

using namespace std;

// 1: Leer del json las notas
/* JSON:
 * "Nota": "A-",
 * "Octava": 3,
 * "Offset": 30.0,
 * "Duración": 1.0
*/

Tecla::Tecla(QVector<QString> names, int octave) {
    nombres = names;
    octava = octave;
}

QVector<QString> Tecla::getNombres() {
    return nombres;
}

int Tecla::getOctava() {
    return octava;
}

qreal Tecla::getPosicion() {
    return posicion;
}

void Tecla::setPosicion(qreal position) {
    posicion = position;
}

bool Tecla::esNegra() {
    if (nombres.size() > 1) {
        return true;
    } else{
        return false;
    }
}

void Tecla::iluminar() {
    setBrush(QBrush(Qt::blue));
}

void Tecla::apagar() {
    setBrush(QBrush(esNegra() ? Qt::black : Qt::white));
}
