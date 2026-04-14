#include "meuframe.h"
#include <QMouseEvent>

MeuFrame::MeuFrame(QWidget *parent) : QFrame(parent) {
    setFrameShape(QFrame::Box);
    setStyleSheet("background-color: #FFFFFF; border: 2px solid #A0A0A0;");
    setFixedSize(800, 600);

    setMouseTracking(true);
}

void MeuFrame::adicionarObjeto(Objeto obj){
    displayFile.append(obj);
    update();
}

void MeuFrame::paintEvent(QPaintEvent *event){
    QFrame::paintEvent(event);

    QPainter painter(this);

    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);

    for(int i=0; i<displayFile.size(); i++){
        Objeto obj = displayFile[i];

        if(obj.tipo == Objeto::PONTO){
            painter.drawPoint(obj.pontos[0]);
        }
        else if(obj.tipo == Objeto::RETA){
            painter.drawLine(obj.pontos[0], obj.pontos[1]);
        }
        else if(obj.tipo == Objeto::POLIGONO){
            painter.drawPolygon(obj.pontos.data(), obj.pontos.size());
        }
    }

}

void MeuFrame::mouseMoveEvent(QMouseEvent *event) {

    emit mouseMoveu(event->pos());
}