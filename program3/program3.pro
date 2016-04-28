HEADERS += glwidget.h structure.h shape.h cir_function.h dome_function.h function.h linear_function.h sin_function.h tower_function.h door.h particle.h fire.h
SOURCES += glwidget.cpp main.cpp structure.cpp shape.cpp door.cpp ../include/tinyobjloader/tiny_obj_loader.cc fire.cpp

QT += opengl
CONFIG -= app_bundle
CONFIG += console c++11
INCLUDEPATH += "../include"
INCLUDEPATH += $$PWD

RESOURCES += shaders.qrc
