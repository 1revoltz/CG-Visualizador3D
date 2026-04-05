#ifndef OBJETO_H
#define OBJETO_H

#include <QString>
#include <QList>
#include <QPoint>

class Objeto {
public:

    enum Tipo {
        PONTO,
        RETA,
        POLIGONO
    };

    Objeto(QString nome, Tipo tipo, QList<QPoint> pontos);

    QString nome;
    Tipo tipo;
    QList<QPoint> pontos;
};

#endif // OBJETO_H