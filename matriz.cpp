#include "matriz.h"

Matriz::Matriz() {
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            m[i][j] = (i == j) ? 1.0 : 0.0;
}

Matriz Matriz::operator*(const Matriz &outro) const {
    Matriz res;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            res.m[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                res.m[i][j] += this->m[i][k] * outro.m[k][j];
            }
        }
    }
    return res;
}

QVector3D Matriz::operator*(const QVector3D &p) const {
    double nx = m[0][0]*p.x() + m[0][1]*p.y() + m[0][2]*p.z() + m[0][3]*1.0;
    double ny = m[1][0]*p.x() + m[1][1]*p.y() + m[1][2]*p.z() + m[1][3]*1.0;
    double nz = m[2][0]*p.x() + m[2][1]*p.y() + m[2][2]*p.z() + m[2][3]*1.0;
    double nw = m[3][0]*p.x() + m[3][1]*p.y() + m[3][2]*p.z() + m[3][3]*1.0;

    // Tratamento de perspectiva/coordenada homogênea genérica
    if (nw != 1.0 && nw != 0.0) {
        return QVector3D(nx / nw, ny / nw, nz / nw);
    }
    return QVector3D(nx, ny, nz);
}

Matriz Matriz::translacao(double dx, double dy, double dz)
{
    Matriz res;
    res.m[0][3] = dx;
    res.m[1][3] = dy;
    res.m[2][3] = dz;
    return res;
}

Matriz Matriz::escala(double sx, double sy, double sz)
{
    Matriz res;
    res.m[0][0] = sx;
    res.m[1][1] = sy;
    res.m[2][2] = sz;
    return res;
}

Matriz Matriz::rotacaoX(double anguloGraus)
{
    Matriz res;
    double rad = anguloGraus * M_PI / 180.0;
    res.m[1][1] = cos(rad);
    res.m[1][2] = -sin(rad);
    res.m[2][1] = sin(rad);
    res.m[2][2] = cos(rad);
    return res;
}

Matriz Matriz::rotacaoY(double anguloGraus)
{
    Matriz res;
    double rad = anguloGraus * M_PI / 180.0;
    res.m[0][0] = cos(rad);
    res.m[0][2] = sin(rad);
    res.m[2][0] = -sin(rad);
    res.m[2][2] = cos(rad);
    return res;
}

Matriz Matriz::rotacaoZ(double anguloGraus)
{
    Matriz res;
    double rad = anguloGraus * M_PI / 180.0;
    res.m[0][0] = cos(rad);
    res.m[0][1] = -sin(rad);
    res.m[1][0] = sin(rad);
    res.m[1][1] = cos(rad);
    return res;
}

Matriz Matriz::projecaoOrtogonalXY()
{
    Matriz res;
    res.m[2][2] = 0.0;
    return res;
}

Matriz Matriz::projecaoOrtogonalXZ()
{
    Matriz res;
    res.m[0][0] = 1.0;
    res.m[0][1] = 0.0;
    res.m[0][2] = 0.0;
    res.m[1][0] = 0.0;
    res.m[1][1] = 0.0;
    res.m[1][2] = 1.0;
    res.m[2][0] = 0.0;
    res.m[2][1] = 0.0;
    res.m[2][2] = 0.0;
    return res;
}

Matriz Matriz::projecaoOrtogonalZY()
{
    Matriz res;
    res.m[0][0] = 0.0;
    res.m[0][1] = 0.0;
    res.m[0][2] = 1.0;
    res.m[1][0] = 0.0;
    res.m[1][1] = 1.0;
    res.m[1][2] = 0.0;
    res.m[2][0] = 0.0;
    res.m[2][1] = 0.0;
    res.m[2][2] = 0.0;
    return res;
}
