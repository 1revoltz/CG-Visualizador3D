#include "mainwindow.h"
#include "leitorobj.h"
#include "matriz.h"
#include "objeto.h"
#include "ui_mainwindow.h"

#include <QCoreApplication>
#include <QDoubleValidator>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QLineEdit>

#include <limits>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto *validadorNumerico = new QDoubleValidator(this);
    validadorNumerico->setNotation(QDoubleValidator::StandardNotation);
    const QList<QLineEdit *> camposNumericos = {
        ui->editX, ui->editY, ui->editZ,
        ui->editEscalaX, ui->editEscalaY, ui->editEscalaZ,
        ui->editAngulo,
        ui->editPivotX, ui->editPivotY, ui->editPivotZ
    };
    for (QLineEdit *campo : camposNumericos) {
        campo->setValidator(validadorNumerico);
    }

    ui->spinVpXMin->setValue(ui->visualizadorFrame->vpXmin);   // X min (0)
    ui->spinVpYMin->setValue(ui->visualizadorFrame->vpYmin); // Y min (0)
    ui->spinVpXMax->setValue(ui->visualizadorFrame->vpXmax); // X max (800)
    ui->spinVpYMax->setValue(ui->visualizadorFrame->vpYmax); // Y max (600)

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

    connect(ui->visualizadorFrame, &MeuFrame::mouseMoveu, this, [this](QPoint pos) {
        QString msg = QString("Status: Pronto | Mouse: (%1, %2)").arg(pos.x()).arg(pos.y());
        ui->statusbar->showMessage(msg);
    });

    setWindowTitle("Visualizador 3D");

    carregarModelosPadrao();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_comboProjecao_currentIndexChanged(int index)
{
    switch (index) {
    case 1:
        ui->visualizadorFrame->setVistaOrtogonal(MeuFrame::VistaOrtogonal::SuperiorXZ);
        break;
    case 2:
        ui->visualizadorFrame->setVistaOrtogonal(MeuFrame::VistaOrtogonal::LateralZY);
        break;
    case 3:
        ui->visualizadorFrame->setVistaOrtogonal(MeuFrame::VistaOrtogonal::Perspectiva);
        break;
    default:
        ui->visualizadorFrame->setVistaOrtogonal(MeuFrame::VistaOrtogonal::FrontalXY);
        break;
    }
}

