#include "numeros.h"

#include <QLocale>
#include <cmath>

namespace {
bool tentarConverter(const QLocale &locale, const QString &texto, double &valor)
{
    bool ok = false;
    const double convertido = locale.toDouble(texto, &ok);
    if (ok && std::isfinite(convertido)) {
        valor = convertido;
        return true;
    }

    return false;
}
}

bool converterDoubleEntrada(const QString &texto, double &valor)
{
    const QString normalizado = texto.trimmed();
    if (normalizado.isEmpty()) {
        return false;
    }

    const bool temVirgula = normalizado.contains(',');
    const bool temPonto = normalizado.contains('.');

    if (temVirgula && !temPonto) {
        if (tentarConverter(QLocale(QLocale::Portuguese, QLocale::Brazil), normalizado, valor)) {
            return true;
        }

        QString comPonto = normalizado;
        comPonto.replace(',', '.');
        return tentarConverter(QLocale::c(), comPonto, valor);
    }

    if (temPonto && !temVirgula) {
        if (tentarConverter(QLocale::c(), normalizado, valor)) {
            return true;
        }
    }

    if (tentarConverter(QLocale::system(), normalizado, valor)) {
        return true;
    }

    if (tentarConverter(QLocale(QLocale::Portuguese, QLocale::Brazil), normalizado, valor)) {
        return true;
    }

    return tentarConverter(QLocale::c(), normalizado, valor);
}