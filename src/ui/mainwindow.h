#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector3D>

#include "objeto.h"

class QLineEdit;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void on_btnEscalar_clicked();

    void on_btnRotacionar_clicked();

    void on_btnTranslacao_clicked();

    void on_spinVpXMin_valueChanged(int arg1);   // X min
    void on_spinVpYMin_valueChanged(int arg1); // Y min
    void on_spinVpXMax_valueChanged(int arg1); // X max
    void on_spinVpYMax_valueChanged(int arg1); // Y max

    void on_btnWinCima_clicked();
    void on_btnWinBaixo_clicked();
    void on_btnWinDireita_clicked();
    void on_btnWinEsquerda_clicked();
    void on_btnZoomIn_clicked();
    void on_btnZoomOut_clicked();
    void on_btnCarregarObj_clicked();
    void on_btnLimparTela_clicked();
    void on_comboProjecao_currentIndexChanged(int index);
    void on_btnCameraFrente_clicked();

    void on_btnCameraTras_clicked();

    void on_btnCameraEsquerda_clicked();

    void on_btnCameraDireita_clicked();

    void on_btnCameraCima_clicked();

    void on_btnCameraBaixo_clicked();

    void on_btnOlharEsquerda_clicked();

    void on_btnOlharDireita_clicked();

    void on_btnOlharCima_clicked();

    void on_btnOlharBaixo_clicked();

private:
    Ui::MainWindow *ui;
    void carregarModelosPadrao();
    void carregarPokemon(const QString &nomeArquivo,
                         const QString &nomeObjeto,
                         const QVector3D &posicao,
                         double altura);
    void ajustarModeloParaCena(Objeto &objeto,
                               const QVector3D &posicao,
                               double altura);
    void ajustarZoomWindow(double zoom);
    bool lerCampoNumerico(QLineEdit *campo,
                          const QString &nomeCampo,
                          double &valor);
    QString localizarArquivoDoProjeto(const QString &nomeArquivo) const;
};
#endif // MAINWINDOW_H
