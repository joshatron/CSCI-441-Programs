#ifndef __GLWIDGET__INCLUDE__
#define __GLWIDGET__INCLUDE__

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QMouseEvent>
#include <glm/glm.hpp>
#include "shape.h"

// glm by default uses degrees, but that functionality
// is deprecated so GLM_FORCE_RADIANS turns off some 
// glm warnings
#define GLM_FORCE_RADIANS

using glm::vec2;
using glm::vec3;

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    public:
        GLWidget(QWidget *parent=0);
        ~GLWidget();

    protected:
        void initializeGL();
        void resizeGL(int w, int h);
        void paintGL();

        void mousePressEvent(QMouseEvent *event);
        void keyPressEvent(QKeyEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void wheelEvent(QWheelEvent *event);

    private:
        GLuint loadShaders(const char* vertf, const char* fragf);
        static const GLchar* readShader(const char* filename);
        void clearScreen();
        void fillUniformly();
        void fillRandomly();
        glm::vec2 rawLocToBaseLoc(int x, int y);
        void addShape(int x, int y);
        void addShapePoints(Shape newShape);

        GLuint vao;
        GLuint program;

        GLuint positionBuffer;
        bool outline;

        GLuint colorBuffer;

        std::vector<vec2> shapes;
        std::vector<vec3> colors;
        std::vector<Shape> allShapes;

        GLuint projectionBuffer;
        GLint projectionLoc;

        glm::mat4 projection;

        int num_shapes;

        //0- square
        //1- circle
        //2- triangle
        //3- lines
        int shape;

        //0- always same
        //1- fit width
        //2- fit height
        //3- fit both
        int resizeMode;

        bool background;
        bool exactColor;
        bool mouseFollow;
        float size;

        int baseWidth;
        int baseHeight;

        int width;
        int height;

        int lastX;
        int lastY;
};

#endif
