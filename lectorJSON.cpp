#include "lectorJSON.h"

LectorJSON::LectorJSON(QGraphicsScene *escena)
    : scene(escena)
{
}

bool LectorJSON::cargarDesdeTexto(const QString &jsonString)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "JSON inválido o mal formateado.";
        return false;
    }

    QJsonObject obj = doc.object();
    QJsonArray notas = obj["Notas"].toArray();

    for (const QJsonValue &v : notas) {
        QJsonObject n = v.toObject();

        QString nota = n["Nota"].toString();
        int octava = n["Octava"].toInt();
        double offset = n["Offset"].toDouble();   // tiempo de inicio
        double duracion = n["Duracion"].toDouble(); // duración en segundos

        // 🧠 Aquí defines cómo traducir a coordenadas (tú defines la escala)
        int anchuraNota = 20;
        int alturaPorSegundo = 100; // altura visual por segundo de duración
        int x = calcularXDesdeNota(nota, octava); // tú defines esta función
        int y = offset * alturaPorSegundo;
        int h = duracion * alturaPorSegundo;

        QRectF rect(x, y, anchuraNota, h);
        QGraphicsRectItem *item = scene->addRect(rect);
        item->setData(0, nota + QString::number(octava));
        item->setData(1, false);
    }

    return true;
}

bool LectorJSON::cargarDesdeArchivo(const QString &rutaArchivo)//desde la ruta
{
    QFile file(rutaArchivo);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "No se pudo abrir el archivo:" << rutaArchivo;
        return false;
    }

    QString contenido = QString::fromUtf8(file.readAll());
    file.close();

    return cargarDesdeTexto(contenido);
}

bool LectorJSON::cargarDesdeBD(int id, QSqlDatabase db)
{
    QSqlQuery query(db);
    query.prepare("SELECT datos_json FROM canciones WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec()) {
        qWarning() << "Error al ejecutar la consulta:" << query.lastError().text();
        return false;
    }

    if (!query.next()) {
        qWarning() << "No se encontró la canción con id:" << id;
        return false;
    }

    QString jsonString = query.value(0).toString();
    return cargarDesdeTexto(jsonString);
}

int calcularXDesdeNota(const QString &nota, int octava)
{
    static const QStringList notasPorOctava = {
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    };

    QString notaMayus = nota.trimmed().toUpper();

    int idxNota = notasPorOctava.indexOf(notaMayus);
    if (idxNota == -1) {
        qWarning() << "Nota inválida:" << notaMayus;
        return -1;
    }

    int indiceTecla = (octava * 12) + idxNota;

    const int ANCHO_TECLA = 20; // Puedes ajustar esto según tu diseño
    return indiceTecla * ANCHO_TECLA;
}
