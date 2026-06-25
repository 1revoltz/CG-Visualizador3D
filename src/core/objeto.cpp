#include "objeto.h"

Objeto::Objeto(QString nome)
{
    this->nome = nome;
}


void Objeto::addFace(QList<QVector3D> pontosDaFace)
{
    faces.append(pontosDaFace);
}


QVector3D Objeto::getCentroide() const
{
    double somaX = 0, somaY = 0, somaZ = 0;
    int totalPontos = 0;

    for (const auto &face : faces) {
        for (const QVector3D &p : face) {
            somaX += p.x();
            somaY += p.y();
            somaZ += p.z();
            totalPontos++;
        }
    }

    if (totalPontos == 0) return QVector3D(0, 0, 0);

    return QVector3D(somaX / totalPontos, somaY / totalPontos, somaZ / totalPontos);
}


void Objeto::transformar(const Matriz &m)
{
    for (int i = 0; i < faces.size(); ++i) {
        for (int j = 0; j < faces[i].size(); ++j) {
            faces[i][j] = m * faces[i][j];
        }
    }
}
