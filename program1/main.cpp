#include <QApplication>

#include "glwidget.h"

int main(int argc, char** argv) {
    QApplication a(argc, argv);

    QSurfaceFormat format;
    format.setVersion(3,3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    char const *imageFile = "pan.jpg";
    if(argc > 1)
    {
        imageFile = argv[1];
    }
    GLWidget glWidget(imageFile);
    qreal pixelRatio = glWidget.devicePixelRatio();
    glWidget.resize(640/pixelRatio,480/pixelRatio);
    glWidget.show();

    return a.exec();
}