QString MainWindow::localizarArquivoDoProjeto(const QString &nomeArquivo) const
{
    const QFileInfo arquivoInformado(nomeArquivo);
    if (arquivoInformado.isAbsolute() && arquivoInformado.exists()) {
        return arquivoInformado.absoluteFilePath();
    }

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

void MainWindow::ajustarModeloParaCena(Objeto &objeto, const QVector3D &posicaoDestino, double alturaDesejada)
{
    if (objeto.faces.isEmpty()) return;

    //encontrar os limites mínimos e máximos do objeto
    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();
    double minZ = std::numeric_limits<double>::max();
    double maxZ = std::numeric_limits<double>::lowest();

    for (const auto &face : objeto.faces) {
        for (const QVector3D &p : face) {
            if (p.x() < minX) minX = p.x();
            if (p.x() > maxX) maxX = p.x();
            if (p.y() < minY) minY = p.y();
            if (p.y() > maxY) maxY = p.y();
            if (p.z() < minZ) minZ = p.z();
            if (p.z() > maxZ) maxZ = p.z();
        }
    }

    // calcular o centro geométrico atual do objeto e a altura real
    double centroX = (minX + maxX) / 2.0;
    double centroY = (minY + maxY) / 2.0;
    double centroZ = (minZ + maxZ) / 2.0;
    double alturaReal = maxY - minY;

    if (alturaReal == 0) alturaReal = 1.0; // evita divisão por zero

    //calcular o fator de escala necessário para o objeto se adequar na tela
    double fatorEscala = alturaDesejada / alturaReal;

    // montar a matriz:
    Matriz tParaOrigem = Matriz::translacao(-centroX, -centroY, -centroZ);
    Matriz e = Matriz::escala(fatorEscala, fatorEscala, fatorEscala);
    Matriz tParaCena = Matriz::translacao(posicaoDestino.x(), posicaoDestino.y(), posicaoDestino.z());

    // multiplicação na ordem correta do pipeline de transformações geométricas
    Matriz transformacaoFinal = tParaCena * (e * tParaOrigem);

    //aplica a transformação em todos os vértices do objeto
    objeto.transformar(transformacaoFinal);
}

void MainWindow::carregarModelosPadrao()
{
    const QString caminhoModelos = localizarArquivoDoProjeto("assets/models");
    if (caminhoModelos.isEmpty()) {
        ui->statusbar->showMessage("Pasta de modelos nao encontrada: assets/models", 10000);
        return;
    }

    QDir diretorioModelos(caminhoModelos);
    const QFileInfoList modelos = diretorioModelos.entryInfoList(QStringList() << "*.obj",
                                                                 QDir::Files,
                                                                 QDir::Name);
    if (modelos.isEmpty()) {
        ui->statusbar->showMessage("Nenhum modelo OBJ encontrado em assets/models", 10000);
        return;
    }

    const int colunas = 4;
    const double inicioX = 120.0;
    const double inicioY = 210.0;
    const double espacoX = 190.0;
    const double espacoY = 220.0;
    const double alturaModelo = modelos.size() > colunas ? 150.0 : 220.0;

    for (int i = 0; i < modelos.size(); ++i) {
        const QFileInfo &modelo = modelos[i];
        const int coluna = i % colunas;
        const int linha = i / colunas;
        const QVector3D posicao(inicioX + coluna * espacoX,
                                inicioY + linha * espacoY,
                                0.0);

        carregarPokemon(modelo.absoluteFilePath(),
                        modelo.completeBaseName(),
                        posicao,
                        alturaModelo);
    }
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
    ui->visualizadorFrame->adicionarObjeto(pokemon);
    ui->listaObjetos->addItem(pokemon.nome);
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

    ajustarModeloParaCena(objeto, QVector3D(400.0, 300.0, 0.0), 350.0);

    ui->visualizadorFrame->adicionarObjeto(objeto);
    ui->listaObjetos->addItem(objeto.nome);
    ui->listaObjetos->setCurrentRow(ui->listaObjetos->count() - 1);
    ui->statusbar->showMessage(
        QString("OBJ carregado: %1 faces").arg(objeto.faces.size()),
        5000);
}

void MainWindow::on_btnTranslacao_clicked()
{
    int index = ui->listaObjetos->currentRow();

    Objeto *obj = ui->visualizadorFrame->getObjeto(index);

    if (!obj)
        return;

    double dx = ui->editX->text().toDouble();
    double dy = ui->editY->text().toDouble();
    double dz = ui->editZ->text().toDouble();

    Matriz m = Matriz::translacao(dx, dy, dz);
    obj->transformar(m);

    ui->visualizadorFrame->update();
}

void MainWindow::on_btnEscalar_clicked()
{
    int index = ui->listaObjetos->currentRow();

    Objeto *obj = ui->visualizadorFrame->getObjeto(index);
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
    ui->visualizadorFrame->update();


}
void MainWindow::on_spinVpXMin_valueChanged(int arg1)
{
    ui->visualizadorFrame->vpXmin = arg1;
    ui->visualizadorFrame->update();
}

void MainWindow::on_spinVpYMin_valueChanged(int arg1)
{
    ui->visualizadorFrame->vpYmin = arg1;
    ui->visualizadorFrame->update();
}

void MainWindow::on_spinVpXMax_valueChanged(int arg1)
{
    ui->visualizadorFrame->vpXmax = arg1;
    ui->visualizadorFrame->update();
}

void MainWindow::on_spinVpYMax_valueChanged(int arg1)
{
    ui->visualizadorFrame->vpYmax = arg1;
    ui->visualizadorFrame->update();
}

void MainWindow::on_btnRotacionar_clicked()
{
    int index = ui->listaObjetos->currentRow();

    Objeto *obj = ui->visualizadorFrame->getObjeto(index);
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
    ui->visualizadorFrame->update();
}

// --- CONTROLES DE CAMERA ---

void MainWindow::on_btnWinCima_clicked()
{
    double passo = 10.0;
    ui->visualizadorFrame->wYmin += passo;
    ui->visualizadorFrame->wYmax += passo;
    ui->visualizadorFrame->update();
}

void MainWindow::on_btnWinBaixo_clicked()
{
    double passo = 10.0;
    ui->visualizadorFrame->wYmin -= passo;
    ui->visualizadorFrame->wYmax -= passo;
    ui->visualizadorFrame->update();
}

void MainWindow::on_btnWinDireita_clicked()
{
    double passo = 10.0;
    ui->visualizadorFrame->wXmin += passo;
    ui->visualizadorFrame->wXmax += passo;
    ui->visualizadorFrame->update();
}

void MainWindow::on_btnWinEsquerda_clicked()
{
    double passo = 10.0;
    ui->visualizadorFrame->wXmin -= passo;
    ui->visualizadorFrame->wXmax -= passo;
    ui->visualizadorFrame->update();
}

// --- CONTROLES DE ZOOM ---

void MainWindow::ajustarZoomWindow(double zoom)
{
    ui->visualizadorFrame->wXmin += zoom;
    ui->visualizadorFrame->wXmax -= zoom;
    ui->visualizadorFrame->wYmin += zoom;
    ui->visualizadorFrame->wYmax -= zoom;
    ui->visualizadorFrame->update();
}

void MainWindow::on_btnZoomIn_clicked()
{
    ajustarZoomWindow(10.0);
}

void MainWindow::on_btnZoomOut_clicked()
{
    ajustarZoomWindow(-10.0);
}


void MainWindow::on_btnCameraFrente_clicked()
{
    if (ui->comboProjecao->currentIndex() == 3) {
        Matriz mov = Matriz::translacao(0.0, 0.0, -60.0);
        ui->visualizadorFrame->transformarMundo(mov);
        return;
    }

    ajustarZoomWindow(10.0);
}


void MainWindow::on_btnCameraTras_clicked()
{
    if (ui->comboProjecao->currentIndex() == 3) {
        Matriz mov = Matriz::translacao(0.0, 0.0, 60.0);
        ui->visualizadorFrame->transformarMundo(mov);
        return;
    }

    ajustarZoomWindow(-10.0);
}

void MainWindow::on_btnCameraEsquerda_clicked()
{
    Matriz mov = Matriz::translacao(20.0, 0.0, 0.0);
    ui->visualizadorFrame->transformarMundo(mov);
}


void MainWindow::on_btnCameraDireita_clicked()
{
    Matriz mov = Matriz::translacao(-20.0, 0.0, 0.0);
    ui->visualizadorFrame->transformarMundo(mov);
}


void MainWindow::on_btnCameraCima_clicked()
{
    Matriz mov = Matriz::translacao(0.0, -20.0, 0.0);
    ui->visualizadorFrame->transformarMundo(mov);
}


void MainWindow::on_btnCameraBaixo_clicked()
{
    Matriz mov = Matriz::translacao(0.0, 20.0, 0.0);
    ui->visualizadorFrame->transformarMundo(mov);
}


void MainWindow::on_btnOlharEsquerda_clicked()
{
    Matriz tIda = Matriz::translacao(-400.0, -300.0, 600.0);
    Matriz rot = Matriz::rotacaoY(5.0);
    Matriz tVolta = Matriz::translacao(400.0, 300.0, -600.0);

    Matriz mFinal = tVolta * (rot * tIda);
    ui->visualizadorFrame->transformarMundo(mFinal);
}


void MainWindow::on_btnOlharDireita_clicked()
{
    Matriz tIda = Matriz::translacao(-400.0, -300.0, 600.0);
    Matriz rot = Matriz::rotacaoY(-5.0);
    Matriz tVolta = Matriz::translacao(400.0, 300.0, -600.0);

    Matriz mFinal = tVolta * (rot * tIda);
    ui->visualizadorFrame->transformarMundo(mFinal);
}


void MainWindow::on_btnOlharCima_clicked()
{
    // move camera pra origem
    Matriz tIda = Matriz::translacao(-400.0, -300.0, 600.0);
    //rotaciona o mundo pra baixo pra olhar pra cima
    Matriz rot = Matriz::rotacaoX(5.0);
    //camera volta pro lugar dela
    Matriz tVolta = Matriz::translacao(400.0, 300.0, -600.0);

    Matriz mFinal = tVolta * (rot * tIda);
    ui->visualizadorFrame->transformarMundo(mFinal);
}


void MainWindow::on_btnOlharBaixo_clicked()
{
    Matriz tIda = Matriz::translacao(-400.0, -300.0, 600.0);
    Matriz rot = Matriz::rotacaoX(-5.0);
    Matriz tVolta = Matriz::translacao(400.0, 300.0, -600.0);

    Matriz mFinal = tVolta * (rot * tIda);
    ui->visualizadorFrame->transformarMundo(mFinal);
}

