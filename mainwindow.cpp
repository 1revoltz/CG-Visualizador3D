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

    // TESTES

    // TESTE PONTO
    QList<QPoint> coordsPonto = {QPoint(50, 50)};
    Objeto meuPonto("Ponto A", Objeto::PONTO, coordsPonto);

    // TESTE RETA
    QList<QPoint> coordsReta = {QPoint(100, 100), QPoint(300, 200)};
    Objeto minhaReta("Reta B", Objeto::RETA, coordsReta);

    // TESTE POLIGONO(TRIANGULO)
    QList<QPoint> coordsTriangulo = {QPoint(400, 400), QPoint(600, 400), QPoint(500, 200)};
    Objeto meuTriangulo("Triângulo C", Objeto::POLIGONO, coordsTriangulo);

    // Mandando os objetos para o Display File dentro do MeuFrame
    ui->frame->adicionarObjeto(meuPonto);
    ui->frame->adicionarObjeto(minhaReta);
    ui->frame->adicionarObjeto(meuTriangulo);

    //preencher a lista de objetos
    ui->listWidget->addItem(meuPonto.nome);
    ui->listWidget->addItem(minhaReta.nome);
    ui->listWidget->addItem(meuTriangulo.nome);
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

    QPoint centro = obj->getCentroide();

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

    QPoint pivot;

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