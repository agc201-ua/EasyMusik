#ifndef BASEDEDATOS_H
#define BASEDEDATOS_H

#include <QObject>
#include <QDir>
#include <QFileInfo>
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>


class BaseDeDatos : public QObject
{
    Q_OBJECT
public:
    explicit BaseDeDatos(QObject *parent = nullptr);

    BaseDeDatos(const QString &nombreBaseDeDatos = nullptr, QObject *parent = nullptr);



    void setRutaBaseDeDatos(const QString &nombreBaseDeDatos);

    void setRutaAbsoluta(); // Cuando se quiere sacar la ruta

    bool ConsultaJArray(QJsonArray &Array, const QString &titulo, const QString &autor);

    bool MapaDeCanciones(QMap<int, QString> &mapa);

private:
    QString m_rutaAbsoluta;
    QString m_rutaBaseDeDatos;

signals:
};

#endif // BASEDEDATOS_H
