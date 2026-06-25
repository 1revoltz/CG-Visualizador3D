#include "meuframe.h"
#include <QMouseEvent>

#include <algorithm>
#include <cmath>

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
        matrizProjecao = Matriz::projecaoPerspectiva(DISTANCIA_PERSPECTIVA);
        break;
    default:
        matrizProjecao = Matriz::projecaoOrtogonalXY();
        break;
    }

    update();
}

QPointF MeuFrame::projetar(const QVector3D &ponto) const
{
    const QVector3D pontoVisao = mundoParaVisao(ponto);

    if (vistaOrtogonal == VistaOrtogonal::Perspectiva) {
        const double z = pontoVisao.z();
        if (std::abs(z) < 1e-9) {
            return QPointF(CENTRO_PERSPECTIVA_X, CENTRO_PERSPECTIVA_Y);
        }

        return QPointF(CENTRO_PERSPECTIVA_X + (DISTANCIA_PERSPECTIVA * pontoVisao.x()) / z,
                       CENTRO_PERSPECTIVA_Y + (DISTANCIA_PERSPECTIVA * pontoVisao.y()) / z);
    }

    return QPointF(pontoVisao.x(), pontoVisao.y());
}

QVector3D MeuFrame::mundoParaVisao(const QVector3D &ponto) const
{
    switch (vistaOrtogonal) {
    case VistaOrtogonal::SuperiorXZ:
        return QVector3D(ponto.x(), ponto.z(), ponto.y());
    case VistaOrtogonal::LateralZY:
        return QVector3D(ponto.z(), ponto.y(), ponto.x());
    case VistaOrtogonal::Perspectiva:
        return QVector3D(ponto.x() - CENTRO_PERSPECTIVA_X,
                         ponto.y() - CENTRO_PERSPECTIVA_Y,
                         ponto.z() + DESLOCAMENTO_CAMERA_Z);
    default:
        return ponto;
    }
}

QVector3D MeuFrame::visaoParaMundo(const QVector3D &ponto) const
{
    switch (vistaOrtogonal) {
    case VistaOrtogonal::SuperiorXZ:
        return QVector3D(ponto.x(), ponto.z(), ponto.y());
    case VistaOrtogonal::LateralZY:
        return QVector3D(ponto.z(), ponto.y(), ponto.x());
    case VistaOrtogonal::Perspectiva:
        return QVector3D(ponto.x() + CENTRO_PERSPECTIVA_X,
                         ponto.y() + CENTRO_PERSPECTIVA_Y,
                         ponto.z() - DESLOCAMENTO_CAMERA_Z);
    default:
        return ponto;
    }
}

QList<MeuFrame::PlanoClip3D> MeuFrame::planosClippingVolumetrico() const
{
    const double xmin = std::min(wXmin, wXmax);
    const double xmax = std::max(wXmin, wXmax);
    const double ymin = std::min(wYmin, wYmax);
    const double ymax = std::max(wYmin, wYmax);

    auto vetorPlano = [](double x, double y, double z) {
        return QVector3D(static_cast<float>(x),
                         static_cast<float>(y),
                         static_cast<float>(z));
    };

    QList<PlanoClip3D> planos;

    if (vistaOrtogonal == VistaOrtogonal::Perspectiva) {
        const double esquerda = (xmin - CENTRO_PERSPECTIVA_X) / DISTANCIA_PERSPECTIVA;
        const double direita = (xmax - CENTRO_PERSPECTIVA_X) / DISTANCIA_PERSPECTIVA;
        const double baixo = (ymin - CENTRO_PERSPECTIVA_Y) / DISTANCIA_PERSPECTIVA;
        const double cima = (ymax - CENTRO_PERSPECTIVA_Y) / DISTANCIA_PERSPECTIVA;

        planos.append(PlanoClip3D{vetorPlano(1.0, 0.0, -esquerda), 0.0});
        planos.append(PlanoClip3D{vetorPlano(-1.0, 0.0, direita), 0.0});
        planos.append(PlanoClip3D{vetorPlano(0.0, 1.0, -baixo), 0.0});
        planos.append(PlanoClip3D{vetorPlano(0.0, -1.0, cima), 0.0});
        planos.append(PlanoClip3D{vetorPlano(0.0, 0.0, 1.0), -PROFUNDIDADE_PERSPECTIVA_MIN});
        planos.append(PlanoClip3D{vetorPlano(0.0, 0.0, -1.0), PROFUNDIDADE_PERSPECTIVA_MAX});
        return planos;
    }

    planos.append(PlanoClip3D{vetorPlano(1.0, 0.0, 0.0), -xmin});
    planos.append(PlanoClip3D{vetorPlano(-1.0, 0.0, 0.0), xmax});
    planos.append(PlanoClip3D{vetorPlano(0.0, 1.0, 0.0), -ymin});
    planos.append(PlanoClip3D{vetorPlano(0.0, -1.0, 0.0), ymax});
    planos.append(PlanoClip3D{vetorPlano(0.0, 0.0, 1.0), -PROFUNDIDADE_ORTOGONAL_MIN});
    planos.append(PlanoClip3D{vetorPlano(0.0, 0.0, -1.0), PROFUNDIDADE_ORTOGONAL_MAX});

    return planos;
}

