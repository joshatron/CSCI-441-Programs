#include "glwidget.h"
#include <iostream>
#include <QOpenGLTexture>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <QTextStream>
#include <QTimer>
#include <memory>
#include "tower_function.h"

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

using glm::inverse;
using glm::vec2;
using glm::vec3;
using glm::mat3;
using glm::mat4;
using glm::perspective;
using glm::normalize;
using glm::length;
using glm::cross;
using glm::dot;
using glm::rotate;
using glm::value_ptr;
using glm::lookAt;
using std::cout;
using std::endl;
using std::make_shared;
using std::max;
using std::min;

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {

    setMouseTracking(true);
    timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(16);

    forward = false;
    left = false;
    right = false;
    back = false;
    flyMode = false;

    mouseCaptured = false;
    collisions = true;
    headBob = true;
    goingUp = false;
    upTime = 0;
    walkSpeed = 40;

    doors.push_back(Door(vec3(10,0,.5), true, M_PI * 3 / 2, M_PI));
    doors.push_back(Door(vec3(-10,0,.5), false, M_PI * 2, M_PI * 3 / 2));
    doors.push_back(Door(vec3(10,0,-79), true, M_PI * 3 / 2, M_PI));
    doors.push_back(Door(vec3(-10,0,-79), false, M_PI * 2, M_PI * 3 / 2));
    doors.push_back(Door(vec3(-21,0,-35), true, M_PI, M_PI / 2));
    doors.push_back(Door(vec3(-21,0,45), true, M_PI, M_PI / 2));
    doors.push_back(Door(vec3(21,0,-35), false, M_PI / 2, 0));
    doors.push_back(Door(vec3(21,0,45), false, M_PI / 2, 0));

    //outer wall
    structure.shapes.push_back(Shape(1,1,1,4,vec3(0, 0, 0),vec3(0,0,0),true,0,1,1,make_shared<Function>()));

    //towers
    structure.shapes.push_back(Shape(.2,.2,1.5,12,vec3(.35, 0, .35),vec3(0,0,0),true,0,1,1,make_shared<TowerFunction>()));
    structure.shapes.push_back(Shape(.2,.2,1.5,12,vec3(-.35, 0, .35),vec3(0,0,0),true,0,1,1,make_shared<TowerFunction>()));
    structure.shapes.push_back(Shape(.2,.2,1.5,12,vec3(-.35, 0, -.35),vec3(0,0,0),true,0,1,1,make_shared<TowerFunction>()));
    structure.shapes.push_back(Shape(.2,.2,1.5,12,vec3(.35, 0, -.35),vec3(0,0,0),true,0,1,1,make_shared<TowerFunction>()));

    structure.updateBrickLocs(4, 2, 2, .01, 100, 25, 100, 1);

    //initialize cube vertices
    cube[0] = vec3(.5,.5,.5);
    cube[1] = vec3(.5,.5,-.5);
    cube[2] = vec3(-.5,.5,-.5);
    cube[3] = vec3(-.5,.5,.5);
    cube[4] = vec3(.5,-.5,.5);
    cube[5] = vec3(-.5,-.5,.5);
    cube[6] = vec3(-.5,-.5,-.5);
    cube[7] = vec3(.5,-.5,-.5);
    cube[8] = vec3(.5,.5,.5);
    cube[9] = vec3(-.5,.5,.5);
    cube[10] = vec3(-.5,-.5,.5);
    cube[11] = vec3(.5,-.5,.5);
    cube[12] = vec3(-.5,-.5,-.5);
    cube[13] = vec3(-.5,.5,-.5);
    cube[14] = vec3(.5,.5,-.5);
    cube[15] = vec3(.5,-.5,-.5);
    cube[16] = vec3(.5,-.5,.5);
    cube[17] = vec3(.5,-.5,-.5);
    cube[18] = vec3(.5,.5,-.5);
    cube[19] = vec3(.5,.5,.5);
    cube[20] = vec3(-.5,-.5,.5);
    cube[21] = vec3(-.5,.5,.5);
    cube[22] = vec3(-.5,.5,-.5);
    cube[23] = vec3(-.5,-.5,-.5);

    //initialize normals
    normals[0] = vec3(0,1,0);
    normals[1] = vec3(0,1,0);
    normals[2] = vec3(0,1,0);
    normals[3] = vec3(0,1,0);
    normals[4] = vec3(0,-1,0);
    normals[5] = vec3(0,-1,0);
    normals[6] = vec3(0,-1,0);
    normals[7] = vec3(0,-1,0);
    normals[8] = vec3(0, 0, 1);
    normals[9] = vec3(0, 0, 1);
    normals[10] = vec3(0, 0, 1);
    normals[11] = vec3(0, 0, 1);
    normals[12] = vec3(0, 0, -1);
    normals[13] = vec3(0, 0, -1);
    normals[14] = vec3(0, 0, -1);
    normals[15] = vec3(0, 0, -1);
    normals[16] = vec3(1, 0, 0);
    normals[17] = vec3(1, 0, 0);
    normals[18] = vec3(1, 0, 0);
    normals[19] = vec3(1, 0, 0);
    normals[20] = vec3(-1, 0, 0);
    normals[21] = vec3(-1, 0, 0);
    normals[22] = vec3(-1, 0, 0);
    normals[23] = vec3(-1, 0, 0);

    cubeColor = vec3(1,1,1);
    lightColor = vec3(1,1,1);
    lightBrightness = 1;
    lightLoc = vec3(0,10,0);

    pitchAngle = 0;
    yawAngle = 0;
    position = vec3(0,14,0);

    xLines.push_back(vec2(79.5,79.5));
    xLines.push_back(vec2(79.5,-79.5));

    xLines.push_back(vec2(-79.5,79.5));
    xLines.push_back(vec2(-79.5,-79.5));

    xLines.push_back(vec2(-20.5,79.5));
    xLines.push_back(vec2(-20.5,45));
    xLines.push_back(vec2(-20.5,35));
    xLines.push_back(vec2(-20.5,-35));
    xLines.push_back(vec2(-20.5,-79.5));
    xLines.push_back(vec2(-20.5,-45));

    xLines.push_back(vec2(20.5,79.5));
    xLines.push_back(vec2(20.5,45));
    xLines.push_back(vec2(20.5,35));
    xLines.push_back(vec2(20.5,-35));
    xLines.push_back(vec2(20.5,-79.5));
    xLines.push_back(vec2(20.5,-45));

    xLines.push_back(vec2(200,200));
    xLines.push_back(vec2(200,-200));

    xLines.push_back(vec2(-200,200));
    xLines.push_back(vec2(-200,-200));

    xLines.push_back(vec2(-160,160));
    xLines.push_back(vec2(-160,120));
    xLines.push_back(vec2(-120,160));
    xLines.push_back(vec2(-120,120));

    xLines.push_back(vec2(-160,-160));
    xLines.push_back(vec2(-160,-120));
    xLines.push_back(vec2(-120,-160));
    xLines.push_back(vec2(-120,-120));

    xLines.push_back(vec2(160,160));
    xLines.push_back(vec2(160,120));
    xLines.push_back(vec2(120,160));
    xLines.push_back(vec2(120,120));

    xLines.push_back(vec2(160,-160));
    xLines.push_back(vec2(160,-120));
    xLines.push_back(vec2(120,-160));
    xLines.push_back(vec2(120,-120));


    zLines.push_back(vec2(79.5,79.5));
    zLines.push_back(vec2(-79.5,79.5));

    zLines.push_back(vec2(79.5,-79.5));
    zLines.push_back(vec2(10,-79.5));
    zLines.push_back(vec2(-79.5,-79.5));
    zLines.push_back(vec2(-10,-79.5));

    zLines.push_back(vec2(79.5,0));
    zLines.push_back(vec2(10,0));
    zLines.push_back(vec2(-79.5,0));
    zLines.push_back(vec2(-10,0));

    zLines.push_back(vec2(200,200));
    zLines.push_back(vec2(-200,200));

    zLines.push_back(vec2(200,-200));
    zLines.push_back(vec2(-200,-200));

    zLines.push_back(vec2(160,160));
    zLines.push_back(vec2(120,160));
    zLines.push_back(vec2(160,120));
    zLines.push_back(vec2(120,120));

    zLines.push_back(vec2(-160,160));
    zLines.push_back(vec2(-120,160));
    zLines.push_back(vec2(-160,120));
    zLines.push_back(vec2(-120,120));

    zLines.push_back(vec2(160,-160));
    zLines.push_back(vec2(120,-160));
    zLines.push_back(vec2(160,-120));
    zLines.push_back(vec2(120,-120));

    zLines.push_back(vec2(-160,-160));
    zLines.push_back(vec2(-120,-160));
    zLines.push_back(vec2(-160,-120));
    zLines.push_back(vec2(-120,-120));

    first = std::chrono::system_clock::now();
    last = std::chrono::system_clock::now();
}

