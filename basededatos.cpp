#include "basededatos.h"

/*
    Estaria bien tener la base de datos en una carpeta separada

    Los scripts de python tambien y asi para que no se mezcle con el proyecto



*/

//Al crear la clase BaseDeDatos
BaseDeDatos::BaseDeDatos(QObject *parent) : QObject{parent} //Constructor sin nombre de la base de datos
{
    setRutaAbsoluta(); // m_rutaAbsoluta = "./EasyMusik/"
}

BaseDeDatos::BaseDeDatos(const QString &nombreBaseDeDatos, QObject *parent) : QObject{parent} //Constructor con nombre
{
    setRutaAbsoluta(); // m_rutaAbsoluta = "./EasyMusik/"
    setRutaBaseDeDatos(nombreBaseDeDatos); // m_rutaBaseDeDatos = "./EasyMusik/--nombreBaseDeDatos--" en nuestro caso
                                           // m_rutaBaseDeDatos = "./EasyMusik/canciones.db
}

void BaseDeDatos::setRutaAbsoluta() {
    QDir dir(QCoreApplication::applicationDirPath()); //Busca la base de datos en el directorio raiz del proyecto
    dir.cdUp();
    dir.cdUp();
    m_rutaAbsoluta = dir.absolutePath();
}

void BaseDeDatos::setRutaBaseDeDatos(const QString &nombreBaseDeDatos) {
    QDir dir(m_rutaAbsoluta);
    m_rutaBaseDeDatos = dir.filePath(nombreBaseDeDatos);
}

bool BaseDeDatos::ConsultaJArray(QJsonArray &notasArray, const QString &titulo, const QString &autor) {

    QSqlDatabase::removeDatabase("QSQLITE");
    QSqlDatabase BaseDeDatos = QSqlDatabase::addDatabase("QSQLITE");

    BaseDeDatos.setDatabaseName(m_rutaBaseDeDatos);

    if(!BaseDeDatos.isOpen()) {
        qWarning() << "No se pudo abrir la base de datos." << BaseDeDatos.lastError().text();
        BaseDeDatos.close();
        return false;
    }

    QSqlQuery query;
    QString string_query = QString("SELECT contenido FROM Partituras WHERE titulo = '%1' AND autor = '%2'").arg(titulo).arg(autor);

    if (!query.exec(string_query)) {
        qWarning() << "Error al ejecutar la consulta: " << query.lastError().text();
        BaseDeDatos.close();
        return false;
    }

    if (query.next()) {
        QString contenidoJson = query.value(0).toString();
        QJsonDocument doc = QJsonDocument::fromJson(contenidoJson.toUtf8());

        if (!doc.isObject()) {
            qWarning("El contenido JSON no es un objeto.");
            BaseDeDatos.close();
            return false;
        }

        notasArray = doc.object()["Notas"].toArray();

    } else {
        qWarning() << "No se encontró la partitura para título y autor dados.";
        BaseDeDatos.close();
        return false;
    }

    BaseDeDatos.close();
    QSqlDatabase::removeDatabase("Verificador"); // Limpia la conexión
    return true;
}

bool BaseDeDatos::MapaDeCanciones(QMap<int, QString> &mapa) {
    QSqlDatabase::removeDatabase("QSQLITE");
    QSqlDatabase BaseDeDatos = QSqlDatabase::addDatabase("QSQLITE");

    BaseDeDatos.setDatabaseName(m_rutaBaseDeDatos);

    if(!BaseDeDatos.isOpen()) {
        qWarning() << "No se pudo abrir la base de datos." << BaseDeDatos.lastError().text();
        BaseDeDatos.close();
        return false;
    }

    QSqlQuery query;
    QString string_query = QString("SELECT id, titulo, autor FROM Partituras");

    while(query.next()) {
        int id = query.value(0).toInt();
        QString string = QString("%1 - %2").arg(query.value(1).toString()).arg(query.value(2).toString());
        mapa.insert(id, string);
    }

    BaseDeDatos.close();
    QSqlDatabase::removeDatabase("Verificador"); // Limpia la conexión
    return true;
}





