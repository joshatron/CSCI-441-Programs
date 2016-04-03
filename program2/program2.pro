HEADERS += glwidget.h shape.h structure.h function.h linear_function.h sin_function.h cir_function.h
SOURCES += glwidget.cpp main.cpp shape.cpp structure.cpp

QT += opengl designer
CONFIG -= app_bundle
CONFIG += console c++11
INCLUDEPATH += "../include"
INCLUDEPATH += $$PWD

RESOURCES += shaders.qrc