GLWidget::~GLWidget() {
}


void GLWidget::animate() {
    current = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = current - last;
    double elapsed = elapsed_seconds.count();
    last = current;

    bool move = false;
    velocity = vec3(0,0,0);
    if (forward && !back) {
        velocity.z -= 1;
        move = true;
    }
    if (back && !forward) {
        velocity.z += 1;
        move = true;
    }
    if (left && !right) {
        velocity.x -= 1;
        move = true;
    }
    if (right && !left) {
        velocity.x += 1;
        move = true;
    }

    if(move)
    {
        if (flyMode) {
            velocity = vec3(pitch * yaw * glm::vec4(velocity, 1.0f));
        } else {
            if(headBob)
            {
                double rate = .8 + (walkSpeed / 250);
                double time = .355 - (walkSpeed / 250);
                if(goingUp)
                {
                    velocity.y += rate * elapsed;
                    upTime += elapsed;
                    if(upTime > time)
                    {
                        upTime = 0;
                        goingUp = false;
                    }
                }
                else
                {
                    velocity.y -= rate * elapsed;
                    upTime += elapsed;
                    if(upTime > time)
                    {
                        upTime = 0;
                        goingUp = true;
                    }
                }
            }
            velocity = vec3(pitch * glm::vec4(velocity, 1.0f));
        }

        velocity = normalize(velocity);
        velocity *= walkSpeed;
    }

    for(unsigned int k = 0; k < doors.size(); k++)
    {
        if(distance(doors.at(k).fulcrumPoint, position) < 40)
        {
            doors.at(k).updateAngle(true, elapsed);
        }
        else
        {
            doors.at(k).updateAngle(false, elapsed);
        }
    }

    position = position + velocity * (float) elapsed;

    if(collisions && !flyMode)
    {
        double collisionBuffer = 5;

        for(unsigned int k = 0; k < xLines.size(); k += 2)
        {
            if(position.z < max(xLines.at(k).y, xLines.at(k + 1).y) &&
               position.z > min(xLines.at(k).y, xLines.at(k + 1).y))
            {
                if(position.x < xLines.at(k).x && position.x > xLines.at(k).x - collisionBuffer)
                {
                    position.x = xLines.at(k).x - collisionBuffer;
                }
                else if(position.x > xLines.at(k).x && position.x < xLines.at(k).x + collisionBuffer)
                {
                    position.x = xLines.at(k).x + collisionBuffer;
                }
            }
        }

        for(unsigned int k = 0; k < zLines.size(); k += 2)
        {
            if(position.x < max(zLines.at(k).x, zLines.at(k + 1).x) &&
               position.x > min(zLines.at(k).x, zLines.at(k + 1).x))
            {
                if(position.z < zLines.at(k).y && position.z > zLines.at(k).y - collisionBuffer)
                {
                    position.z = zLines.at(k).y - collisionBuffer;
                }
                else if(position.z > zLines.at(k).y && position.z < zLines.at(k).y + collisionBuffer)
                {
                    position.z = zLines.at(k).y + collisionBuffer;
                }
            }
        }
    }

    mat4 translateMatrix = glm::translate(mat4(1.0f), position);

    viewMatrix = inverse(translateMatrix * pitch * yaw);

    glUseProgram(cubeProg);
    glUniformMatrix4fv(cubeViewMatrixLoc, 1, false, value_ptr(viewMatrix));

    glUseProgram(gridProg);
    glUniformMatrix4fv(gridViewMatrixLoc, 1, false, value_ptr(viewMatrix));

    glUseProgram(faceProg);
    glUniformMatrix4fv(faceViewMatrixLoc, 1, false, value_ptr(viewMatrix));

    update();
}

