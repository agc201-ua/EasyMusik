#ifndef DURACIONNOTA_H
#define DURACIONNOTA_H

#include <QString>
#include <QMap>

class DuracionNota
{
public:
    static double calcularDuracionEnSegundos(const QString &figura, int bpm);

private:
    static double factorFigura(const QString &figuraBase);
};

#endif // DURACIONNOTA_H
