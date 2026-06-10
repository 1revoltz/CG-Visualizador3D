#ifndef LEITOROBJ_H
#define LEITOROBJ_H

#include <QString>

#include "objeto.h"

class LeitorObj
{
public:
    static bool carregar(const QString &caminhoArquivo, Objeto &objeto, QString *erro = nullptr);
};

#endif // LEITOROBJ_H
