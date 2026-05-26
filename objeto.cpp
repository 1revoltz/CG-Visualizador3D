#include "objeto.h"

Objeto::Objeto(QString nome)
{
    this->nome = nome;
}

void Objeto::addFace(QList<QPointF> pontosDaFace)
{
    faces.append(pontosDaFace);
}

QPointF Objeto::getCentroide() const
{
    double somaX = 0;
    double somaY = 0;
    int totalPontos = 0;

    for (const auto &face : faces) {
        for (const QPointF &p : face) {
            somaX += p.x();
            somaY += p.y();
            totalPontos++;
        }
    }

    if (totalPontos == 0) return QPointF(0, 0);

    return QPointF(somaX / totalPontos, somaY / totalPontos);
}

void Objeto::transformar(const Matriz &m)
{
    for (int i = 0; i < faces.size(); ++i) {
        for (int j = 0; j < faces[i].size(); ++j) {
            faces[i][j] = m * faces[i][j];
        }
    }
}