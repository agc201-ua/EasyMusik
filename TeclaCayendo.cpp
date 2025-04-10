#include "TeclaCayendo.h"

TeclaCayendo::TeclaCayendo(QString codigo, float longitud, float duracion, float posX, float posY, float offset)
    : codigo(codigo), longitud(longitud), duracion(duracion), posX(posX), posY(posY), offset(offset) {}

QString TeclaCayendo::getCodigo() const {
    return codigo;
}

float TeclaCayendo::getLongitud() const {
    return longitud;
}

float TeclaCayendo::getDuracion() const {
    return duracion;
}

float TeclaCayendo::getPosX() const {
    return posX;
}

float TeclaCayendo::getPosY() const {
    return posY;
}

float TeclaCayendo::getOffset() const {
    return offset;
}

void TeclaCayendo::setPosX(float x) {
    posX = x;
}

void TeclaCayendo::setPosY(float y) {
    posY = y;
}

void TeclaCayendo::mover(float velocidad) {
    posY += velocidad;
}
