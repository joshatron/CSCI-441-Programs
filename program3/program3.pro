HEADERS += glwidget.h structure.h shape.h cir_function.h dome_function.h function.h linear_function.h sin_function.h tower_function.h
SOURCES += glwidget.cpp main.cpp structure.cpp shape.cpp

QT += opengl
CONFIG -= app_bundle
CONFIG += console c++11
INCLUDEPATH += "../include"
INCLUDEPATH += $$PWD

RESOURCES += shaders.qrc
