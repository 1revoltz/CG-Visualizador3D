#include "mainwindow.h"
#include "leitorobj.h"
#include "matriz.h"
#include "objeto.h"
#include "ui_mainwindow.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include <algorithm>
#include <limits>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->spinBox->setValue(ui->frame->vpXmin);   // X min (0)
    ui->spinBox_2->setValue(ui->frame->vpYmin); // Y min (0)
    ui->spinBox_3->setValue(ui->frame->vpXmax); // X max (800)
    ui->spinBox_4->setValue(ui->frame->vpYmax); // Y max (600)

    ui->editX->setText("0");
    ui->editY->setText("0");
    ui->editZ->setText("0");
    ui->editEscalaX->setText("1");
    ui->editEscalaY->setText("1");
    ui->editEscalaZ->setText("1");
    ui->editAngulo->setText("0");
    ui->editPivotX->setText("0");
    ui->editPivotY->setText("0");
    ui->editPivotZ->setText("0");
    ui->radioCentroide->setChecked(true);
    ui->comboEixoRotacao->setCurrentIndex(2);
    ui->comboProjecao->setCurrentIndex(0);

    connect(ui->frame, &MeuFrame::mouseMoveu, this, [this](QPoint pos) {
        QString msg = QString("Status: Pronto | Mouse: (%1, %2)").arg(pos.x()).arg(pos.y());
        ui->statusbar->showMessage(msg);
    });

    setWindowTitle("Visualizador 3D");

    criarCasinha(100, 100, "Casa 1");
    criarCasinha(300, 200, "Casa 2");
    criarCasinha(550, 50, "Casa 3");

    carregarPokemon("UmbreonHighPoly.obj",
                    "Umbreon",
                    QVector3D(150.0, 400.0, 0.0),
                    220.0);
    carregarPokemon("weedle.obj",
                    "Weedle",
                    QVector3D(650.0, 380.0, 0.0),
                    220.0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_comboProjecao_currentIndexChanged(int index)
{
    switch (index) {
    case 1:
        ui->frame->setVistaOrtogonal(MeuFrame::VistaOrtogonal::SuperiorXZ);
        break;
    case 2:
        ui->frame->setVistaOrtogonal(MeuFrame::VistaOrtogonal::LateralZY);
        break;
    default:
        ui->frame->setVistaOrtogonal(MeuFrame::VistaOrtogonal::FrontalXY);
        break;
    }
}

QString MainWindow::localizarArquivoDoProjeto(const QString &nomeArquivo) const
{
    QDir diretorio(QCoreApplication::applicationDirPath());

    for (int nivel = 0; nivel < 6; ++nivel) {
        const QString candidato = diretorio.absoluteFilePath(nomeArquivo);
        if (QFileInfo::exists(candidato)) {
            return candidato;
        }

        if (!diretorio.cdUp()) {
            break;
        }
    }

    const QString candidatoAtual = QDir::current().absoluteFilePath(nomeArquivo);
    if (QFileInfo::exists(candidatoAtual)) {
        return candidatoAtual;
    }

    return QString();
}

void MainWindow::ajustarModeloParaCena(Objeto &objeto,
                                       const QVector3D &posicao,
                                       double altura)
{
    QVector3D minimo(std::numeric_limits<float>::max(),
                     std::numeric_limits<float>::max(),
                     std::numeric_limits<float>::max());
    QVector3D maximo(std::numeric_limits<float>::lowest(),
                     std::numeric_limits<float>::lowest(),
                     std::numeric_limits<float>::lowest());

    for (const QList<QVector3D> &face : objeto.faces) {
        for (const QVector3D &ponto : face) {
            minimo.setX(std::min(minimo.x(), ponto.x()));
            minimo.setY(std::min(minimo.y(), ponto.y()));
            minimo.setZ(std::min(minimo.z(), ponto.z()));
            maximo.setX(std::max(maximo.x(), ponto.x()));
            maximo.setY(std::max(maximo.y(), ponto.y()));
            maximo.setZ(std::max(maximo.z(), ponto.z()));
        }
    }

    const double alturaAtual = maximo.y() - minimo.y();
    if (alturaAtual <= 0.0) {
        return;
    }

    const QVector3D centro = (minimo + maximo) / 2.0f;
    const double fator = altura / alturaAtual;

    const Matriz centralizar = Matriz::translacao(-centro.x(), -centro.y(), -centro.z());
    const Matriz redimensionar = Matriz::escala(fator, fator, fator);
    const Matriz posicionar = Matriz::translacao(posicao.x(), posicao.y(), posicao.z());

    objeto.transformar(posicionar * redimensionar * centralizar);
}

void MainWindow::carregarPokemon(const QString &nomeArquivo,
                                 const QString &nomeObjeto,
                                 const QVector3D &posicao,
                                 double altura)
{
    const QString caminho = localizarArquivoDoProjeto(nomeArquivo);
    if (caminho.isEmpty()) {
        ui->statusbar->showMessage(
            QString("Arquivo não encontrado: %1").arg(nomeArquivo),
            10000);
        return;
    }

    Objeto pokemon(nomeObjeto);
    QString erro;
    if (!LeitorObj::carregar(caminho, pokemon, &erro)) {
        ui->statusbar->showMessage(
            QString("Erro ao carregar %1: %2").arg(nomeObjeto, erro),
            10000);
        return;
    }

    pokemon.nome = nomeObjeto;
    ajustarModeloParaCena(pokemon, posicao, altura);
    ui->frame->adicionarObjeto(pokemon);
    ui->listWidget->addItem(pokemon.nome);
}

void MainWindow::on_btnCarregarObj_clicked()
{
    const QString caminho = QFileDialog::getOpenFileName(
        this,
        "Carregar modelo OBJ",
        QString(),
        "Modelo Wavefront (*.obj)");

    if (caminho.isEmpty()) {
        return;
    }

    Objeto objeto(QFileInfo(caminho).completeBaseName());
    QString erro;

    if (!LeitorObj::carregar(caminho, objeto, &erro)) {
        QMessageBox::critical(this, "Erro ao carregar OBJ", erro);
        return;
    }

    ui->frame->adicionarObjeto(objeto);
    ui->listWidget->addItem(objeto.nome);
    ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
    ui->statusbar->showMessage(
        QString("OBJ carregado: %1 faces").arg(objeto.faces.size()),
        5000);
}

void MainWindow::on_btnTranslacao_clicked()
{
    int index = ui->listWidget->currentRow();

    Objeto *obj = ui->frame->getObjeto(index);

    if (!obj)
        return;

    double dx = ui->editX->text().toDouble();
    double dy = ui->editY->text().toDouble();
    double dz = ui->editZ->text().toDouble();

    Matriz m = Matriz::translacao(dx, dy, dz);
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
    double sz = ui->editEscalaZ->text().toDouble();

    QVector3D centro = obj->getCentroide();

    Matriz tIda = Matriz::translacao(-centro.x(), -centro.y(), -centro.z());
    Matriz esc = Matriz::escala(sx, sy, sz);
    Matriz tVolta = Matriz::translacao(centro.x(), centro.y(), centro.z());

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

    QVector3D pivot;

    if (ui->radioCentroide->isChecked()) {
        pivot = obj->getCentroide();
    } else {
        pivot = QVector3D(ui->editPivotX->text().toDouble(),
                          ui->editPivotY->text().toDouble(),
                          ui->editPivotZ->text().toDouble());
    }

    Matriz tIda = Matriz::translacao(-pivot.x(), -pivot.y(), -pivot.z());
    Matriz rot;
    switch (ui->comboEixoRotacao->currentIndex()) {
    case 0:
        rot = Matriz::rotacaoX(angulo);
        break;
    case 1:
        rot = Matriz::rotacaoY(angulo);
        break;
    default:
        rot = Matriz::rotacaoZ(angulo);
        break;
    }
    Matriz tVolta = Matriz::translacao(pivot.x(), pivot.y(), pivot.z());

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

    // Define as partes usando QVector3D com Z = 0.0
    QList<QVector3D> base = {QVector3D(x, y, 0.0), QVector3D(x + 100, y, 0.0), QVector3D(x + 100, y + 100, 0.0), QVector3D(x, y + 100, 0.0)};
    QList<QVector3D> teto = {QVector3D(x, y + 100, 0.0), QVector3D(x + 100, y + 100, 0.0), QVector3D(x + 50, y + 150, 0.0)};
    QList<QVector3D> porta = {QVector3D(x + 20, y, 0.0), QVector3D(x + 50, y, 0.0), QVector3D(x + 50, y + 60, 0.0), QVector3D(x + 20, y + 60, 0.0)};

    // Adiciona as partes para dentro do obj
    casa.addFace(base);
    casa.addFace(teto);
    casa.addFace(porta);

    // Manda pro Frame
    ui->frame->adicionarObjeto(casa);
    ui->listWidget->addItem(casa.nome);
}
