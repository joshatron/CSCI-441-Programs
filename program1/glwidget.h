#ifndef __GLWIDGET__INCLUDE__
#define __GLWIDGET__INCLUDE__

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QMouseEvent>
#include <glm/glm.hpp>
#include "shape.h"
#include <QtGui/QImage>

// glm by default uses degrees, but that functionality
// is deprecated so GLM_FORCE_RADIANS turns off some 
// glm warnings
#define GLM_FORCE_RADIANS

using glm::vec2;
using glm::vec3;

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core {
    Q_OBJECT

    public:
        GLWidget(char* image, QWidget *parent=0);
        ~GLWidget();

    protected:
        void initializeGL();
        void resizeGL(int w, int h);
        void paintGL();

        //when mouse pressed
        void mousePressEvent(QMouseEvent *event);
        //when key pressed
        void keyPressEvent(QKeyEvent *event);
        //when mouse dragged
        void mouseMoveEvent(QMouseEvent *event);
        //when scrolling
        void wheelEvent(QWheelEvent *event);

    private:
        GLuint loadShaders(const char* vertf, const char* fragf);
        static const GLchar* readShader(const char* filename);
        //clear screen of polygons
        void clearScreen();
        //fill screen uniformly
        void fillUniformly();
        //fill screen randomly
        void fillRandomly();
        //convert raw coordinate from mouse onto the image
        glm::vec2 rawLocToBaseLoc(int x, int y);
        //sets basic info for shapes
        void addShape(int x, int y);
        //creates individual points for shape
        void addShapePoints(Shape newShape);

        //the image
        QImage *img;

        GLuint vao;
        GLuint program;

        GLuint positionBuffer;
        GLuint colorBuffer;

        //points for polygons
        std::vector<vec2> shapes;
        //colors for those points
        std::vector<vec3> colors;
        //info for each polygon
        std::vector<Shape> allShapes;

        GLuint projectionBuffer;
        GLint projectionLoc;

        //the projection matrix
        glm::mat4 projection;

        //number of shapes to draw
        int num_shapes;

        //1 means a circle
        //2 means a group of lines
        //anything else is number of sides
        int sides;

        //0- always same
        //1- fit width
        //2- fit height
        //3- fit both
        int resizeMode;

        //toggle using polygon center or corners
        bool exactColor;
        //toggle polygons changing angle to follow mouse
        bool mouseFollow;
        //diameter of the polygons
        float size;

        //the width and height of the image
        int baseWidth;
        int baseHeight;

        //the width and height of the window
        int width;
        int height;

        //the last x and y that a polygon was drawn
        int lastX;
        int lastY;
};

#endif
