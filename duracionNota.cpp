#include "duracionNota.h"

double DuracionNota::calcularDuracionEnSegundos(const QString &figura, int bpm)
{
    if (bpm <= 0)
        return 0.0;

    // Duración de la figura negra
    double negra = 60.0 / bpm;

    QString figuraLimpia = figura.toLower().trimmed();

    // Si es un silencio, lo tratamos igual pero quitamos el prefijo
    if (figuraLimpia.startsWith("silencio."))
        figuraLimpia = figuraLimpia.mid(QString("silencio.").length());

    return negra * factorFigura(figuraLimpia);
}

double DuracionNota::factorFigura(const QString &figura)
{
    static const QMap<QString, double> mapa = {
        {"redonda",           4.0},
        {"blanca",            2.0},
        {"negra",             1.0},
        {"corchea",           0.5},
        {"semicorchea",       0.25},
        {"fusa",              0.125},
        {"semifusa",          0.0625},
        {"negra.puntillo",    1.5},
        {"corchea.puntillo",  0.75},
        {"blanca.puntillo",   3.0}
    };

    return mapa.value(figura, 1.0); // Por defecto, negra
}