void GLWidget::initializeGrid() {
    glGenVertexArrays(1, &gridVao);
    glBindVertexArray(gridVao);

    // Create a buffer on the GPU for position data
    GLuint positionBuffer;
    glGenBuffers(1, &positionBuffer);

    vec3 pts[84];
    for(int i = -10; i <= 10; i++) {

        pts[2*(i+10)] = vec3(i, -.5f, 10);
        pts[2*(i+10)+1] = vec3(i, -.5f, -10);

        pts[2*(i+10)+42] = vec3(10,-.5f, i);
        pts[2*(i+10)+43] = vec3(-10,-.5f, i);
    }

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pts), pts, GL_STATIC_DRAW);

    // Load our vertex and fragment shaders into a program object
    // on the GPU
    GLuint program = loadShaders(":/grid_vert.glsl", ":/grid_frag.glsl");
    glUseProgram(program);
    gridProg = program;

    // Bind the attribute "position" (defined in our vertex shader)
    // to the currently bound buffer object, which contains our
    // position data for a single triangle. This information 
    // is stored in our vertex array object.
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    GLint positionIndex = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(positionIndex);
    glVertexAttribPointer(positionIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);

    gridProjMatrixLoc = glGetUniformLocation(program, "projection");
    gridViewMatrixLoc = glGetUniformLocation(program, "view");
    gridModelMatrixLoc = glGetUniformLocation(program, "model");
}

