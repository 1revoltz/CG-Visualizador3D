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
    explicit MeuFrame(QWidget *parent = nullptr);

    void adicionarObjeto(Objeto obj);

    Objeto *getObjeto(int index);

    double wXmin, wXmax, wYmin, wYmax;

    int vpXmin, vpXmax, vpYmin, vpYmax;

signals:
    void mouseMoveu(QPoint pos);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QList<Objeto> displayFile;
    QPoint mundoParaTela(QPoint ptMundo);
};

#endif // MEUFRAME_H