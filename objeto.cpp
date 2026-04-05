#include "objeto.h"

Objeto::Objeto(QString nome, Tipo tipo, QList<QPoint> pontos) {
    this->nome = nome;
    this->tipo = tipo;
    this->pontos = pontos;
}