void GLWidget::initializeCube() {
    // Create a new Vertex Array Object on the GPU which
    // saves the attribute layout of our vertices.
    glGenVertexArrays(1, &cubeVao);
    glBindVertexArray(cubeVao);

    // Create a buffer on the GPU for position data
    GLuint positionBuffer;
    glGenBuffers(1, &positionBuffer);
    GLuint normalBuffer;
    glGenBuffers(1, &normalBuffer);

    // Upload the position data to the GPU, storing
    // it in the buffer we just allocated.
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

    // Load our vertex and fragment shaders into a program object
    // on the GPU
    GLuint program = loadShaders(":/vert.glsl", ":/frag.glsl");
    glUseProgram(program);
    cubeProg = program;

    // Bind the attribute "position" (defined in our vertex shader)
    // to the currently bound buffer object, which contains our
    // position data for a single triangle. This information 
    // is stored in our vertex array object.
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    GLint positionIndex = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(positionIndex);
    glVertexAttribPointer(positionIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // assign normal data
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    GLint normalIndex = glGetAttribLocation(program, "normal");
    glEnableVertexAttribArray(normalIndex);
    glVertexAttribPointer(normalIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);

    cubeProjMatrixLoc = glGetUniformLocation(program, "projection");
    cubeViewMatrixLoc = glGetUniformLocation(program, "view");
    cubeModelMatrixLoc = glGetUniformLocation(program, "model");
    cubeColorLoc = glGetUniformLocation(program, "color");
    cubeLightPosLoc = glGetUniformLocation(program, "lightPos");
    cubeLightColorLoc = glGetUniformLocation(program, "lightColor");
    cubeLightBrightnessLoc = glGetUniformLocation(program, "lightBrightness");

    glUniform3fv(cubeLightColorLoc, 1, value_ptr(lightColor));
    glUniform1f(cubeLightBrightnessLoc, lightBrightness);
    glUniform3fv(cubeLightPosLoc, 1, value_ptr(lightLoc));
    glUniform3fv(cubeColorLoc, 1, value_ptr(cubeColor));
}

void GLWidget::initializeFace()
{
    float outsideAmbient = .8;
    float insideAmbient = .1;
    //ground
    faces.push_back(vec3(200,0,200));
    faces.push_back(vec3(200,0,-200));
    faces.push_back(vec3(-200,0,-200));
    faces.push_back(vec3(-200,0,200));
    faceNormals.push_back(vec3(0,1,0));
    faceNormals.push_back(vec3(0,1,0));
    faceNormals.push_back(vec3(0,1,0));
    faceNormals.push_back(vec3(0,1,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);

    //building
    //outside
    faces.push_back(vec3(-80,40,-80));
    faces.push_back(vec3(-80,40,80));
    faces.push_back(vec3(-80,0,80));
    faces.push_back(vec3(-80,0,-80));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);

    faces.push_back(vec3(80,40,-80));
    faces.push_back(vec3(80,40,80));
    faces.push_back(vec3(80,0,80));
    faces.push_back(vec3(80,0,-80));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);

    faces.push_back(vec3(-80,40,80));
    faces.push_back(vec3(80,40,80));
    faces.push_back(vec3(80,0,80));
    faces.push_back(vec3(-80,0,80));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);

    faces.push_back(vec3(-80,40,-80));
    faces.push_back(vec3(80,40,-80));
    faces.push_back(vec3(80,20,-80));
    faces.push_back(vec3(-80,20,-80));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);

    faces.push_back(vec3(-80,20,-80));
    faces.push_back(vec3(-10,20,-80));
    faces.push_back(vec3(-10,0,-80));
    faces.push_back(vec3(-80,0,-80));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);

    faces.push_back(vec3(80,20,-80));
    faces.push_back(vec3(10,20,-80));
    faces.push_back(vec3(10,0,-80));
    faces.push_back(vec3(80,0,-80));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);
    faceAmbients.push_back(outsideAmbient);

    //inside
    faces.push_back(vec3(-79,40,-80));
    faces.push_back(vec3(-79,40,80));
    faces.push_back(vec3(-79,0,80));
    faces.push_back(vec3(-79,0,-80));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(79,40,-80));
    faces.push_back(vec3(79,40,80));
    faces.push_back(vec3(79,0,80));
    faces.push_back(vec3(79,0,-80));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-80,40,79));
    faces.push_back(vec3(80,40,79));
    faces.push_back(vec3(80,0,79));
    faces.push_back(vec3(-80,0,79));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-80,40,-79));
    faces.push_back(vec3(80,40,-79));
    faces.push_back(vec3(80,20,-79));
    faces.push_back(vec3(-80,20,-79));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-80,20,-79));
    faces.push_back(vec3(-10,20,-79));
    faces.push_back(vec3(-10,0,-79));
    faces.push_back(vec3(-80,0,-79));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(80,20,-79));
    faces.push_back(vec3(10,20,-79));
    faces.push_back(vec3(10,0,-79));
    faces.push_back(vec3(80,0,-79));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-80,40,-.5));
    faces.push_back(vec3(80,40,-.5));
    faces.push_back(vec3(80,20,-.5));
    faces.push_back(vec3(-80,20,-.5));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-80,20,-.5));
    faces.push_back(vec3(-10,20,-.5));
    faces.push_back(vec3(-10,0,-.5));
    faces.push_back(vec3(-80,0,-.5));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(80,20,-.5));
    faces.push_back(vec3(10,20,-.5));
    faces.push_back(vec3(10,0,-.5));
    faces.push_back(vec3(80,0,-.5));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-80,40,.5));
    faces.push_back(vec3(80,40,.5));
    faces.push_back(vec3(80,20,.5));
    faces.push_back(vec3(-80,20,.5));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-80,20,.5));
    faces.push_back(vec3(-10,20,.5));
    faces.push_back(vec3(-10,0,.5));
    faces.push_back(vec3(-80,0,.5));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(80,20,.5));
    faces.push_back(vec3(10,20,.5));
    faces.push_back(vec3(10,0,.5));
    faces.push_back(vec3(80,0,.5));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-20,40,-80));
    faces.push_back(vec3(-20,40,80));
    faces.push_back(vec3(-20,20,80));
    faces.push_back(vec3(-20,20,-80));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(20,40,-80));
    faces.push_back(vec3(20,40,80));
    faces.push_back(vec3(20,20,80));
    faces.push_back(vec3(20,20,-80));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-21,40,-80));
    faces.push_back(vec3(-21,40,80));
    faces.push_back(vec3(-21,20,80));
    faces.push_back(vec3(-21,20,-80));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(21,40,-80));
    faces.push_back(vec3(21,40,80));
    faces.push_back(vec3(21,20,80));
    faces.push_back(vec3(21,20,-80));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-21,20,-80));
    faces.push_back(vec3(-21,20,-45));
    faces.push_back(vec3(-21,0,-45));
    faces.push_back(vec3(-21,0,-80));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(21,20,-80));
    faces.push_back(vec3(21,20,-45));
    faces.push_back(vec3(21,0,-45));
    faces.push_back(vec3(21,0,-80));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-20,20,-80));
    faces.push_back(vec3(-20,20,-45));
    faces.push_back(vec3(-20,0,-45));
    faces.push_back(vec3(-20,0,-80));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(20,20,-80));
    faces.push_back(vec3(20,20,-45));
    faces.push_back(vec3(20,0,-45));
    faces.push_back(vec3(20,0,-80));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-21,20,80));
    faces.push_back(vec3(-21,20,45));
    faces.push_back(vec3(-21,0,45));
    faces.push_back(vec3(-21,0,80));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(21,20,80));
    faces.push_back(vec3(21,20,45));
    faces.push_back(vec3(21,0,45));
    faces.push_back(vec3(21,0,80));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-20,20,80));
    faces.push_back(vec3(-20,20,45));
    faces.push_back(vec3(-20,0,45));
    faces.push_back(vec3(-20,0,80));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(20,20,80));
    faces.push_back(vec3(20,20,45));
    faces.push_back(vec3(20,0,45));
    faces.push_back(vec3(20,0,80));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-21,20,35));
    faces.push_back(vec3(-21,20,-35));
    faces.push_back(vec3(-21,0,-35));
    faces.push_back(vec3(-21,0,35));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(21,20,35));
    faces.push_back(vec3(21,20,-35));
    faces.push_back(vec3(21,0,-35));
    faces.push_back(vec3(21,0,35));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-20,20,35));
    faces.push_back(vec3(-20,20,-35));
    faces.push_back(vec3(-20,0,-35));
    faces.push_back(vec3(-20,0,35));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(20,20,35));
    faces.push_back(vec3(20,20,-35));
    faces.push_back(vec3(20,0,-35));
    faces.push_back(vec3(20,0,35));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(80,40,80));
    faces.push_back(vec3(80,40,-80));
    faces.push_back(vec3(-80,40,-80));
    faces.push_back(vec3(-80,40,80));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    //inside doors
    faces.push_back(vec3(20,20,35));
    faces.push_back(vec3(21,20,35));
    faces.push_back(vec3(21,0,35));
    faces.push_back(vec3(20,0,35));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(20,20,-35));
    faces.push_back(vec3(21,20,-35));
    faces.push_back(vec3(21,0,-35));
    faces.push_back(vec3(20,0,-35));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(20,20,45));
    faces.push_back(vec3(21,20,45));
    faces.push_back(vec3(21,0,45));
    faces.push_back(vec3(20,0,45));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(20,20,-45));
    faces.push_back(vec3(21,20,-45));
    faces.push_back(vec3(21,0,-45));
    faces.push_back(vec3(20,0,-45));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-20,20,35));
    faces.push_back(vec3(-21,20,35));
    faces.push_back(vec3(-21,0,35));
    faces.push_back(vec3(-20,0,35));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-20,20,-35));
    faces.push_back(vec3(-21,20,-35));
    faces.push_back(vec3(-21,0,-35));
    faces.push_back(vec3(-20,0,-35));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-20,20,45));
    faces.push_back(vec3(-21,20,45));
    faces.push_back(vec3(-21,0,45));
    faces.push_back(vec3(-20,0,45));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceNormals.push_back(vec3(0,0,-1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-20,20,-45));
    faces.push_back(vec3(-21,20,-45));
    faces.push_back(vec3(-21,0,-45));
    faces.push_back(vec3(-20,0,-45));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceNormals.push_back(vec3(0,0,1));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-10,20,-.5));
    faces.push_back(vec3(-10,20,.5));
    faces.push_back(vec3(-10,0,.5));
    faces.push_back(vec3(-10,0,-.5));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(10,20,-.5));
    faces.push_back(vec3(10,20,.5));
    faces.push_back(vec3(10,0,.5));
    faces.push_back(vec3(10,0,-.5));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-10,20,-79));
    faces.push_back(vec3(-10,20,-80));
    faces.push_back(vec3(-10,0,-80));
    faces.push_back(vec3(-10,0,-79));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceNormals.push_back(vec3(1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(10,20,-79));
    faces.push_back(vec3(10,20,-80));
    faces.push_back(vec3(10,0,-80));
    faces.push_back(vec3(10,0,-79));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceNormals.push_back(vec3(-1,0,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(10,20,-79));
    faces.push_back(vec3(10,20,-80));
    faces.push_back(vec3(-10,20,-80));
    faces.push_back(vec3(-10,20,-79));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(10,20,-.5));
    faces.push_back(vec3(10,20,.5));
    faces.push_back(vec3(-10,20,.5));
    faces.push_back(vec3(-10,20,-.5));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(20,20,-35));
    faces.push_back(vec3(21,20,-35));
    faces.push_back(vec3(21,20,-45));
    faces.push_back(vec3(20,20,-45));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(20,20,35));
    faces.push_back(vec3(21,20,35));
    faces.push_back(vec3(21,20,45));
    faces.push_back(vec3(20,20,45));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-20,20,-35));
    faces.push_back(vec3(-21,20,-35));
    faces.push_back(vec3(-21,20,-45));
    faces.push_back(vec3(-20,20,-45));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    faces.push_back(vec3(-20,20,35));
    faces.push_back(vec3(-21,20,35));
    faces.push_back(vec3(-21,20,45));
    faces.push_back(vec3(-20,20,45));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceNormals.push_back(vec3(0,-1,0));
    faceUVs.push_back(vec2(0,0));
    faceUVs.push_back(vec2(1,0));
    faceUVs.push_back(vec2(1,1));
    faceUVs.push_back(vec2(0,1));
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);
    faceAmbients.push_back(insideAmbient);

    glGenVertexArrays(1, &faceVao);
    glBindVertexArray(faceVao);

    // Create a buffer on the GPU for position data
    GLuint positionBuffer;
    glGenBuffers(1, &positionBuffer);
    GLuint normalBuffer;
    glGenBuffers(1, &normalBuffer);
    GLuint uvBuffer;
    glGenBuffers(1, &uvBuffer);
    GLuint ambientBuffer;
    glGenBuffers(1, &ambientBuffer);

    // Upload the position data to the GPU, storing
    // it in the buffer we just allocated.
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * faces.size(), faces.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * faceNormals.size(), faceNormals.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * faceUVs.size(), faceUVs.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, ambientBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * faceAmbients.size(), faceAmbients.data(), GL_STATIC_DRAW);

    // Load our vertex and fragment shaders into a program object
    // on the GPU
    GLuint program = loadShaders(":/face_vert.glsl", ":/face_frag.glsl");
    glUseProgram(program);
    faceProg = program;

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    GLint positionIndex = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(positionIndex);
    glVertexAttribPointer(positionIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    GLint normalIndex = glGetAttribLocation(program, "normal");
    glEnableVertexAttribArray(normalIndex);
    glVertexAttribPointer(normalIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    GLint uvIndex = glGetAttribLocation(program, "uv");
    glEnableVertexAttribArray(uvIndex);
    glVertexAttribPointer(uvIndex, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, ambientBuffer);
    GLint ambientIndex = glGetAttribLocation(program, "ambient");
    glEnableVertexAttribArray(ambientIndex);
    glVertexAttribPointer(ambientIndex, 1, GL_FLOAT, GL_FALSE, 0, 0);

    faceProjMatrixLoc = glGetUniformLocation(program, "projection");
    faceViewMatrixLoc = glGetUniformLocation(program, "view");
    faceColorLoc = glGetUniformLocation(program, "color");
    faceLightPosLoc = glGetUniformLocation(program, "lightPos");
    faceLightColorLoc = glGetUniformLocation(program, "lightColor");
    faceLightBrightnessLoc = glGetUniformLocation(program, "lightBrightness");

    glUniform3fv(faceLightColorLoc, 1, value_ptr(lightColor));
    glUniform1f(faceLightBrightnessLoc, lightBrightness);
    glUniform3fv(faceLightPosLoc, 1, value_ptr(lightLoc));
    glUniform3fv(faceColorLoc, 1, value_ptr(vec3(1,1,1)));
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();

    glClearColor(0.0f/255.f, 191.0f/255.f, 255.0f/250.f, 1.0f);
    glPointSize(4.0f);

    glEnable(GL_DEPTH_TEST);
    GLuint restart = 0xFFFFFFFF;
    glPrimitiveRestartIndex(restart);
    glEnable(GL_PRIMITIVE_RESTART);

    initializeCube();
    initializeGrid();
    initializeFace();

    viewMatrix = mat4(1.0f);
    modelMatrix = mat4(1.0f);

    glUseProgram(cubeProg);
    glUniformMatrix4fv(cubeViewMatrixLoc, 1, false, value_ptr(viewMatrix));
    glUniformMatrix4fv(cubeModelMatrixLoc, 1, false, value_ptr(modelMatrix));

    glUseProgram(gridProg);
    glUniformMatrix4fv(gridViewMatrixLoc, 1, false, value_ptr(viewMatrix));
    glUniformMatrix4fv(gridModelMatrixLoc, 1, false, value_ptr(modelMatrix));

    glUseProgram(faceProg);
    glUniformMatrix4fv(faceViewMatrixLoc, 1, false, value_ptr(viewMatrix));
}

void GLWidget::resizeGL(int w, int h) {
    width = w;
    height = h;

    float aspect = (float)w/h;

    projMatrix = perspective(45.0f, aspect, .01f, 400.0f);

    glUseProgram(cubeProg);
    glUniformMatrix4fv(cubeProjMatrixLoc, 1, false, value_ptr(projMatrix));

    glUseProgram(gridProg);
    glUniformMatrix4fv(gridProjMatrixLoc, 1, false, value_ptr(projMatrix));

    glUseProgram(faceProg);
    glUniformMatrix4fv(faceProjMatrixLoc, 1, false, value_ptr(projMatrix));
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderGrid();
    renderCube();

    glUseProgram(faceProg);
    glBindVertexArray(faceVao);
    for(int k = 0; k < faces.size() / 4; k++)
    {
        glDrawArrays(GL_TRIANGLE_FAN, k * 4, 4);
    }
}

void GLWidget::renderCube() {
    glUseProgram(cubeProg);
    glBindVertexArray(cubeVao);
    //for each brickLoc
    for(unsigned int k = 0; k < structure.brickLocs.size(); k++)
    {
        //get transform and upload to shader
        mat4 loc = modelMatrix * structure.brickLocs.at(k) * structure.brickShape;
        glUniformMatrix4fv(cubeModelMatrixLoc, 1, false, value_ptr(loc));
        //for each face, render it
        for(int a = 0; a < 6; a++)
        {
            glDrawArrays(GL_TRIANGLE_FAN, a * 4, 4);
        }
    }

    for(unsigned int k = 0; k < doors.size(); k++)
    {
        mat4 loc = doors.at(k).getTransform();
        glUniformMatrix4fv(cubeModelMatrixLoc, 1, false, value_ptr(loc));
        //for each face, render it
        for(int a = 0; a < 6; a++)
        {
            glDrawArrays(GL_TRIANGLE_FAN, a * 4, 4);
        }
    }
}

void GLWidget::renderGrid() {
    glUseProgram(gridProg);
    glBindVertexArray(gridVao);
    glDrawArrays(GL_LINES, 0, 84);
}

GLuint GLWidget::loadShaders(const char* vertf, const char* fragf) {
    GLuint program = glCreateProgram();

    // read vertex shader from Qt resource file
    QFile vertFile(vertf);
    vertFile.open(QFile::ReadOnly | QFile::Text);
    QString vertString;
    QTextStream vertStream(&vertFile);
    vertString.append(vertStream.readAll());
    std::string vertSTLString = vertString.toStdString();

    const GLchar* vertSource = vertSTLString.c_str();

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertSource, NULL);
    glCompileShader(vertShader);
    {
        GLint compiled;
        glGetShaderiv( vertShader, GL_COMPILE_STATUS, &compiled );
        if ( !compiled ) {
            GLsizei len;
            glGetShaderiv( vertShader, GL_INFO_LOG_LENGTH, &len );

            GLchar* log = new GLchar[len+1];
            glGetShaderInfoLog( vertShader, len, &len, log );
            std::cout << "Shader compilation failed: " << log << std::endl;
            delete [] log;
        }
    }
    glAttachShader(program, vertShader);

    // read fragment shader from Qt resource file
    QFile fragFile(fragf);
    fragFile.open(QFile::ReadOnly | QFile::Text);
    QString fragString;
    QTextStream fragStream(&fragFile);
    fragString.append(fragStream.readAll());
    std::string fragSTLString = fragString.toStdString();

    const GLchar* fragSource = fragSTLString.c_str();

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSource, NULL);
    glCompileShader(fragShader);
    {
        GLint compiled;
        glGetShaderiv( fragShader, GL_COMPILE_STATUS, &compiled );
        if ( !compiled ) {
            GLsizei len;
            glGetShaderiv( fragShader, GL_INFO_LOG_LENGTH, &len );

            GLchar* log = new GLchar[len+1];
            glGetShaderInfoLog( fragShader, len, &len, log );
            std::cerr << "Shader compilation failed: " << log << std::endl;
            delete [] log;
        }
    }
    glAttachShader(program, fragShader);

    glLinkProgram(program);
    {
        GLint linked;
        glGetProgramiv( program, GL_LINK_STATUS, &linked );
        if ( !linked ) {
            GLsizei len;
            glGetProgramiv( program, GL_INFO_LOG_LENGTH, &len );

            GLchar* log = new GLchar[len+1];
            glGetProgramInfoLog( program, len, &len, log );
            std::cout << "Shader linker failed: " << log << std::endl;
            delete [] log;
        }
    }

    return program;
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
        case Qt::Key_W:
            // forward
            forward = true;
            break;
        case Qt::Key_A:
            // left
            left = true;
            break;
        case Qt::Key_S:
            // back
            back = true;
            break;
        case Qt::Key_D:
            // right
            right = true;
            break;
        case Qt::Key_Tab:
            // toggle fly mode
            flyMode = !flyMode;
            break;
        case Qt::Key_Shift:
            // down
            break;
        case Qt::Key_Space:
            // up or jump
            break;
        case Qt::Key_Q:
            mouseCaptured = !mouseCaptured;
            break;
        case Qt::Key_C:
            collisions = !collisions;
            break;
        case Qt::Key_B:
            headBob = !headBob;
            break;
    }
}

