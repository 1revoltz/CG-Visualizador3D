#ifndef MATRIZ_H
#define MATRIZ_H

#include <QPoint>
#include <cmath>

class Matriz
{
public:
    double m[3][3];

    Matriz();

    Matriz operator*(const Matriz &outro) const;

    QPointF operator*(const QPointF &p) const;

    static Matriz translacao(double dx, double dy);
    static Matriz escala(double sx, double sy);
    static Matriz rotacao(double anguloGraus);
};

#endif // MATRIZ_H