#include "glwidget.h"
#include <iostream>

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

GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent)
{ 
    numCubes = 0;
    cubeColor = vec3(1,1,1);

    structure.lightLoc = vec3(0,10,0);
    structure.lightColor = vec3(1,1,1);
    structure.lightBrightness = 1;
    structure.shapes.push_back(Shape(.5,1,0,1,0,1,vec2(0, 2)));
    structure.shapes.push_back(Shape(1,1,0,.5,0,1,vec2(0, -2)));

    dist = 50;

    brickWidth = 1;
    brickHeight = .5;
    brickDepth = .5;
    spacing = .01;
    wallDepth = 1;

    structure.updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, 10, wallDepth);
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

void GLWidget::initializeCube() {
    // Create a new Vertex Array Object on the GPU which
    // saves the attribute layout of our vertices.
    glGenVertexArrays(1, &cubeVao);
    glBindVertexArray(cubeVao);

    // Create a buffer on the GPU for position data
    glGenBuffers(1, &positionBuffer);
    glGenBuffers(1, &normalBuffer);

    // Upload the position data to the GPU, storing
    // it in the buffer we just allocated.
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);

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

    glUniform3fv(cubeLightColorLoc, 1, value_ptr(structure.lightColor));
    glUniform1f(cubeLightBrightnessLoc, structure.lightBrightness);
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glPointSize(4.0f);

    glEnable(GL_DEPTH_TEST);
    GLuint restart = 0xFFFFFFFF;
    glPrimitiveRestartIndex(restart);
    glEnable(GL_PRIMITIVE_RESTART);

    initializeCube();
    initializeGrid();

    glUseProgram(cubeProg);
    glUniform3fv(cubeColorLoc, 1, value_ptr(cubeColor));
    createCubes(structure.brickLocs.size());
}

void GLWidget::createCubes(int num)
{
    if(num > numCubes)
    {
        for(int k = 0; k < num - numCubes; k++)
        {
            //points
            cubes.push_back(vec3(.5,.5,.5));
            cubes.push_back(vec3(.5,.5,-.5));
            cubes.push_back(vec3(-.5,.5,-.5));
            cubes.push_back(vec3(-.5,.5,.5));
            cubes.push_back(vec3(.5,-.5,.5));
            cubes.push_back(vec3(-.5,-.5,.5));
            cubes.push_back(vec3(-.5,-.5,-.5));
            cubes.push_back(vec3(.5,-.5,-.5));
            cubes.push_back(vec3(.5,.5,.5));
            cubes.push_back(vec3(-.5,.5,.5));
            cubes.push_back(vec3(-.5,-.5,.5));
            cubes.push_back(vec3(.5,-.5,.5));
            cubes.push_back(vec3(-.5,-.5,-.5));
            cubes.push_back(vec3(-.5,.5,-.5));
            cubes.push_back(vec3(.5,.5,-.5));
            cubes.push_back(vec3(.5,-.5,-.5));
            cubes.push_back(vec3(.5,-.5,.5));
            cubes.push_back(vec3(.5,-.5,-.5));
            cubes.push_back(vec3(.5,.5,-.5));
            cubes.push_back(vec3(.5,.5,.5));
            cubes.push_back(vec3(-.5,-.5,.5));
            cubes.push_back(vec3(-.5,.5,.5));
            cubes.push_back(vec3(-.5,.5,-.5));
            cubes.push_back(vec3(-.5,-.5,-.5));

            //normals
            normals.push_back(vec3(0,1,0));
            normals.push_back(vec3(0,1,0));
            normals.push_back(vec3(0,1,0));
            normals.push_back(vec3(0,1,0));
            normals.push_back(vec3(0,-1,0));
            normals.push_back(vec3(0,-1,0));
            normals.push_back(vec3(0,-1,0));
            normals.push_back(vec3(0,-1,0));
            normals.push_back(vec3(0, 0, 1));
            normals.push_back(vec3(0, 0, 1));
            normals.push_back(vec3(0, 0, 1));
            normals.push_back(vec3(0, 0, 1));
            normals.push_back(vec3(0, 0, -1));
            normals.push_back(vec3(0, 0, -1));
            normals.push_back(vec3(0, 0, -1));
            normals.push_back(vec3(0, 0, -1));
            normals.push_back(vec3(1, 0, 0));
            normals.push_back(vec3(1, 0, 0));
            normals.push_back(vec3(1, 0, 0));
            normals.push_back(vec3(1, 0, 0));
            normals.push_back(vec3(-1, 0, 0));
            normals.push_back(vec3(-1, 0, 0));
            normals.push_back(vec3(-1, 0, 0));
            normals.push_back(vec3(-1, 0, 0));
        }

        glUseProgram(cubeProg);
        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubes) * cubes.size(), cubes.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(normals) * normals.size(), normals.data(), GL_DYNAMIC_DRAW);
    }
    else if(numCubes > num)
    {
        for(int k = 0; k < num - numCubes; k++)
        {
            for(int a = 0; a < 24; a++)
            {
                cubes.erase(cubes.begin());
                normals.erase(normals.begin());
            }
        }

        glUseProgram(cubeProg);
        glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubes), cubes.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals.data(), GL_DYNAMIC_DRAW);
    }

    numCubes = num;
}