void GLWidget::keyReleaseEvent(QKeyEvent *event) {
    switch(event->key()) {
        case Qt::Key_W:
            // forward
            forward = false;
            break;
        case Qt::Key_A:
            // left
            left = false;
            break;
        case Qt::Key_S:
            // back
            back = false;
            break;
        case Qt::Key_D:
            // right
            right = false;
            break;
        case Qt::Key_Tab:
            // toggle fly mode
//            flyMode = false;
            break;
        case Qt::Key_Shift:
            // down
            break;
        case Qt::Key_Space:
            // up or jump
            break;
    }
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    vec2 pt(event->x(), event->y());
    lastPt = pt;
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    vec2 pt(event->x(), event->y());

    if((abs(pt.x-width/2) > .0001 || abs(pt.y-height/2) > .0001) && (abs(pt.x-lastPt.x) < 10 && abs(pt.y-lastPt.y) < 10))
    {
        if(mouseCaptured)
        {
            vec2 d = pt-lastPt;

            pitchAngle -= d.x * .01;
            yawAngle -= d.y * .01;

            pitch = rotate(mat4(1.f), (float)pitchAngle, vec3(0,1,0));
            yaw = rotate(mat4(1.f), (float)yawAngle, vec3(1,0,0));

            QCursor c = cursor();
            c.setPos(mapToGlobal(QPoint(width/2, height/2)));
            c.setShape(Qt::BlankCursor);
            setCursor(c);
        }
        else
        {
            QCursor c = cursor();
            c.setShape(Qt::ArrowCursor);
            setCursor(c);
        }
    }

    lastPt = pt;
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    float numSteps = event->delta() / 8.f / 15.f;

    walkSpeed += numSteps;
    if(walkSpeed < 10)
    {
        walkSpeed = 10;
    }
}
