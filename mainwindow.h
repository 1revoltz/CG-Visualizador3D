#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
private:
    Ui::MainWindow *ui;
    void criarCasinha(double x, double y, QString nomeCasa);
};
#endif // MAINWINDOW_H