double MeuFrame::distanciaAoPlano(const PlanoClip3D &plano, const QVector3D &ponto)
{
    return QVector3D::dotProduct(plano.normal, ponto) + plano.deslocamento;
}

bool MeuFrame::clipContraPlano(const PlanoClip3D &plano,
                               const QVector3D &inicio,
                               const QVector3D &fim,
                               double &tEntrada,
                               double &tSaida)
{
    constexpr double EPS = 1e-9;
    const double distInicio = distanciaAoPlano(plano, inicio);
    const double distFim = distanciaAoPlano(plano, fim);

    if (distInicio >= -EPS && distFim >= -EPS) {
        return true;
    }

    if (distInicio < -EPS && distFim < -EPS) {
        return false;
    }

    const double denominador = distInicio - distFim;
    if (std::abs(denominador) < EPS) {
        return false;
    }

    const double t = distInicio / denominador;
    if (distInicio < -EPS) {
        tEntrada = std::max(tEntrada, t);
    } else {
        tSaida = std::min(tSaida, t);
    }

    return tEntrada <= tSaida + EPS;
}

bool MeuFrame::pontoDentroVolume(const QVector3D &ponto) const
{
    const QVector3D pontoVisao = mundoParaVisao(ponto);
    const QList<PlanoClip3D> planos = planosClippingVolumetrico();

    for (const PlanoClip3D &plano : planos) {
        if (distanciaAoPlano(plano, pontoVisao) < -1e-9) {
            return false;
        }
    }

    return true;
}

bool MeuFrame::clipSegmentoVolume(QVector3D &p1, QVector3D &p2) const
{
    const QVector3D inicio = mundoParaVisao(p1);
    const QVector3D fim = mundoParaVisao(p2);
    const QVector3D direcao = fim - inicio;
    const QList<PlanoClip3D> planos = planosClippingVolumetrico();

    double tEntrada = 0.0;
    double tSaida = 1.0;

    for (const PlanoClip3D &plano : planos) {
        if (!clipContraPlano(plano, inicio, fim, tEntrada, tSaida)) {
            return false;
        }
    }

    if (tEntrada > tSaida) {
        return false;
    }

    const QVector3D inicioRecortado = inicio + direcao * static_cast<float>(tEntrada);
    const QVector3D fimRecortado = inicio + direcao * static_cast<float>(tSaida);

    p1 = visaoParaMundo(inicioRecortado);
    p2 = visaoParaMundo(fimRecortado);
    return true;
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
                const QVector3D ponto = obj.faces[f][0];
                if(pontoDentroVolume(ponto)) {
                    const QPointF p = projetar(ponto);
                    if(calcularCodigo(p) == INSIDE) {
                        painter.drawPoint(mundoParaTela(p));
                    }
                }
                continue;
            }

            for(int j = 0; j < numPontos; j++){

                // Dentro do laço de renderização - Conforme a imagem
                QVector3D p1_3d = obj.faces[f][j];
                QVector3D p2_3d = obj.faces[f][(j + 1) % numPontos];

                if (clipSegmentoVolume(p1_3d, p2_3d)) {
                    QPointF p1_mundo = projetar(p1_3d);
                    QPointF p2_mundo = projetar(p2_3d);

                    if (cohenSutherlandClip(p1_mundo, p2_mundo)) {
                        QPoint p1_tela = mundoParaTela(p1_mundo);
                        QPoint p2_tela = mundoParaTela(p2_mundo);
                        painter.drawLine(p1_tela, p2_tela);
                    }
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

    //verificação para evitar divisão por zero se a window tiver tamanho zero
    if (wXmax == wXmin) wXmax += 1;
    if (wYmax == wYmin) wYmax += 1;

    // fórmula da Transformação de Viewport (X)
    double xvp = ((xw - wXmin) / (wXmax - wXmin)) * (vpXmax - vpXmin) + vpXmin;

    // fórmula da Transformação de Viewport (Y)
    double yvp = (1.0 - (yw - wYmin) / (wYmax - wYmin)) * (vpYmax - vpYmin) + vpYmin;

    // retorna o ponto convertido arredondado para inteiros (pixels)
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
