#ifndef __GLWIDGET__INCLUDE__
#define __GLWIDGET__INCLUDE__

#include <QGLWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QMouseEvent>
#include <glm/glm.hpp>
#include "structure.h"
#include "shape.h"

#define GLM_FORCE_RADIANS

using glm::mat4;
using glm::vec3;

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core { 
    Q_OBJECT

    public:
        GLWidget(QWidget *parent=0);
        ~GLWidget();

        GLuint loadShaders(const char* vertf, const char* fragf);
    protected:
        void initializeGL();
        void resizeGL(int w, int h);
        void paintGL();

        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void wheelEvent(QWheelEvent *event);

    private:
        void initializeCube();
        void renderCube();
        void createCubes(int num);

        double dist;
        double brickWidth;
        double brickHeight;
        double brickDepth;
        double spacing;
        int wallDepth;

        vector<vec3> cubes;
        vector<vec3> normals;
        vec3 cubeColor;
        int numCubes;

        Structure structure;

        GLuint positionBuffer;
        GLuint normalBuffer;
        GLuint cubeProg;
        GLuint cubeVao;
        GLint cubeProjMatrixLoc;
        GLint cubeViewMatrixLoc;
        GLint cubeModelMatrixLoc;
        GLint cubeColorLoc;
        GLint cubeLightPosLoc;
        GLint cubeLightColorLoc;
        GLint cubeLightBrightnessLoc;

        void initializeGrid();
        void renderGrid();

        GLuint gridProg;
        GLuint gridVao;
        GLint gridProjMatrixLoc;
        GLint gridViewMatrixLoc;
        GLint gridModelMatrixLoc;

        mat4 projMatrix;
        mat4 viewMatrix;
        mat4 modelMatrix;

        int width;
        int height;

        glm::vec3 lastVPt;
        glm::vec3 pointOnVirtualTrackball(const glm::vec2 &pt);

        vec3 light;
};

#endif
