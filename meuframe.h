#ifndef MEUFRAME_H
#define MEUFRAME_H

#include <QFrame>
#include <QList>
#include <QPaintEvent>
#include <QPainter>
#include "objeto.h"

class MeuFrame : public QFrame
{
    Q_OBJECT

public:
    enum class VistaOrtogonal {
        FrontalXY,
        SuperiorXZ,
        LateralZY,
        Perspectiva
    };

    //zonas
    static const int INSIDE = 0; // 0000
    static const int LEFT = 1;   // 0001
    static const int RIGHT = 2;  // 0010
    static const int BOTTOM = 4; // 0100
    static const int TOP = 8;    // 1000

    explicit MeuFrame(QWidget *parent = nullptr);

    void adicionarObjeto(Objeto obj);

    Objeto *getObjeto(int index);
    void setVistaOrtogonal(VistaOrtogonal vista);
    void transformarMundo(const Matriz &m);

    double wXmin, wXmax, wYmin, wYmax;

    int vpXmin, vpXmax, vpYmin, vpYmax;

signals:
    void mouseMoveu(QPoint pos);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QList<Objeto> displayFile;
    VistaOrtogonal vistaOrtogonal = VistaOrtogonal::FrontalXY;
    Matriz matrizProjecao = Matriz::projecaoOrtogonalXY();
    QPointF projetar(const QVector3D &ponto) const;
    QPoint mundoParaTela(QPointF ptMundo);

    //clipping
    int calcularCodigo(QPointF p);
    bool cohenSutherlandClip(QPointF &p1, QPointF &p2);
};

#endif // MEUFRAME_H
