#include "objeto.h"

Objeto::Objeto(QString nome, Tipo tipo, QList<QPoint> pontos) {
    this->nome = nome;
    this->tipo = tipo;
    this->pontos = pontos;
}

QPoint Objeto::getCentroide() const{
    if (pontos.isEmpty()) return QPoint(0, 0);

    double somaX = 0;
    double somaY = 0;

    for(const QPoint& p : pontos){
        somaX += p.x();
        somaY += p.y();
    }

    // media das cordenadas
    return QPoint(round(somaX / pontos.size()), round(somaY / pontos.size()));
}

void Objeto::transformar(const Matriz& m) {
    for (int i = 0; i < pontos.size(); ++i) {
        pontos[i] = m * pontos[i];
    }
}