void GLWidget::resizeGL(int w, int h) {
    width = w;
    height = h;

    float aspect = (float)w/h;

    projMatrix = perspective(45.0f, aspect, 1.0f, 100.0f);
    viewMatrix = lookAt(vec3(0,0,-1 * dist),vec3(0,0,0),vec3(0,1,0));

    glUseProgram(cubeProg);
    glUniformMatrix4fv(cubeProjMatrixLoc, 1, false, value_ptr(projMatrix));
    glUniformMatrix4fv(cubeViewMatrixLoc, 1, false, value_ptr(viewMatrix));
    glUniformMatrix4fv(cubeModelMatrixLoc, 1, false, value_ptr(modelMatrix));
    vec3 tempLight = vec3(viewMatrix * modelMatrix * vec4(structure.lightLoc, 1));
    glUniform3fv(cubeLightPosLoc, 1, value_ptr(tempLight));


    glUseProgram(gridProg);
    glUniformMatrix4fv(gridProjMatrixLoc, 1, false, value_ptr(projMatrix));
    glUniformMatrix4fv(gridViewMatrixLoc, 1, false, value_ptr(viewMatrix));
    glUniformMatrix4fv(gridModelMatrixLoc, 1, false, value_ptr(modelMatrix));
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    renderGrid();
    renderCube();
}

void GLWidget::renderCube()
{
    glUseProgram(cubeProg);
    glBindVertexArray(cubeVao);
    int start = 0;
    for(int k = 0; k < numCubes; k++)
    {
        mat4 loc = modelMatrix * structure.brickLocs.at(k) * structure.brickShape;
        glUniformMatrix4fv(cubeModelMatrixLoc, 1, false, value_ptr(loc));
        for(int a = 0; a < 6; a++)
        {
            glDrawArrays(GL_TRIANGLE_FAN, start, 4);
            start += 4;
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
        mat4 r = rotate(mat4(1.0f), (float)(angle * 180. / M_PI), axis);

        modelMatrix = r*modelMatrix;

        glUseProgram(cubeProg);
        glUniformMatrix4fv(cubeModelMatrixLoc, 1, false, value_ptr(modelMatrix));
        vec3 tempLight = vec3(viewMatrix * modelMatrix * vec4(structure.lightLoc, 1));
        glUniform3fv(cubeLightPosLoc, 1, value_ptr(tempLight));

        glUseProgram(gridProg);
        glUniformMatrix4fv(gridModelMatrixLoc, 1, false, value_ptr(modelMatrix));
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

void GLWidget::wheelEvent(QWheelEvent *event)
{
    float numSteps = event->delta() / 8.f / 15.f;
    dist -= numSteps;
    if(dist < 1)
    {
        dist = 1;
    }

    glUseProgram(cubeProg);
    viewMatrix = lookAt(vec3(0,0,-1 * dist),vec3(0,0,0),vec3(0,1,0));
    glUniformMatrix4fv(cubeViewMatrixLoc, 1, false, value_ptr(viewMatrix));
    vec3 tempLight = vec3(viewMatrix * modelMatrix * vec4(structure.lightLoc, 1));
    glUniform3fv(cubeLightPosLoc, 1, value_ptr(tempLight));
    glUseProgram(gridProg);
    glUniformMatrix4fv(gridViewMatrixLoc, 1, false, value_ptr(viewMatrix));

    update();
}
