#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "objeto.h"
#include "matriz.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->frame, &MeuFrame::mouseMoveu, this, [this](QPoint pos){
        QString msg = QString("Status: Pronto | Mouse: (%1, %2)").arg(pos.x()).arg(pos.y());
        ui->statusbar->showMessage(msg);
    });

    setWindowTitle("Visualizador 3D");

    // TESTES

    // TESTE PONTO
    QList<QPoint> coordsPonto = { QPoint(50, 50) };
    Objeto meuPonto("Ponto A", Objeto::PONTO, coordsPonto);

    // TESTE RETA
    QList<QPoint> coordsReta = { QPoint(100, 100), QPoint(300, 200) };
    Objeto minhaReta("Reta B", Objeto::RETA, coordsReta);

    // TESTE POLIGONO(TRIANGULO)
    QList<QPoint> coordsTriangulo = { QPoint(400, 400), QPoint(600, 400), QPoint(500, 200) };
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

    Objeto* obj = ui->frame->getObjeto(index);

    if (!obj) return;

    double dx = ui->editX->text().toDouble();
    double dy = ui->editY->text().toDouble();

    Matriz m = Matriz::translacao(dx, dy);
    obj->transformar(m);

    ui->frame->update();
}

void MainWindow::on_btnEscalar_clicked()
{
    int index = ui->listWidget->currentRow();

    Objeto* obj = ui->frame->getObjeto(index);
    if (!obj) return;

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

void MainWindow::on_btnRotacionar_clicked()
{
    int index = ui->listWidget->currentRow();

    Objeto* obj = ui->frame->getObjeto(index);
    if (!obj) return;

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