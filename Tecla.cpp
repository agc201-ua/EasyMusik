#include "Tecla.h"

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QDir>

using namespace std;

// 1: Leer del json las notas
/* JSON:
 * "Nota": "A-",
 * "Octava": 3,
 * "Offset": 30.0,
 * "Duración": 1.0
*/
QString normalizarNombreNota(const QString& nombre) {
    static QMap<QString, QString> mapa = {
        {"C", "c"}, {"C+", "#c"}, {"D-", "#c"},
        {"D", "d"}, {"D+", "#d"}, {"E-", "#d"},
        {"E", "e"},
        {"F", "f"}, {"F+", "#f"}, {"G-", "#f"},
        {"G", "g"}, {"G+", "#g"}, {"A-", "#g"},
        {"A", "a"}, {"A+", "#a"}, {"B-", "#a"},
        {"B", "b"}
    };
    return mapa.value(nombre.toUpper(), nombre.toLower());
}


Tecla::Tecla(QVector<QString> names, int octave, bool illuminated) {
    nombres = names;
    octava = octave;
    iluminada = illuminated;

    // Obtener el audio
    // Construimos el nombre del archivo, por ejemplo "E-3.wav"
    QString notaAudio = normalizarNombreNota(names.first());
    QString nombreArchivo = notaAudio + QString::number(octave) + "vH.wav";


    // Ruta al recurso
    QDir dir(QDir::currentPath());   // Obtén el directorio actual
    dir.cdUp();
    dir.cdUp();
    rutaAudio = dir.absoluteFilePath("wav_sounds/" + nombreArchivo);  // Construye la ruta del archivo
    rutaAudio = dir.absoluteFilePath("sounds/" + nombreArchivo);
    QFile file(rutaAudio);
    if (!file.exists()) {
        qDebug() << "El archivo de audio no existe en la ruta: " << rutaAudio;
        return;
    }
}

QVector<QString> Tecla::getNombres() {
    return nombres;
}

int Tecla::getOctava() {
    return octava;
}

qreal Tecla::getPosX() {
    return posX;
}

qreal Tecla::getPosY() {
    return posY;
}

qreal Tecla::getAnchura() {
    return anchura;
}

qreal Tecla::getAltura() {
    return altura;
}

void Tecla::setPosX(qreal x) {
    posX = x;
}

void Tecla::setPosY(qreal y) {
    posY = y;
}

void Tecla::setAnchura(qreal width) {
    anchura = width;
}

void Tecla::setAltura(qreal height) {
    altura = height;
}

void Tecla::setIluminada(bool iluminated) {
    iluminada = iluminated;
}

bool Tecla::getIluminada() {
    return iluminada;
}

QString Tecla::getRutaAudio() {
    return rutaAudio;
}

bool Tecla::esNegra() {
    if (nombres.size() > 1) {
        return true;
    } else{
        return false;
    }
}

void Tecla::iluminar() {
    iluminada = true;
    setBrush(QBrush(Qt::blue));
    update();
}

void Tecla::apagar() {
    iluminada = false;
    setBrush(QBrush(esNegra() ? Qt::black : Qt::white));
    update();
}

// Establecer el área de detección de colisiones
QRectF Tecla::boundingRect() const {
    return QRectF(0, 0, anchura, altura);
}

// Pintar la tecla
void Tecla::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    // Tecla iluminada
    if (iluminada) {
        painter->setBrush(QBrush(Qt::darkCyan));
        painter->setPen(QPen(Qt::black));
    }

    // Tecla blanca
    else if (!this->esNegra()) {
        painter->setBrush(Qt::white);
        painter->setPen(QPen(Qt::black));
    }

    // Tecla negra
    else {
        painter->setBrush(Qt::black);
        painter->setPen(QPen(Qt::black));
    }

    // Dibujamos el objeto gráfico de la tecla
    painter->drawRect(0, 0, anchura, altura);
}

QPainterPath Tecla::shape() const {
    QPainterPath path;
    path.addRect(boundingRect());
    return path;
}
