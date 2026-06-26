QT += widgets

CONFIG += c++17
TARGET = Visualizador3D

INCLUDEPATH += \
    $$PWD/src \
    $$PWD/src/core \
    $$PWD/src/ui

SOURCES += \
    src/main.cpp \
    src/core/leitorobj.cpp \
    src/core/matriz.cpp \
    src/core/numeros.cpp \
    src/core/objeto.cpp \
    src/ui/mainwindow.cpp \
    src/ui/meuframe.cpp

HEADERS += \
    src/core/leitorobj.h \
    src/core/matriz.h \
    src/core/numeros.h \
    src/core/objeto.h \
    src/ui/mainwindow.h \
    src/ui/meuframe.h

FORMS += \
    forms/mainwindow.ui

DISTFILES += \
    assets/models/*.obj \
    docs/CG_trab_2023_01.pdf

win32 {
    QMAKE_POST_LINK += if not exist $$shell_quote($$OUT_PWD/assets/models) $$QMAKE_MKDIR $$shell_quote($$OUT_PWD/assets/models) $$escape_expand(\n\t)
    QMAKE_POST_LINK += if not $$shell_quote($$PWD/assets/models)==$$shell_quote($$OUT_PWD/assets/models) $$QMAKE_COPY_DIR $$shell_quote($$PWD/assets/models) $$shell_quote($$OUT_PWD/assets/models) $$escape_expand(\n\t)
} else {
    QMAKE_POST_LINK += mkdir -p $$shell_quote($$OUT_PWD/assets/models) $$escape_expand(\n\t)
    QMAKE_POST_LINK += if [ $$shell_quote($$PWD/assets/models) != $$shell_quote($$OUT_PWD/assets/models) ]; then cp -R $$shell_quote($$PWD/assets/models/.) $$shell_quote($$OUT_PWD/assets/models); fi $$escape_expand(\n\t)
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target