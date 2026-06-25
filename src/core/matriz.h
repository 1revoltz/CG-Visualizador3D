#ifndef MATRIZ_H
#define MATRIZ_H

#include <QVector3D>
#include <cmath>

class Matriz
{
public:
    double m[4][4];

    Matriz();

    Matriz operator*(const Matriz &outro) const;
    QVector3D operator*(const QVector3D &p) const;


    static Matriz translacao(double dx, double dy, double dz = 0.0);
    static Matriz escala(double sx, double sy, double sz = 1.0);
    static Matriz rotacaoX(double anguloGraus);
    static Matriz rotacaoY(double anguloGraus);
    static Matriz rotacaoZ(double anguloGraus);
    static Matriz projecaoOrtogonalXY();
    static Matriz projecaoOrtogonalXZ();
    static Matriz projecaoOrtogonalZY();
    static Matriz projecaoPerspectiva(double d);
};

#endif // MATRIZ_H
