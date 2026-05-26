#include "matriz.h"

Matriz::Matriz()
{
    // inicializa como matriz identidade
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            m[i][j] = (i == j) ? 1.0 : 0.0;
}

Matriz Matriz::operator*(const Matriz &outro) const
{
    Matriz res;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            res.m[i][j] = 0;
            for (int k = 0; k < 3; k++) {
                res.m[i][j] += this->m[i][k] * outro.m[k][j];
            }
        }
    }
    return res;
}

QPointF Matriz::operator*(const QPointF &p) const
{
    double nx = m[0][0] * p.x() + m[0][1] * p.y() + m[0][2] * 1;
    double ny = m[1][0] * p.x() + m[1][1] * p.y() + m[1][2] * 1;
    return QPointF(nx, ny);
}

Matriz Matriz::translacao(double dx, double dy)
{
    Matriz res;
    res.m[0][2] = dx;
    res.m[1][2] = dy;
    return res;
}

Matriz Matriz::escala(double sx, double sy)
{
    Matriz res;
    res.m[0][0] = sx;
    res.m[1][1] = sy;
    return res;
}

Matriz Matriz::rotacao(double anguloGraus)
{
    Matriz res;
    double rad = anguloGraus * M_PI / 180.0;
    res.m[0][0] = cos(rad);
    res.m[0][1] = -sin(rad);
    res.m[1][0] = sin(rad);
    res.m[1][1] = cos(rad);
    return res;
}