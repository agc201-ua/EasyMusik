#include "tecla.h"

Tecla::Tecla(QString codigo, float longitud, float duracion, float posX, float posY, float offset)
    : codigo(codigo), longitud(longitud), duracion(duracion), posX(posX), posY(posY), offset(offset) {}

QString Tecla::getCodigo() const {
    return codigo;
}

float Tecla::getLongitud() const {
    return longitud;
}

float Tecla::getDuracion() const {
    return duracion;
}

float Tecla::getPosX() const {
    return posX;
}

float Tecla::getPosY() const {
    return posY;
}

float Tecla::getOffset() const {
    return offset;
}

void Tecla::setPosX(float x) {
    posX = x;
}

void Tecla::setPosY(float y) {
    posY = y;
}

void Tecla::mover(float velocidad) {
    posY += velocidad;
}
