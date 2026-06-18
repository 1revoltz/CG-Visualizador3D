#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector3D>

#include "objeto.h"

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

    void on_spinBox_valueChanged(int arg1);   // X min
    void on_spinBox_2_valueChanged(int arg1); // Y min
    void on_spinBox_3_valueChanged(int arg1); // X max
    void on_spinBox_4_valueChanged(int arg1); // Y max

    void on_btnWinCima_clicked();
    void on_btnWinBaixo_clicked();
    void on_btnWinDireita_clicked();
    void on_btnWinEsquerda_clicked();
    void on_btnZoomIn_clicked();
    void on_btnZoomOut_clicked();
    void on_btnCarregarObj_clicked();
    void on_comboProjecao_currentIndexChanged(int index);
    void on_Frente_clicked();

    void on_tras_clicked();

    void on_esquerda_clicked();

    void on_direita_clicked();

    void on_cima_clicked();

    void on_baixo_clicked();

    void on_olharEsquerda_clicked();

    void on_olharDireita_clicked();

    void on_olharCima_clicked();

    void on_olharBaixo_clicked();

private:
    Ui::MainWindow *ui;
    void carregarPokemon(const QString &nomeArquivo,
                         const QString &nomeObjeto,
                         const QVector3D &posicao,
                         double altura);
    void ajustarModeloParaCena(Objeto &objeto,
                               const QVector3D &posicao,
                               double altura);
    QString localizarArquivoDoProjeto(const QString &nomeArquivo) const;
};
#endif // MAINWINDOW_H
