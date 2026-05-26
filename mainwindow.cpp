#include "mainwindow.h"
#include "matriz.h"
#include "objeto.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->spinBox->setValue(ui->frame->vpXmin);   // X min (0)
    ui->spinBox_2->setValue(ui->frame->vpYmin); // Y min (0)
    ui->spinBox_3->setValue(ui->frame->vpXmax); // X max (800)
    ui->spinBox_4->setValue(ui->frame->vpYmax); // Y max (600)

    connect(ui->frame, &MeuFrame::mouseMoveu, this, [this](QPoint pos) {
        QString msg = QString("Status: Pronto | Mouse: (%1, %2)").arg(pos.x()).arg(pos.y());
        ui->statusbar->showMessage(msg);
    });

    setWindowTitle("Visualizador 3D");

    criarCasinha(100, 100, "Casa 1");
    criarCasinha(300, 200, "Casa 2");
    criarCasinha(550, 50, "Casa 3");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnTranslacao_clicked()
{
    int index = ui->listWidget->currentRow();

    Objeto *obj = ui->frame->getObjeto(index);

    if (!obj)
        return;

    double dx = ui->editX->text().toDouble();
    double dy = ui->editY->text().toDouble();

    Matriz m = Matriz::translacao(dx, dy);
    obj->transformar(m);

    ui->frame->update();
}

void MainWindow::on_btnEscalar_clicked()
{
    int index = ui->listWidget->currentRow();

    Objeto *obj = ui->frame->getObjeto(index);
    if (!obj)
        return;

    double sx = ui->editEscalaX->text().toDouble();
    double sy = ui->editEscalaY->text().toDouble();

    QPointF centro = obj->getCentroide();

    Matriz tIda = Matriz::translacao(-centro.x(), -centro.y());
    Matriz esc = Matriz::escala(sx, sy);
    Matriz tVolta = Matriz::translacao(centro.x(), centro.y());

    Matriz final = tVolta * esc * tIda;

    obj->transformar(final);
    ui->frame->update();


}
void MainWindow::on_spinBox_valueChanged(int arg1)
{
    ui->frame->vpXmin = arg1;
    ui->frame->update();
}

void MainWindow::on_spinBox_2_valueChanged(int arg1)
{
    ui->frame->vpYmin = arg1;
    ui->frame->update();
}

void MainWindow::on_spinBox_3_valueChanged(int arg1)
{
    ui->frame->vpXmax = arg1;
    ui->frame->update();
}

void MainWindow::on_spinBox_4_valueChanged(int arg1)
{
    ui->frame->vpYmax = arg1;
    ui->frame->update();
}

void MainWindow::on_btnRotacionar_clicked()
{
    int index = ui->listWidget->currentRow();

    Objeto *obj = ui->frame->getObjeto(index);
    if (!obj)
        return;

    double angulo = ui->editAngulo->text().toDouble();

    QPointF pivot;

    if (ui->radioCentroide->isChecked()) {
        pivot = obj->getCentroide();
    } else {
        pivot = QPoint(ui->editPivotX->text().toInt(), ui->editPivotY->text().toInt());
    }

    Matriz tIda = Matriz::translacao(-pivot.x(), -pivot.y());
    Matriz rot = Matriz::rotacao(angulo);
    Matriz tVolta = Matriz::translacao(pivot.x(), pivot.y());

    obj->transformar(tVolta * rot * tIda);
    ui->frame->update();
}

// --- CONTROLES DE PAN (Mover a câmera) ---

void MainWindow::on_btnWinCima_clicked()
{
    double passo = 10.0;
    ui->frame->wYmin += passo;
    ui->frame->wYmax += passo;
    ui->frame->update();
}

void MainWindow::on_btnWinBaixo_clicked()
{
    double passo = 10.0;
    ui->frame->wYmin -= passo;
    ui->frame->wYmax -= passo;
    ui->frame->update();
}

void MainWindow::on_btnWinDireita_clicked()
{
    double passo = 10.0;
    ui->frame->wXmin += passo;
    ui->frame->wXmax += passo;
    ui->frame->update();
}

void MainWindow::on_btnWinEsquerda_clicked()
{
    double passo = 10.0;
    ui->frame->wXmin -= passo;
    ui->frame->wXmax -= passo;
    ui->frame->update();
}

// --- CONTROLES DE ZOOM ---

void MainWindow::on_btnZoomIn_clicked()
{
    double zoom = 10.0;
    // Para dar Zoom IN, encolhemos a window em direção ao centro
    ui->frame->wXmin += zoom;
    ui->frame->wXmax -= zoom;
    ui->frame->wYmin += zoom;
    ui->frame->wYmax -= zoom;
    ui->frame->update();
}

void MainWindow::on_btnZoomOut_clicked()
{
    double zoom = 10.0;
    // Para dar Zoom OUT, expandimos a window para fora
    ui->frame->wXmin -= zoom;
    ui->frame->wXmax += zoom;
    ui->frame->wYmin -= zoom;
    ui->frame->wYmax += zoom;
    ui->frame->update();
}

void MainWindow::criarCasinha(double x, double y, QString nomeCasa)
{
    Objeto casa(nomeCasa);

    //define as partes
    QList<QPointF> base = {QPoint(x, y), QPoint(x + 100, y), QPoint(x + 100, y + 100), QPoint(x, y + 100)};
    QList<QPointF> teto = {QPoint(x, y + 100), QPoint(x + 100, y + 100), QPoint(x + 50, y + 150)};
    QList<QPointF> porta = {QPoint(x + 20, y), QPoint(x + 50, y), QPoint(x + 50, y + 60), QPoint(x + 20, y + 60)};

    //adiciona as partes para dentro do obj
    casa.addFace(base);
    casa.addFace(teto);
    casa.addFace(porta);

    //manda pro Frame
    ui->frame->adicionarObjeto(casa);
    ui->listWidget->addItem(casa.nome);
}