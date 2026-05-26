#ifndef OBJETO_H
#define OBJETO_H

#include <QList>
#include <QPoint>
#include <QString>
#include "matriz.h"

class Objeto
{
public:
    QString nome;
    QList<QList<QPointF>> faces;

    Objeto(QString nome);

    void addFace(QList<QPointF> pontosDaFace);
    QPointF getCentroide() const;
    void transformar(const Matriz &m);
};

#endif // OBJETO_H