#ifndef OBJETO_H
#define OBJETO_H

#include <QString>
#include <QList>
#include <QPoint>
#include "matriz.h"

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

    QPoint getCentroide() const;

    void transformar(const Matriz& m);
};

#endif // OBJETO_H