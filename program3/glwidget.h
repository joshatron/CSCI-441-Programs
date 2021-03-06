#ifndef __GLWIDGET__INCLUDE__
#define __GLWIDGET__INCLUDE__

#include <QGLWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QMouseEvent>
#include <QOpenGLTexture>
#include <QTimer>
#include <chrono>
#include <tinyobjloader/tiny_obj_loader.h>
#include <glm/glm.hpp>
#include "structure.h"
#include "door.h"
#include "fire.h"

#define GLM_FORCE_RADIANS

using glm::mat3;
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
        void keyPressEvent(QKeyEvent *event);
        void keyReleaseEvent(QKeyEvent *event);
        void wheelEvent(QWheelEvent *event);

    public slots:
    // Part 2 - add an animate slot
        void animate();

    private:
        void initializeCube();
        void initializeFace();
        void initializeModels();
        void initializeLights();
        void renderCube();
        void initializeSmoothModel(const tinyobj::shape_t &shape, vector<vec3> &positions, vector<vec2> & uvs, vector<vec3> &normals, vector<unsigned int> &indices);


        GLuint cubeProg;
        GLuint cubeVao;
        GLint cubeProjMatrixLoc;
        GLint cubeViewMatrixLoc;
        GLint cubeModelMatrixLoc;
        GLuint textureObject;

        void initializeGrid();
        void renderGrid();

        GLuint gridProg;
        GLuint gridVao;
        GLint gridProjMatrixLoc;
        GLint gridViewMatrixLoc;
        GLint gridModelMatrixLoc;

        GLuint modelProg;
        GLuint modelVao;
        GLint modelProjMatrixLoc;
        GLint modelViewMatrixLoc;
        GLint modelModelMatrixLoc;
        GLint modelColorLoc;
        GLint modelLightPosLoc;
        GLint modelLightColorLoc;
        GLint modelLightBrightnessLoc;
        GLint modelIndoorPosLoc;
        GLint modelIndoorBrightLoc;

        mat4 projMatrix;
        mat4 viewMatrix;
        mat4 modelMatrix;

        // Part 1 - Add two mat4 variables for pitch and yaw.
        // Also add two float variables for the pitch and yaw angles.
        mat4 pitch;
        mat4 yaw;
        double pitchAngle;
        double yawAngle;

        // Part 2 - Add a QTimer variable for our render loop.
        QTimer *timer;

        // Part 3 - Add state variables for keeping track
        //          of which movement keys are being pressed
        //        - Add two vec3 variables for position and velocity.
        //        - Add a variable for toggling fly mode

        bool forward;
        bool left;
        bool right;
        bool back;
        bool flyMode;
        bool move;

        vec3 position;
        vec3 velocity;

        int width;
        int height;

        glm::vec2 lastPt;
        void updateView();

        Structure structure;

        vec3 cube[24];
        vec3 normals[24];
        vec2 uvs[24];

        GLint cubeColorLoc;
        GLint cubeLightPosLoc;
        GLint cubeLightColorLoc;
        GLint cubeLightBrightnessLoc;
        GLint cubeIndoorPosLoc;
        GLint cubeIndoorBrightLoc;

        vec3 cubeColor;
        vec3 lightLoc;
        vec3 lightColor;
        float lightBrightness;

        bool mouseCaptured;
        bool collisions;
        bool headBob;
        bool goingUp;
        double upTime;
        double walkSpeed;

        vector<vec3> faces;
        vector<vec3> faceNormals;
        vector<vec2> faceUVs;
        vector<float> faceAmbients;
        vector<int> faceTextures;

        GLuint faceProg;
        GLuint faceVao;
        GLint faceProjMatrixLoc;
        GLint faceViewMatrixLoc;
        GLint faceColorLoc;
        GLint faceLightPosLoc;
        GLint faceLightColorLoc;
        GLint faceLightBrightnessLoc;
        GLint faceIndoorPosLoc;
        GLint faceIndoorBrightLoc;

        vector<Door> doors;

        vector<vec2> xLines;
        vector<vec2> zLines;

        std::chrono::time_point<std::chrono::system_clock> first, last, current;

        GLfloat indoorBright[24];
        bool brightening[24];
        double magnitude[24];

        QOpenGLTexture brickTex;
        QOpenGLTexture groundTex;
        QOpenGLTexture plankTex;
        QOpenGLTexture stoneTex;
        QOpenGLTexture doorTex;
        QOpenGLTexture chairTex;
        QOpenGLTexture bedTex;
        QOpenGLTexture tableTex;
        QOpenGLTexture torchTex;
        QOpenGLTexture throneTex;
        QOpenGLTexture couchTex;
        QOpenGLTexture bookTex;

        int numSmoothIndices;

        vector<mat4> modelTransforms;
        vector<int> modelIndices;
        vector<int> modelTexture;

        vector<Fire> fires;

        GLuint lightProg;
        GLuint lightVao;
        GLint lightProjMatrixLoc;
        GLint lightViewMatrixLoc;
        GLint lightModelMatrixLoc;
};

#endif
