#include "meuframe.h"
#include <QMouseEvent>

MeuFrame::MeuFrame(QWidget *parent)
    : QFrame(parent)
{
    setFrameShape(QFrame::Box);
    setStyleSheet("background-color: #FFFFFF; border: 2px solid #A0A0A0;");
    setFixedSize(800, 600);

    setMouseTracking(true);

    wXmin = 0.0;
    wYmin = 0.0;
    wXmax = 800.0;
    wYmax = 600.0;

    vpXmin = 0;
    vpYmin = 0;
    vpXmax = 800;
    vpYmax = 600;
}

void MeuFrame::adicionarObjeto(Objeto obj){

    for (int i = 0; i < obj.pontos.size(); i++) {
        int novoX = obj.pontos[i].x() * 2;
        int novoY = obj.pontos[i].y() * 2;
        obj.pontos[i] = QPoint(novoX, novoY);
    }

    displayFile.append(obj);
    update();
}


void MeuFrame::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);

    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);

    for(int i = 0; i < displayFile.size(); i++){
        Objeto obj = displayFile[i];

        // 1. Criamos uma lista temporária para guardar os pontos convertidos
        QList<QPoint> ptsTela;
        for (int j = 0; j < obj.pontos.size(); j++) {
            ptsTela.append(mundoParaTela(obj.pontos[j]));
        }

        // 2. Desenhamos usando a lista 'ptsTela' em vez de 'obj.pontos'
        if(obj.tipo == Objeto::PONTO){
            painter.drawPoint(ptsTela[0]);
        }
        else if(obj.tipo == Objeto::RETA){
            painter.drawLine(ptsTela[0], ptsTela[1]);
        }
        else if(obj.tipo == Objeto::POLIGONO){
            painter.drawPolygon(ptsTela.data(), ptsTela.size());
        }
    }
}


void MeuFrame::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoveu(event->pos());
}

Objeto *MeuFrame::getObjeto(int index)
{
    if (index >= 0 && index < displayFile.size()) {
        return &displayFile[index];
    }
    return nullptr;
}

QPoint MeuFrame::mundoParaTela(QPoint ptMundo) {
    double xw = ptMundo.x();
    double yw = ptMundo.y();

    // Pequena verificação para evitar divisão por zero se a window tiver tamanho zero
    if (wXmax == wXmin) wXmax += 1;
    if (wYmax == wYmin) wYmax += 1;

    // Fórmula da Transformação de Viewport (X)
    double xvp = ((xw - wXmin) / (wXmax - wXmin)) * (vpXmax - vpXmin) + vpXmin;

    // Fórmula da Transformação de Viewport (Y) - Invertemos com (1 - ...) porque o Y da tela cresce para baixo
    double yvp = (1.0 - (yw - wYmin) / (wYmax - wYmin)) * (vpYmax - vpYmin) + vpYmin;

    // Retorna o ponto convertido arredondado para inteiros (pixels)
    return QPoint(std::round(xvp), std::round(yvp));
}