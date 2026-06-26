QT += core gui

CONFIG += console c++17
CONFIG -= app_bundle
TEMPLATE = app
TARGET = escalonamento_bancada

INCLUDEPATH += \
    ../src/core

SOURCES += \
    escalonamento_bancada.cpp \
    ../src/core/matriz.cpp \
    ../src/core/numeros.cpp \
    ../src/core/objeto.cpp

HEADERS += \
    ../src/core/matriz.h \
    ../src/core/numeros.h \
    ../src/core/objeto.h