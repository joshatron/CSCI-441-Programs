#ifndef __GLWIDGET__INCLUDE__
#define __GLWIDGET__INCLUDE__

#ifndef GLM_FORCE_RADIANS
    #define GLM_FORCE_RADIANS
#endif

#include <QGLWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QMouseEvent>
#include <glm/glm.hpp>
#include "structure.h"
#include "shape.h"


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
        void keyPressEvent(QKeyEvent *event);

    private:
        void initializeLight();
        void initializeCube();
        void renderLight();
        void renderCube();
        void updateBuffer(int num);

        double dist;
        double brickWidth;
        double brickHeight;
        double brickDepth;
        double spacing;
        double scaleX;
        double scaleY;
        double scaleZ;
        int wallDepth;

        //0- zoom
        //1- brickWidth
        //2- brickHeight
        //3- brickDepth
        //4- uniform brick scale
        //5- scale x
        //6- scale y
        //7- scale z
        //8- uniform scale
        //9- scale x and z
        int scaler;

        vec3 cube[24];
        vec3 normals[24];
        vec3 cubeColor;
        int numCubes;

        mat4 lightTransform;
        vec3 lightLoc;
        vec3 lightColor;
        double lightBrightness;

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

        GLuint lightProg;
        GLuint lightVao;
        GLint lightProjMatrixLoc;
        GLint lightViewMatrixLoc;
        GLint lightModelMatrixLoc;
        GLint lightLightMatrixLoc;
        GLint lightColorLoc;
        GLint lightBrightnessLoc;

        mat4 lightMatrix;

        mat4 projMatrix;
        mat4 viewMatrix;
        mat4 modelMatrix;

        int width;
        int height;

        glm::vec3 lastVPt;
        glm::vec3 pointOnVirtualTrackball(const glm::vec2 &pt);

        vec3 tempLight;
};

#endif
