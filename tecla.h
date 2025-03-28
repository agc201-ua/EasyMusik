#ifndef TECLA_H
#define TECLA_H
#include <QString>

class Tecla {
public:
    // Constructor
    Tecla(QString codigo, float longitud, float duracion, float posX, float posY, float offset);

    // Getters y setters
    QString getCodigo() const;
    float getLongitud() const;
    float getDuracion() const;
    float getPosX() const;
    float getPosY() const;
    float getOffset() const;

    void setPosX(float x);
    void setPosY(float y);

    void mover(float velocidad);

private:
    QString codigo;
    float longitud;
    float duracion;
    float posX;
    float posY;
    float offset;
};

#endif // TECLA_H
