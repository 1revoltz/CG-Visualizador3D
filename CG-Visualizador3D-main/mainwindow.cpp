#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "objeto.h"

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

    ui->statusbar->showMessage("Status: Pronto | Nível de Zoom: 100% | Mouse: (0, 0)");

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