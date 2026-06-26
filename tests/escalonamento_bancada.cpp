#include "matriz.h"
#include "numeros.h"
#include "objeto.h"

#include <QCoreApplication>
#include <QDebug>
#include <QList>
#include <QLocale>
#include <QVector3D>

#include <algorithm>
#include <cmath>

namespace {
constexpr double EPS = 1e-6;

bool perto(double a, double b)
{
    return std::abs(a - b) <= EPS;
}

bool perto(const QVector3D &a, const QVector3D &b)
{
    return perto(a.x(), b.x()) && perto(a.y(), b.y()) && perto(a.z(), b.z());
}

struct Caixa {
    QVector3D minimo;
    QVector3D maximo;
};

Caixa calcularCaixa(const Objeto &objeto)
{
    Caixa caixa{QVector3D(), QVector3D()};
    bool primeiroPonto = true;

    for (const QList<QVector3D> &face : objeto.faces) {
        for (const QVector3D &p : face) {
            if (primeiroPonto) {
                caixa.minimo = p;
                caixa.maximo = p;
                primeiroPonto = false;
                continue;
            }

            caixa.minimo.setX(std::min(caixa.minimo.x(), p.x()));
            caixa.minimo.setY(std::min(caixa.minimo.y(), p.y()));
            caixa.minimo.setZ(std::min(caixa.minimo.z(), p.z()));
            caixa.maximo.setX(std::max(caixa.maximo.x(), p.x()));
            caixa.maximo.setY(std::max(caixa.maximo.y(), p.y()));
            caixa.maximo.setZ(std::max(caixa.maximo.z(), p.z()));
        }
    }

    return caixa;
}

QVector3D dimensoes(const Caixa &caixa)
{
    return caixa.maximo - caixa.minimo;
}

Objeto criarCaixa(const QVector3D &centro, double largura, double altura, double profundidade)
{
    Objeto objeto("caixa_teste");
    const double hx = largura / 2.0;
    const double hy = altura / 2.0;
    const double hz = profundidade / 2.0;

    objeto.addFace({
        centro + QVector3D(-hx, -hy, -hz),
        centro + QVector3D( hx, -hy, -hz),
        centro + QVector3D( hx,  hy, -hz),
        centro + QVector3D(-hx,  hy, -hz),
        centro + QVector3D(-hx, -hy,  hz),
        centro + QVector3D( hx, -hy,  hz),
        centro + QVector3D( hx,  hy,  hz),
        centro + QVector3D(-hx,  hy,  hz)
    });

    return objeto;
}

void aplicarEscalaNoCentroide(Objeto &objeto, double sx, double sy, double sz)
{
    const QVector3D centro = objeto.getCentroide();
    const Matriz tIda = Matriz::translacao(-centro.x(), -centro.y(), -centro.z());
    const Matriz escala = Matriz::escala(sx, sy, sz);
    const Matriz tVolta = Matriz::translacao(centro.x(), centro.y(), centro.z());

    objeto.transformar(tVolta * escala * tIda);
}

void verificar(bool condicao, const QString &mensagem, int &falhas)
{
    if (!condicao) {
        qCritical().noquote() << "FALHA:" << mensagem;
        ++falhas;
    }
}
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    Q_UNUSED(app);

    QLocale::setDefault(QLocale(QLocale::Portuguese, QLocale::Brazil));

    int falhas = 0;
    double valor = 0.0;

    verificar(converterDoubleEntrada("0,5", valor) && perto(valor, 0.5),
              "entrada 0,5 deve ser interpretada como 0.5",
              falhas);
    verificar(converterDoubleEntrada("0.5", valor) && perto(valor, 0.5),
              "entrada 0.5 deve ser interpretada como 0.5",
              falhas);
    verificar(!converterDoubleEntrada("abc", valor),
              "entrada invalida nao pode virar 0 silenciosamente",
              falhas);

    Objeto cubo = criarCaixa(QVector3D(0.0, 0.0, 0.0), 2.0, 2.0, 2.0);
    const QVector3D centroCubo = cubo.getCentroide();
    aplicarEscalaNoCentroide(cubo, 0.5, 0.5, 0.5);
    verificar(perto(cubo.getCentroide(), centroCubo),
              "escala uniforme deve preservar o centroide",
              falhas);
    verificar(perto(dimensoes(calcularCaixa(cubo)), QVector3D(1.0, 1.0, 1.0)),
              "escala 0.5 deve reduzir cada dimensao pela metade",
              falhas);

    Objeto caixa = criarCaixa(QVector3D(10.0, -4.0, 2.0), 8.0, 4.0, 10.0);
    const QVector3D centroCaixa = caixa.getCentroide();
    aplicarEscalaNoCentroide(caixa, 0.25, 0.5, 0.1);
    verificar(perto(caixa.getCentroide(), centroCaixa),
              "escala nao uniforme deve preservar o centroide",
              falhas);
    verificar(perto(dimensoes(calcularCaixa(caixa)), QVector3D(2.0, 2.0, 1.0)),
              "escala nao uniforme deve multiplicar cada eixo pelo respectivo fator",
              falhas);

    Objeto achatada = criarCaixa(QVector3D(5.0, 5.0, 5.0), 4.0, 4.0, 4.0);
    aplicarEscalaNoCentroide(achatada, 0.0, 1.0, 1.0);
    verificar(perto(dimensoes(calcularCaixa(achatada)).x(), 0.0),
              "fator 0 colapsa o eixo; por isso a UI deve recusar esse valor",
              falhas);

    if (falhas == 0) {
        qInfo().noquote() << "Bancada de escalonamento: todos os testes passaram.";
    }

    return falhas == 0 ? 0 : 1;
}