#ifndef LECTORJSON_H
#define LECTORJSON_H

#include <QGraphicsScene>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

const ANCHO_TECLA = 20;

class LectorJSON
{
public:
    explicit LectorJSON(QGraphicsScene *escena);

    bool cargarDesdeTexto(const QString &jsonString);
    bool cargarDesdeArchivo(const QString &rutaArchivo);
    bool cargarDesdeBD(int id, QSqlDatabase db);
    int calcularXDesdeNota(const QString &nota, int octava);

private:

    QGraphicsScene *scene;
};

#endif // LECTORJSON_H
