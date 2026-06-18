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

    vpXmin = 50;
    vpYmin = 50;
    vpXmax = 750;
    vpYmax = 550;
}

void MeuFrame::adicionarObjeto(Objeto obj){
    displayFile.append(obj);
    update();
}

void MeuFrame::setVistaOrtogonal(VistaOrtogonal vista)
{
    vistaOrtogonal = vista;

    switch (vistaOrtogonal) {
    case VistaOrtogonal::SuperiorXZ:
        matrizProjecao = Matriz::projecaoOrtogonalXZ();
        break;
    case VistaOrtogonal::LateralZY:
        matrizProjecao = Matriz::projecaoOrtogonalZY();
        break;
    case VistaOrtogonal::Perspectiva:
        matrizProjecao = Matriz::projecaoPerspectiva(500.0);
        break;
    default:
        matrizProjecao = Matriz::projecaoOrtogonalXY();
        break;
    }

    update();
}

QPointF MeuFrame::projetar(const QVector3D &ponto) const
{
    // SE FOR MODO PERSPECTIVA
    if (vistaOrtogonal == VistaOrtogonal::Perspectiva) {

        // 1. Centraliza o ponto em relação ao meio do mundo (400, 300)
        // e afasta o modelo em Z (+600) para ele não ficar colado na lente da câmera
        double xc = ponto.x() - 400.0;
        double yc = ponto.y() - 300.0;
        double zc = ponto.z() + 600.0;

        QVector3D pontoEspacoCamera(xc, yc, zc);

        // 2. Multiplica pela matriz de perspectiva (executa a divisão por W automaticamente no seu operator*)
        QVector3D pProjetado = matrizProjecao * pontoEspacoCamera;

        // 3. Devolve o ponto movendo-o de volta para o centro do seu Frame de desenho
        return QPointF(pProjetado.x() + 400.0, pProjetado.y() + 300.0);
    }

    // SE FOR MODO ORTOGONAL (Mantém o seu código original funcionando igualzinho)
    else {
        QVector3D pProjetado = matrizProjecao * ponto;
        return QPointF(pProjetado.x(), pProjetado.y());
    }
}

void MeuFrame::transformarMundo(const Matriz &m)
{
    for (int i = 0; i < displayFile.size(); ++i) {
        displayFile[i].transformar(m);
    }

    update();
}

void MeuFrame::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    QPainter painter(this);

    QPen pen(Qt::black);
    pen.setWidth(2);
    painter.setPen(pen);

    painter.drawRect(vpXmin, vpYmin, vpXmax - vpXmin, vpYmax - vpYmin);

    for(int i = 0; i < displayFile.size(); i++){
        const Objeto &obj = displayFile[i];

        for(int f = 0; f < obj.faces.size(); f++) {

            int numPontos = obj.faces[f].size();

            // Tratamento especial para pontos soltos (adaptado para QVector3D)
            if(numPontos == 1) {
                const QPointF p = projetar(obj.faces[f][0]);
                if(calcularCodigo(p) == INSIDE) {
                    painter.drawPoint(mundoParaTela(p));
                }
                continue;
            }

            for(int j = 0; j < numPontos; j++){

                // Dentro do laço de renderização - Conforme a imagem
                QPointF p1_mundo = projetar(obj.faces[f][j]);
                QPointF p2_mundo = projetar(obj.faces[f][(j + 1) % numPontos]);

                if (cohenSutherlandClip(p1_mundo, p2_mundo)) {
                    QPoint p1_tela = mundoParaTela(p1_mundo);
                    QPoint p2_tela = mundoParaTela(p2_mundo);
                    painter.drawLine(p1_tela, p2_tela);
                }
            }
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

QPoint MeuFrame::mundoParaTela(QPointF ptMundo) {
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

//diz onde o ponto esta em relação a WINDOW
int MeuFrame::calcularCodigo(QPointF p) {
    int code = INSIDE;

    if (p.x() < wXmin) code |= LEFT;      //ativa bit da esquerda
    else if (p.x() > wXmax) code |= RIGHT; //ativa bit da direita

    if (p.y() < wYmin) code |= BOTTOM;    //ativa bit de baixo
    else if (p.y() > wYmax) code |= TOP;   //ativa bit de cima

    return code;
}

bool MeuFrame::cohenSutherlandClip(QPointF &p1, QPointF &p2) {
    int code1 = calcularCodigo(p1);
    int code2 = calcularCodigo(p2);
    bool aceito = false;

    while (true) {
        if (!(code1 | code2)) {
            //a linha esta totalmente dentro 0000
            aceito = true;
            break;
        } else if (code1 & code2) {
            //ambos os pontos compartilham uma mesma zona externa, totalmente fora
            break;
        } else {
            //linha cruza alguma borda
            int codeOut;
            double x, y;

            //escolhendo o ponto que esta fora
            if (code1 != INSIDE) codeOut = code1;
            else codeOut = code2;

            if (codeOut & TOP) {           //ponto esta acima da Window
                x = p1.x() + (p2.x() - p1.x()) * (wYmax - p1.y()) / (p2.y() - p1.y());
                y = wYmax;
            } else if (codeOut & BOTTOM) { //ponto esta abaixo da Window
                x = p1.x() + (p2.x() - p1.x()) * (wYmin - p1.y()) / (p2.y() - p1.y());
                y = wYmin;
            } else if (codeOut & RIGHT) {  //ponto esta a direita
                y = p1.y() + (p2.y() - p1.y()) * (wXmax - p1.x()) / (p2.x() - p1.x());
                x = wXmax;
            } else if (codeOut & LEFT) {   //ponto esta a esquerda
                y = p1.y() + (p2.y() - p1.y()) * (wXmin - p1.x()) / (p2.x() - p1.x());
                x = wXmin;
            }

            //atualizamos o ponto que estava fora com as novas coordenadas do corte
            if (codeOut == code1) {
                p1.setX(x);
                p1.setY(y);
                code1 = calcularCodigo(p1); //recalculamos o codigo para ver se ele entrou
            } else {
                p2.setX(x);
                p2.setY(y);
                code2 = calcularCodigo(p2); //recalculamos o codigo do segundo ponto
            }
        }
    }
    return aceito; //se aceito=true, desenhamos a reta (p1->p2)
}
