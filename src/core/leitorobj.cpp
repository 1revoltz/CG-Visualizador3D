#include "leitorobj.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include <utility>

bool LeitorObj::carregar(const QString &caminhoArquivo, Objeto &objeto, QString *erro)
{
    QFile arquivo(caminhoArquivo);
    if (!arquivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (erro) {
            *erro = QString("Não foi possível abrir o arquivo: %1").arg(arquivo.errorString());
        }
        return false;
    }

    QList<QVector3D> vertices;
    QList<QList<QVector3D>> faces;
    QTextStream entrada(&arquivo);
    qsizetype numeroLinha = 0;

    while (!entrada.atEnd()) {
        const QString linha = entrada.readLine().trimmed();
        ++numeroLinha;

        if (linha.isEmpty() || linha.startsWith('#')) {
            continue;
        }

        const QStringList partes = linha.split(QChar(' '), Qt::SkipEmptyParts);
        if (partes.isEmpty()) {
            continue;
        }

        if (partes[0] == "v") {
            if (partes.size() < 4) {
                if (erro) {
                    *erro = QString("Vértice inválido na linha %1.").arg(numeroLinha);
                }
                return false;
            }

            bool xValido;
            bool yValido;
            bool zValido;
            const float x = partes[1].toFloat(&xValido);
            const float y = partes[2].toFloat(&yValido);
            const float z = partes[3].toFloat(&zValido);

            if (!xValido || !yValido || !zValido) {
                if (erro) {
                    *erro = QString("Coordenada inválida na linha %1.").arg(numeroLinha);
                }
                return false;
            }

            vertices.append(QVector3D(x, y, z));
            continue;
        }

        if (partes[0] != "f" && partes[0] != "l") {
            continue;
        }

        if (partes.size() < 3) {
            if (erro) {
                *erro = QString("Face ou aresta inválida na linha %1.").arg(numeroLinha);
            }
            return false;
        }

        QList<QVector3D> face;
        face.reserve(partes.size() - 1);

        for (qsizetype i = 1; i < partes.size(); ++i) {
            const QString indiceTexto = partes[i].section('/', 0, 0);
            bool indiceValido;
            const int indiceObj = indiceTexto.toInt(&indiceValido);

            if (!indiceValido || indiceObj == 0) {
                if (erro) {
                    *erro = QString("Índice de vértice inválido na linha %1.").arg(numeroLinha);
                }
                return false;
            }

            const qsizetype indice = indiceObj > 0
                                          ? static_cast<qsizetype>(indiceObj - 1)
                                          : vertices.size() + indiceObj;

            if (indice < 0 || indice >= vertices.size()) {
                if (erro) {
                    *erro = QString("Índice fora dos limites na linha %1.").arg(numeroLinha);
                }
                return false;
            }

            face.append(vertices[indice]);
        }

        faces.append(face);
    }

    if (vertices.isEmpty() || faces.isEmpty()) {
        if (erro) {
            *erro = "O arquivo OBJ não contém vértices e faces válidos.";
        }
        return false;
    }

    objeto.nome = QFileInfo(caminhoArquivo).completeBaseName();
    objeto.faces = std::move(faces);
    return true;
}
