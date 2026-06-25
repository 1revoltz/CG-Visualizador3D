#ifndef OBJETO_H
#define OBJETO_H

#include <QList>
#include <QVector3D> 
#include <QString>
#include "matriz.h"

class Objeto
{
public:
    QString nome;
    QList<QList<QVector3D>> faces; // alterado de QPointF para QVector3D

    Objeto(QString nome);

    void addFace(QList<QVector3D> pontosDaFace); // atualizada a assinatura para QVector3D
    QVector3D getCentroide() const;               // agora retorna um QVector3D
    void transformar(const Matriz &m);
};

#endif // OBJETO_H
