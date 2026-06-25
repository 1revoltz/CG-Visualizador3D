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
    struct PlanoClip3D {
        QVector3D normal;
        double deslocamento;
    };

    static constexpr double DISTANCIA_PERSPECTIVA = 500.0;
    static constexpr double CENTRO_PERSPECTIVA_X = 400.0;
    static constexpr double CENTRO_PERSPECTIVA_Y = 300.0;
    static constexpr double DESLOCAMENTO_CAMERA_Z = 600.0;
    static constexpr double PROFUNDIDADE_ORTOGONAL_MIN = -1000.0;
    static constexpr double PROFUNDIDADE_ORTOGONAL_MAX = 1000.0;
    static constexpr double PROFUNDIDADE_PERSPECTIVA_MIN = 40.0;
    static constexpr double PROFUNDIDADE_PERSPECTIVA_MAX = 2500.0;

    QList<Objeto> displayFile;
    VistaOrtogonal vistaOrtogonal = VistaOrtogonal::FrontalXY;
    Matriz matrizProjecao = Matriz::projecaoOrtogonalXY();
    QPointF projetar(const QVector3D &ponto) const;
    QPoint mundoParaTela(QPointF ptMundo);
    QVector3D mundoParaVisao(const QVector3D &ponto) const;
    QVector3D visaoParaMundo(const QVector3D &ponto) const;
    QList<PlanoClip3D> planosClippingVolumetrico() const;
    static double distanciaAoPlano(const PlanoClip3D &plano, const QVector3D &ponto);
    static bool clipContraPlano(const PlanoClip3D &plano,
                                const QVector3D &inicio,
                                const QVector3D &fim,
                                double &tEntrada,
                                double &tSaida);
    bool pontoDentroVolume(const QVector3D &ponto) const;
    bool clipSegmentoVolume(QVector3D &p1, QVector3D &p2) const;

    //clipping
    int calcularCodigo(QPointF p);
    bool cohenSutherlandClip(QPointF &p1, QPointF &p2);
};

#endif // MEUFRAME_H
