#ifndef MEUFRAME_H
#define MEUFRAME_H

#include <QFrame>
#include <QPaintEvent>
#include <QPainter>
#include <QList>
#include "objeto.h"

class MeuFrame : public QFrame {
    Q_OBJECT

public:
    explicit MeuFrame(QWidget *parent = nullptr);

    void adicionarObjeto(Objeto obj);

signals:
    void mouseMoveu(QPoint pos);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    QList<Objeto> displayFile;
};

#endif // MEUFRAME_H