#include "glwidget.h"
#include <iostream>
#include <math.h>
#include <memory>
#include "linear_function.h"
#include "sin_function.h"
#include "cir_function.h"
#include "tower_function.h"
#include "dome_function.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <QTextStream>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

using glm::inverse;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::perspective;
using glm::normalize;
using glm::length;
using glm::cross;
using glm::dot;
using glm::rotate;
using glm::translate;
using glm::value_ptr;
using glm::lookAt;
using std::cout;
using std::endl;
using std::make_shared;

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    numCubes = 0;
    cubeColor = vec3(1,1,1);
    scaler = 0;

    lightLoc = vec3(0,10,0);
    lightColor = vec3(1,1,1);
    lightBrightness = 1;
    structure.shapes.push_back(Shape(1,0,1,1,vec3(0, 0, 0),vec3(0,0,0),true,0,1,1,make_shared<Function>()));

    dist = 50;
    brickWidth = .4;
    brickHeight = .2;
    brickDepth = .2;
    spacing = .01;
    scaleX = 30;
    scaleY = 30;
    scaleZ = 30;
    wallDepth = 1;

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

    structure.updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);
}

GLWidget::~GLWidget() {
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

void GLWidget::initializeLight()
{
    glGenVertexArrays(1, &lightVao);
    glBindVertexArray(lightVao);

    // Create a buffer on the GPU for position data
    GLuint positionBuffer;
    glGenBuffers(1, &positionBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    GLuint program = loadShaders(":/light_vert.glsl", ":/light_frag.glsl");
    glUseProgram(program);
    lightProg = program;

    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    GLint positionIndex = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(positionIndex);
    glVertexAttribPointer(positionIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);

    lightProjMatrixLoc = glGetUniformLocation(program, "projection");
    lightViewMatrixLoc = glGetUniformLocation(program, "view");
    lightModelMatrixLoc = glGetUniformLocation(program, "model");
    lightLightMatrixLoc = glGetUniformLocation(program, "light");
    lightColorLoc = glGetUniformLocation(program, "color");
    lightBrightnessLoc = glGetUniformLocation(program, "brightness");

    glUniform3fv(lightColorLoc, 1, value_ptr(lightColor));
    glUniform1f(lightBrightnessLoc, lightBrightness);
    lightMatrix = translate(mat4(1.f), lightLoc) * scale(mat4(1.f), vec3(.5, .5, .5));
    glUniformMatrix4fv(lightLightMatrixLoc, 1, false, value_ptr(lightMatrix));
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
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glPointSize(4.0f);

    glEnable(GL_DEPTH_TEST);

    initializeCube();
    initializeGrid();
    initializeLight();

    glUseProgram(cubeProg);
    glUniform3fv(cubeColorLoc, 1, value_ptr(cubeColor));
}

void GLWidget::resizeGL(int w, int h) {
    width = w;
    height = h;

    float aspect = (float)w/h;

    projMatrix = perspective(45.0f, aspect, 1.0f, 300.0f);
    viewMatrix = lookAt(vec3(0,0,-1 * dist),vec3(0,0,0),vec3(0,1,0));

    glUseProgram(cubeProg);
    glUniformMatrix4fv(cubeProjMatrixLoc, 1, false, value_ptr(projMatrix));
    glUniformMatrix4fv(cubeViewMatrixLoc, 1, false, value_ptr(viewMatrix));
    glUniformMatrix4fv(cubeModelMatrixLoc, 1, false, value_ptr(modelMatrix));
    tempLight = vec3(viewMatrix * modelMatrix * vec4(lightLoc, 1));
    glUniform3fv(cubeLightPosLoc, 1, value_ptr(tempLight));

    glUseProgram(lightProg);
    glUniformMatrix4fv(lightProjMatrixLoc, 1, false, value_ptr(projMatrix));
    glUniformMatrix4fv(lightViewMatrixLoc, 1, false, value_ptr(viewMatrix));
    glUniformMatrix4fv(lightModelMatrixLoc, 1, false, value_ptr(modelMatrix));

    glUseProgram(gridProg);
    glUniformMatrix4fv(gridProjMatrixLoc, 1, false, value_ptr(projMatrix));
    glUniformMatrix4fv(gridViewMatrixLoc, 1, false, value_ptr(viewMatrix));
    glUniformMatrix4fv(gridModelMatrixLoc, 1, false, value_ptr(modelMatrix));
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderGrid();
    renderCube();
    renderLight();
}

void GLWidget::renderCube()
{
    glUseProgram(cubeProg);
    glBindVertexArray(cubeVao);
    for(unsigned int k = 0; k < structure.brickLocs.size(); k++)
    {
        mat4 loc = modelMatrix * structure.brickLocs.at(k) * structure.brickShape;
        glUniformMatrix4fv(cubeModelMatrixLoc, 1, false, value_ptr(loc));
        for(int a = 0; a < 6; a++)
        {
            glDrawArrays(GL_TRIANGLE_FAN, a * 4, 4);
        }
    }
}

void GLWidget::renderLight()
{
    glUseProgram(lightProg);
    glBindVertexArray(lightVao);
    for(int k = 0; k < 6; k++)
    {
        glDrawArrays(GL_TRIANGLE_FAN, k * 4, 4);
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

    return program;
}

void GLWidget::mousePressEvent(QMouseEvent *event) {
    vec2 pt(event->x(), event->y());
    lastVPt = normalize(pointOnVirtualTrackball(pt));
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {
    vec2 pt(event->x(), event->y());
    vec3 vPt = normalize(pointOnVirtualTrackball(pt));

    vec3 axis = cross(lastVPt, vPt);
    if(length(axis) >= .00001) {
        axis = normalize(axis);
        float angle = acos(dot(vPt,lastVPt));
        if(!isnan(angle))
        {
            mat4 r = rotate(mat4(1.0f), (float)(angle), axis);

            modelMatrix = r*modelMatrix;

            glUseProgram(cubeProg);
            glUniformMatrix4fv(cubeModelMatrixLoc, 1, false, value_ptr(modelMatrix));
            tempLight = vec3(viewMatrix * modelMatrix * vec4(lightLoc, 1));
            glUniform3fv(cubeLightPosLoc, 1, value_ptr(tempLight));

            glUseProgram(lightProg);
            glUniformMatrix4fv(lightModelMatrixLoc, 1, false, value_ptr(modelMatrix));

            glUseProgram(gridProg);
            glUniformMatrix4fv(gridModelMatrixLoc, 1, false, value_ptr(modelMatrix));
        }
    }
    lastVPt = vPt;
    update();
}

vec3 GLWidget::pointOnVirtualTrackball(const vec2 &pt) {
    float r = .5f;
    float rr = r*r;
    vec3 p;
    p.x = -1 + pt.x*(2.0f/width);
    p.y = -(float)height/width*(1-pt.y*(2.0f/height));

    float xx = p.x*p.x;
    float yy = p.y*p.y;

    if(xx+yy <= rr*.5) {
        p.z = sqrt(rr-(xx+yy));
    } else {
        p.z = rr*.5/sqrt(xx+yy);
    }

    return p;
}

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
//10- spacing
void GLWidget::wheelEvent(QWheelEvent *event)
{
    float numSteps = event->delta() / 8.f / 15.f;
    //zoom
    if(scaler == 0)
    {
        dist -= numSteps;
        if(dist < 1)
        {
            dist = 1;
        }

        glUseProgram(cubeProg);
        viewMatrix = lookAt(vec3(0,0,-1 * dist),vec3(0,0,0),vec3(0,1,0));
        glUniformMatrix4fv(cubeViewMatrixLoc, 1, false, value_ptr(viewMatrix));
        tempLight = vec3(viewMatrix * modelMatrix * vec4(lightLoc, 1));
        glUniform3fv(cubeLightPosLoc, 1, value_ptr(tempLight));
        glUseProgram(lightProg);
        glUniformMatrix4fv(lightViewMatrixLoc, 1, false, value_ptr(viewMatrix));
        glUseProgram(gridProg);
        glUniformMatrix4fv(gridViewMatrixLoc, 1, false, value_ptr(viewMatrix));
    }
    else
    {
        switch(scaler)
        {
            //brick width
            case 1:
                brickWidth += numSteps * .1;
                if(brickWidth < .1)
                {
                    brickWidth = .1;
                }
                break;
            //brick height
            case 2:
                brickHeight += numSteps * .1;
                if(brickHeight < .1)
                {
                    brickHeight = .1;
                }
                break;
            //brick depth
            case 3:
                brickDepth += numSteps * .1;
                if(brickDepth < .1)
                {
                    brickDepth = .1;
                }
                break;
            //uniform brick scale
            case 4:
                brickHeight += numSteps * .1 * (brickHeight / brickWidth);
                brickDepth += numSteps * .1 * (brickDepth / brickWidth);
                brickWidth += numSteps * .1;
                if(brickWidth < .1)
                {
                    brickWidth = .1;
                }
                if(brickHeight < .1)
                {
                    brickHeight = .1;
                }
                if(brickDepth < .1)
                {
                    brickDepth = .1;
                }
                break;
            //scale x
            case 5:
                scaleX += numSteps;
                if(scaleX < 1)
                {
                    scaleX = 1;
                }
                break;
            //scale y
            case 6:
                scaleY += numSteps;
                if(scaleY < 1)
                {
                    scaleY = 1;
                }
                break;
            //scale z
            case 7:
                scaleZ += numSteps;
                if(scaleZ < 1)
                {
                    scaleZ = 1;
                }
                break;
            //uniform scale
            case 8:
                scaleY += numSteps * (scaleY / scaleX);
                scaleZ += numSteps * (scaleZ / scaleX);
                scaleX += numSteps;
                if(scaleX < 1)
                {
                    scaleX = 1;
                }
                if(scaleY < 1)
                {
                    scaleY = 1;
                }
                if(scaleZ < 1)
                {
                    scaleZ = 1;
                }
                break;
            //scale x and z
            case 9:
                scaleZ += numSteps * (scaleZ / scaleX);
                scaleX += numSteps;
                if(scaleX < 1)
                {
                    scaleX = 1;
                }
                if(scaleZ < 1)
                {
                    scaleZ = 1;
                }
                break;
            //spacing
            case 10:
                spacing += numSteps * .01;
                if(spacing < 0)
                {
                    spacing = 0;
                }
                break;
            //brightness
            case 11:
                lightBrightness += numSteps * .01;
                if(lightBrightness < 0)
                {
                    lightBrightness = 0;
                }
                if(lightBrightness > 1)
                {
                    lightBrightness = 1;
                }
                glUseProgram(lightProg);
                glUniform1f(lightBrightnessLoc, lightBrightness);
                glUseProgram(cubeProg);
                glUniform1f(cubeLightBrightnessLoc, lightBrightness);
                break;
            //red light
            case 12:
                lightColor.r += numSteps * .01;
                if(lightColor.r < 0)
                {
                    lightColor.r = 0;
                }
                if(lightColor.r > 1)
                {
                    lightColor.r = 1;
                }
                glUseProgram(lightProg);
                glUniform3fv(lightColorLoc, 1, value_ptr(lightColor));
                glUseProgram(cubeProg);
                glUniform3fv(cubeLightColorLoc, 1, value_ptr(lightColor));
                break;
            //green light
            case 13:
                lightColor.g += numSteps * .01;
                if(lightColor.g < 0)
                {
                    lightColor.g = 0;
                }
                if(lightColor.g > 1)
                {
                    lightColor.g = 1;
                }
                glUseProgram(lightProg);
                glUniform3fv(lightColorLoc, 1, value_ptr(lightColor));
                glUseProgram(cubeProg);
                glUniform3fv(cubeLightColorLoc, 1, value_ptr(lightColor));
                break;
            //blue light
            case 14:
                lightColor.b += numSteps * .01;
                if(lightColor.b < 0)
                {
                    lightColor.b = 0;
                }
                if(lightColor.b > 1)
                {
                    lightColor.b = 1;
                }
                glUseProgram(lightProg);
                glUniform3fv(lightColorLoc, 1, value_ptr(lightColor));
                glUseProgram(cubeProg);
                glUniform3fv(cubeLightColorLoc, 1, value_ptr(lightColor));
                break;
        }

        structure.updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);
    }

    update();
}

void GLWidget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        //zoom
        case Qt::Key_Z:
            scaler = 0;
            break;
        //brickWidth
        case Qt::Key_T:
            scaler = 1;
            break;
        //brickHeight
        case Qt::Key_Y:
            scaler = 2;
            break;
        //brickDepth
        case Qt::Key_U:
            scaler = 3;
            break;
        //uniform brick scale
        case Qt::Key_B:
            scaler = 4;
            break;
        //scale x
        case Qt::Key_G:
            scaler = 5;
            break;
        //scale y
        case Qt::Key_H:
            scaler = 6;
            break;
        //scale z
        case Qt::Key_J:
            scaler = 7;
            break;
        //uniform scale
        case Qt::Key_N:
            scaler = 8;
            break;
        //scale x and z
        case Qt::Key_M:
            scaler = 9;
            break;
        //spacing
        case Qt::Key_X:
            scaler = 10;
            break;
        //brightness
        case Qt::Key_I:
            scaler = 11;
            break;
        //red light
        case Qt::Key_R:
            scaler = 12;
            break;
        //green light
        case Qt::Key_F:
            scaler = 13;
            break;
        //blue light
        case Qt::Key_V:
            scaler = 14;
            break;
        //return to defaults
        case Qt::Key_Q:
            dist = 50;
            brickWidth = .4;
            brickHeight = .2;
            brickDepth = .2;
            spacing = .01;
            scaleX = 30;
            scaleY = 30;
            scaleZ = 30;
            wallDepth = 1;

            lightLoc = vec3(0,10,0);
            lightColor = vec3(1,1,1);
            lightBrightness = 1;

            modelMatrix = mat4(1.f);
            viewMatrix = lookAt(vec3(0,0,-1 * dist),vec3(0,0,0),vec3(0,1,0));

            structure.updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);

            glUseProgram(cubeProg);
            glUniformMatrix4fv(cubeViewMatrixLoc, 1, false, value_ptr(viewMatrix));
            tempLight = vec3(viewMatrix * modelMatrix * vec4(lightLoc, 1));
            glUniform3fv(cubeLightPosLoc, 1, value_ptr(tempLight));
            glUniformMatrix4fv(cubeModelMatrixLoc, 1, false, value_ptr(modelMatrix));
            glUseProgram(lightProg);
            glUniformMatrix4fv(lightViewMatrixLoc, 1, false, value_ptr(viewMatrix));
            glUniformMatrix4fv(lightModelMatrixLoc, 1, false, value_ptr(modelMatrix));
            glUseProgram(gridProg);
            glUniformMatrix4fv(gridViewMatrixLoc, 1, false, value_ptr(viewMatrix));
            glUniformMatrix4fv(gridModelMatrixLoc, 1, false, value_ptr(modelMatrix));

            break;
        //light positive z
        case Qt::Key_W:
            lightLoc.z += .1;
            glUseProgram(lightProg);
            lightMatrix = translate(mat4(1.f), lightLoc) * scale(mat4(1.f), vec3(.5, .5, .5));
            glUniformMatrix4fv(lightLightMatrixLoc, 1, false, value_ptr(lightMatrix));
            glUseProgram(cubeProg);
            tempLight = vec3(viewMatrix * modelMatrix * vec4(lightLoc, 1));
            glUniform3fv(cubeLightPosLoc, 1, value_ptr(tempLight));
            break;
        //light negative z
        case Qt::Key_S:
            lightLoc.z -= .1;
            glUseProgram(lightProg);
            lightMatrix = translate(mat4(1.f), lightLoc) * scale(mat4(1.f), vec3(.5, .5, .5));
            glUniformMatrix4fv(lightLightMatrixLoc, 1, false, value_ptr(lightMatrix));
            glUseProgram(cubeProg);
            tempLight = vec3(viewMatrix * modelMatrix * vec4(lightLoc, 1));
            glUniform3fv(cubeLightPosLoc, 1, value_ptr(tempLight));
            break;
        //light negative x
        case Qt::Key_A:
            lightLoc.x -= .1;
            glUseProgram(lightProg);
            lightMatrix = translate(mat4(1.f), lightLoc) * scale(mat4(1.f), vec3(.5, .5, .5));
            glUniformMatrix4fv(lightLightMatrixLoc, 1, false, value_ptr(lightMatrix));
            glUseProgram(cubeProg);
            tempLight = vec3(viewMatrix * modelMatrix * vec4(lightLoc, 1));
            glUniform3fv(cubeLightPosLoc, 1, value_ptr(tempLight));
            break;
        //light positive x
        case Qt::Key_D:
            lightLoc.x += .1;
            glUseProgram(lightProg);
            lightMatrix = translate(mat4(1.f), lightLoc) * scale(mat4(1.f), vec3(.5, .5, .5));
            glUniformMatrix4fv(lightLightMatrixLoc, 1, false, value_ptr(lightMatrix));
            glUseProgram(cubeProg);
            tempLight = vec3(viewMatrix * modelMatrix * vec4(lightLoc, 1));
            glUniform3fv(cubeLightPosLoc, 1, value_ptr(tempLight));
            break;
        //light positive y
        case Qt::Key_E:
            lightLoc.y += .1;
            glUseProgram(lightProg);
            lightMatrix = translate(mat4(1.f), lightLoc) * scale(mat4(1.f), vec3(.5, .5, .5));
            glUniformMatrix4fv(lightLightMatrixLoc, 1, false, value_ptr(lightMatrix));
            glUseProgram(cubeProg);
            tempLight = vec3(viewMatrix * modelMatrix * vec4(lightLoc, 1));
            glUniform3fv(cubeLightPosLoc, 1, value_ptr(tempLight));
            break;
        //light negative y
        case Qt::Key_C:
            lightLoc.y -= .1;
            glUseProgram(lightProg);
            lightMatrix = translate(mat4(1.f), lightLoc) * scale(mat4(1.f), vec3(.5, .5, .5));
            glUniformMatrix4fv(lightLightMatrixLoc, 1, false, value_ptr(lightMatrix));
            glUseProgram(cubeProg);
            tempLight = vec3(viewMatrix * modelMatrix * vec4(lightLoc, 1));
            glUniform3fv(cubeLightPosLoc, 1, value_ptr(tempLight));
            break;
        //line
        case Qt::Key_1:
            structure.shapes.clear();
            structure.shapes.push_back(Shape(1,0,1,1,vec3(0, 0, 0),vec3(0,0,0),true,0,1,1,make_shared<Function>()));
            structure.updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);
            break;
        //square
        case Qt::Key_2:
            structure.shapes.clear();
            structure.shapes.push_back(Shape(1,1,1,4,vec3(0, 0, 0),vec3(0,0,0),true,0,1,1,make_shared<Function>()));
            structure.updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);
            break;
        //6 sides
        case Qt::Key_3:
            structure.shapes.clear();
            structure.shapes.push_back(Shape(1,1,1,6,vec3(0, 0, 0),vec3(0,0,0),true,0,1,1,make_shared<Function>()));
            structure.updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);
            break;
        //8 sided funnel
        case Qt::Key_4:
            structure.shapes.clear();
            structure.shapes.push_back(Shape(1,1,1,8,vec3(0, 0, 0),vec3(0,0,0),true,0,1,1,make_shared<LinearFunction>()));
            structure.updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);
            break;
        //4 sided pyramid
        case Qt::Key_5:
            structure.shapes.clear();
            structure.shapes.push_back(Shape(1,1,1,4,vec3(0, 0, 0),vec3(0,0,0),true,1,0,1,make_shared<LinearFunction>()));
            structure.updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);
            break;
        //100 sides sin
        case Qt::Key_6:
            structure.shapes.clear();
            structure.shapes.push_back(Shape(.5,.5,1,100,vec3(0, 0, 0),vec3(0,0,0),true,0,1,.3,make_shared<SinFunction>()));
            structure.updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);
            break;
        //castle
        case Qt::Key_7:
            structure.shapes.clear();
            structure.shapes.push_back(Shape(2,2,1.5,4,vec3(0, 0, 0),vec3(0,0,0),true,0,1,1,make_shared<Function>()));
            structure.shapes.push_back(Shape(.4,.4,2.4,10,vec3(.7, 0, .7),vec3(0,0,0),true,0,1,1,make_shared<TowerFunction>()));
            structure.shapes.push_back(Shape(.4,.4,2.4,10,vec3(-.7, 0, .7),vec3(0,0,0),true,0,1,1,make_shared<TowerFunction>()));
            structure.shapes.push_back(Shape(.4,.4,2.4,10,vec3(.7, 0, -.7),vec3(0,0,0),true,0,1,1,make_shared<TowerFunction>()));
            structure.shapes.push_back(Shape(.4,.4,2.4,10,vec3(-.7, 0, -.7),vec3(0,0,0),true,0,1,1,make_shared<TowerFunction>()));
            structure.shapes.push_back(Shape(1,1,1,4,vec3(0, 0, 0),vec3(0,0,0),true,0,1,1,make_shared<DomeFunction>()));
            structure.updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);
            break;
        //stadium
        case Qt::Key_8:
            structure.shapes.clear();
            structure.shapes.push_back(Shape(.75,1,1.5,500,vec3(0, 0, 0),vec3(0,0,0),true,0,1,1,make_shared<Function>()));
            structure.shapes.push_back(Shape(.75,1,1,500,vec3(0, .25, 0),vec3(0,0,0),true,.5,1,1,make_shared<LinearFunction>()));
            structure.shapes.push_back(Shape(.75*.75,.75,.5,500,vec3(0, 0, 0),vec3(0,0,0),true,0,1,1,make_shared<Function>()));
            structure.updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);
            break;
        //scale problem
        case Qt::Key_9:
            structure.shapes.clear();
            structure.shapes.push_back(Shape(1,1,1,4,vec3(0, 0, 0),vec3(0,0,0),true,0,1,1,make_shared<Function>()));
            structure.shapes.push_back(Shape(1,1,1,4,vec3(0, 0, 0),vec3(M_PI/4,0,0),true,0,1,1,make_shared<Function>()));
            structure.updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);
            break;
        //odd problem
        case Qt::Key_0:
            structure.shapes.clear();
            structure.shapes.push_back(Shape(1,1,1,3,vec3(0, 0, 0),vec3(0,0,0),true,0,1,1,make_shared<Function>()));
            structure.updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);
            break;
    }

    update();
}

