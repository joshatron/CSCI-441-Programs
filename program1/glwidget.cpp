#include "glwidget.h"
#include <iostream>
#include <stdlib.h>
#include <math.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <QTextStream>

#define PI 3.14159265

using namespace std;


GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent) {
    sides = 4;
    resizeMode = 3;
    background = false;
    exactColor = true;
    mouseFollow = false;
    size = 30;
    width = baseWidth;
    height = baseHeight;
    num_shapes = 0;
    lastX = -1;
    lastY = -1;
    img = new QImage("image.jpg");
    baseWidth = img->width();
    baseHeight = img->height();
    srand(time(NULL));
}

GLWidget::~GLWidget() {
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
        case Qt::Key_1:
            sides = 1;
            break;
        case Qt::Key_2:
            sides = 2;
            break;
        case Qt::Key_3:
            sides = 3;
            break;
        case Qt::Key_4:
            sides = 4;
            break;
        case Qt::Key_5:
            sides = 5;
            break;
        case Qt::Key_6:
            sides = 6;
            break;
        case Qt::Key_7:
            sides = 7;
            break;
        case Qt::Key_8:
            sides = 8;
            break;
        case Qt::Key_9:
            sides = 9;
            break;
        case Qt::Key_Z:
            resizeMode = 1;
            resizeGL(width, height);
            break;
        case Qt::Key_X:
            resizeMode = 2;
            resizeGL(width, height);
            break;
        case Qt::Key_C:
            resizeMode = 3;
            resizeGL(width, height);
            break;
        case Qt::Key_V:
            resizeMode = 0;
            resizeGL(width, height);
            break;
        case Qt::Key_U:
            background = !background;
            break;
        case Qt::Key_I:
            exactColor = !exactColor;
            break;
        case Qt::Key_O:
            cout << "Clearing screen" << endl;
            clearScreen();
            break;
        case Qt::Key_P:
            mouseFollow = !mouseFollow;
            if(!mouseFollow)
            {
                lastX = -1;
                lastY = -1;
            }
            break;
        case Qt::Key_A:
            cout << "Filling uniformly" << endl;
            fillUniformly();
            break;
        case Qt::Key_S:
            cout << "Filling randomly" << endl;
            fillRandomly();
            break;
    }
    update();

    cout << "Sides: " << sides << endl;
    cout << "Resize: " << resizeMode << endl;
    cout << "Background: " << background << endl;
    cout << "Exact color: " << exactColor << endl;
    cout << "Mouse follow: " << mouseFollow << endl;
    cout << "Size: " << size << endl << endl;
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    lastX = -1;
    lastY = -1;
    glm::vec2 loc = rawLocToBaseLoc(event->x(), event->y());
    addShape(loc.x, loc.y);
    lastX = loc.x;
    lastY = loc.y;

    //push all buffers
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * shapes.size(), shapes.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * colors.size(), colors.data(), GL_DYNAMIC_DRAW);
    update();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(!(event->buttons() & Qt::LeftButton))
    {
        return;
    }

    glm::vec2 loc = rawLocToBaseLoc(event->x(), event->y());

    if(((loc.x - lastX) * (loc.x - lastX)) + ((loc.y - lastY) * (loc.y - lastY)) < size * size)
    {
        return;
    }

    addShape(loc.x, loc.y);
    lastX = loc.x;
    lastY = loc.y;

    //push all buffers
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * shapes.size(), shapes.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * shapes.size(), colors.data(), GL_DYNAMIC_DRAW);
    update();
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
    float numSteps = event->delta() / 8.f / 15.f;
    size += numSteps / 2;
    if(size < 1)
    {
        size = 1;
    }
}

void GLWidget::clearScreen()
{
    shapes.clear();
    colors.clear();
    allShapes.clear();
    num_shapes = 0;

    lastX = -1;
    lastY = -1;

    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
    update();
}

void GLWidget::fillUniformly()
{
    clearScreen();
    bool tempFollow = mouseFollow;
    mouseFollow = false;

    float offsetX = baseWidth / size;
    offsetX -= (int)offsetX;
    offsetX *= size;
    offsetX /= 2;

    float offsetY = baseHeight / size;
    offsetY -= (int)offsetY;
    offsetY *= size;
    offsetY /= 2;

    for(int k = (int)offsetX; k < baseWidth; k += size)
    {
        for(int a = (int)offsetY; a < baseHeight; a += size)
        {
            addShape(k, a);
        }
    }

    mouseFollow = tempFollow;

    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * shapes.size(), shapes.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * colors.size(), colors.data(), GL_DYNAMIC_DRAW);
    update();
}

void GLWidget::fillRandomly()
{
    clearScreen();
    bool tempFollow = mouseFollow;
    mouseFollow = false;

    float totalArea = baseWidth * baseHeight;
    float singleArea = PI * size * size / 6;
    if(sides == 3)
    {
        singleArea /= 2;
    }

    int totalShapes = totalArea / singleArea * 2;
    for(int k = 0; k < totalShapes; k++)
    {
        addShape(abs(rand() % baseWidth), abs(rand() % baseHeight));
    }

    mouseFollow = tempFollow;

    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * shapes.size(), shapes.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * colors.size(), colors.data(), GL_DYNAMIC_DRAW);
    update();
}

glm::vec2 GLWidget::rawLocToBaseLoc(int x, int y)
{
    glm::vec2 newShape;
    //same
    if(resizeMode == 0)
    {
        newShape.x = x;
        newShape.y = y;
    }
    //width
    else if(resizeMode == 1)
    {
        newShape.x = x * ((float)baseWidth / width);
        if(((float)width / (float)height) > ((float)baseWidth / (float)baseHeight))
        {
            float newH = baseHeight * (1.f * width / baseWidth);
            newShape.y = (baseHeight / newH) * y;
        }
        else
        {
            float newH = baseHeight * (1.f * width / baseWidth);
            float diff = ((float)height - newH) * (baseHeight / newH);
            newShape.y = (baseHeight / newH) * y - (diff / 2);
        }
    }
    //height
    else if(resizeMode == 2)
    {
        newShape.y = y * ((float)baseHeight / height);
        if(((float)width / (float)height) < ((float)baseWidth / (float)baseHeight))
        {
            float newW = baseWidth * (1.f * height / baseHeight);
            newShape.x = (baseWidth / newW) * x;
        }
        else
        {
            float newW = baseWidth * (1.f * height / baseHeight);
            float diff = ((float)width - newW) * (baseWidth / newW);
            newShape.x = (baseWidth / newW) * x - (diff / 2);
        }
    }
    //both
    else
    {
        if(((float)width / (float)height) < ((float)baseWidth / (float)baseHeight))
        {
            float newH = baseHeight * (1.f * width / baseWidth);
            float diff = ((float)height - newH) * (baseHeight / newH);
            newShape.y = (baseHeight / newH) * y - (diff / 2);
            newShape.x = x * ((float)baseWidth / width);
        }
        else
        {
            float newW = baseWidth * (1.f * height / baseHeight);
            float diff = ((float)width - newW) * (baseWidth / newW);
            newShape.x = (baseWidth / newW) * x - (diff / 2);
            newShape.y = y * ((float)baseHeight / height);
        }
    }

    return newShape;

}

void GLWidget::addShape(int x, int y)
{
    Shape newShape;
    newShape.x = x;
    newShape.y = y;
    
    //check to see if it is out of bounds
    if(newShape.x < 0 || newShape.x > baseWidth || newShape.y < 0 || newShape.y > baseHeight)
    {
        cout << "out of bounds!" << endl;
        return;
    }

    newShape.radius = size;
    newShape.sides = sides;
    QColor clr(img->pixel(x, y));
    newShape.r = clr.red() / 255.;
    newShape.g = clr.green() / 255.;
    newShape.b = clr.blue() / 255.;

    addShapePoints(newShape);
}

void GLWidget::addShapePoints(Shape newShape)
{
    //lines
    if(newShape.sides == 2) 
    {
        vec3 nextColor;
        nextColor.r = newShape.r;
        nextColor.g = newShape.g;
        nextColor.b = newShape.b;

        vec2 next;
        for(int k = 0; k < 15; k++)
        {
            int angle = rand() % 180;
            next.x = newShape.x + cos(angle * PI / 180) * size / 2;
            next.y = newShape.y + sin(angle * PI / 180) * size / 2;
            shapes.push_back(next);
            next.x = newShape.x - cos(angle * PI / 180) * size / 2;
            next.y = newShape.y - sin(angle * PI / 180) * size / 2;
            shapes.push_back(next);
            if(exactColor)
            {
                colors.push_back(nextColor);
                colors.push_back(nextColor);
            }
            else
            {
                int size = shapes.size();
                QColor clr(img->pixel(shapes[size - 2].x, shapes[size - 2].y));
                nextColor.r = clr.red() / 255.;
                nextColor.g = clr.green() / 255.;
                nextColor.b = clr.blue() / 255.;
                colors.push_back(nextColor);
                QColor clr2(img->pixel(shapes[size - 1].x, shapes[size - 1].y));
                nextColor.r = clr2.red() / 255.;
                nextColor.g = clr2.green() / 255.;
                nextColor.b = clr2.blue() / 255.;
                colors.push_back(nextColor);
            }
        }
    }
    else
    {
        if(newShape.sides == 1)
        {
            newShape.sides = 10;
        }
        float anglePer = 360. / newShape.sides;
        float angle = 0;
        if(mouseFollow && lastX >= 0)
        {
            angle = atan2(lastX - newShape.x, lastY - newShape.y) * 180 / PI;
            angle = 360 - angle;
        }
        if(newShape.sides % 2 == 0)
        {
            angle += anglePer / 2;
        }
        angle = angle * PI / 180;

        glm::mat2 rotate = glm::mat2(cos(angle), sin(angle), -1 * sin(angle), cos(angle));

        vec3 nextColor;
        nextColor.r = newShape.r;
        nextColor.g = newShape.g;
        nextColor.b = newShape.b;

        vec2 next;
        for(int k = 0; k <= 361; k += anglePer)
        {
            next.x = sin(k * PI / 180) * size / 2;
            next.y = -1 * cos(k * PI / 180) * size / 2;
            next = rotate * next;
            next.x += newShape.x;
            next.y += newShape.y;
            shapes.push_back(next);

            if(exactColor)
            {
                colors.push_back(nextColor);
            }
            else
            {
                QColor clr(img->pixel(next.x, next.y));
                nextColor.r = clr.red() / 255.;
                nextColor.g = clr.green() / 255.;
                nextColor.b = clr.blue() / 255.;
                colors.push_back(nextColor);
            }
        }
    }

    num_shapes++;

    allShapes.push_back(newShape);
}

void GLWidget::initializeGL() {
    initializeOpenGLFunctions();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glPointSize(4.0f);

    // Create a new Vertex Array Object on the GPU which
    // saves the attribute layout of our vertices.
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a buffer on the GPU for position data
    glGenBuffers(1, &positionBuffer);

    // Upload the position data to the GPU, storing
    // it in the buffer we just allocated.
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);


    // Load our vertex and fragment shaders into a program object
    // on the GPU
    program = loadShaders(":/vert.glsl", ":/frag.glsl");

    // Bind the attribute "position" (defined in our vertex shader)
    // to the currently bound buffer object, which contains our
    // position data for a single triangle. This information 
    // is stored in our vertex array object.
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    GLint positionIndex = glGetAttribLocation(program, "position");
    glEnableVertexAttribArray(positionIndex);
    glVertexAttribPointer(positionIndex, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    GLint colorIndex = glGetAttribLocation(program, "color_in");
    glEnableVertexAttribArray(colorIndex);
    glVertexAttribPointer(colorIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glUseProgram(program);
    projectionLoc = glGetUniformLocation(program, "projection");
}

void GLWidget::resizeGL(int w, int h) {
    glViewport(0,0,w,h);
    width = w;
    height = h;

    // When our OpenGL context is resized, we need to change
    // our projection matrix to match. Create an orthographic 
    // projection matrix that converts window coordinates to normalized 
    // device coordinates.  This is similar to our previous lab, 
    // except the conversion will happen in our vertex shader. 
    // This way we won't need to do any conversions on our mouse 
    // event coordinates and when we resize the window the geometry 
    // won't be scaled unevenly.

    //same
    if(resizeMode == 0)
    {
        projection = glm::ortho(0.f, (float)w, (float)h, 0.f);
    }
    //width
    else if(resizeMode == 1)
    {
        if(((float)w / (float)h) > ((float)baseWidth / (float)baseHeight))
        {
            projection = glm::ortho(0.f, (float)baseWidth, (float)h * (baseWidth / (float)w), 0.f);
        }
        else
        {
            float newH = baseHeight * (1.f * w / baseWidth);
            float diff = ((float)h - newH) * (baseHeight / newH);
            projection = glm::ortho(0.f, (float)baseWidth, diff / 2 + baseHeight, diff / 2 * -1);
        }
    }
    //height
    else if(resizeMode == 2)
    {
        if(((float)w / (float)h) < ((float)baseWidth / (float)baseHeight))
        {
            projection = glm::ortho(0.f, (float)w * (baseHeight / (float)h), (float)baseHeight, 0.f);
        }
        else
        {
            float newW = baseWidth * (1.f * h / baseHeight);
            float diff = ((float)w - newW) * (baseWidth / newW);
            projection = glm::ortho(diff / 2 * -1, diff / 2 + baseWidth, (float)baseHeight, 0.f);
        }
    }
    //both
    else
    {
        if(((float)w / (float)h) < ((float)baseWidth / (float)baseHeight))
        {
            float newH = baseHeight * (1.f * w / baseWidth);
            float diff = ((float)h - newH) * (baseHeight / newH);
            projection = glm::ortho(0.f, (float)baseWidth, diff / 2 + baseHeight, diff / 2 * -1);
        }
        else
        {
            float newW = baseWidth * (1.f * h / baseHeight);
            float diff = ((float)w - newW) * (baseWidth / newW);
            projection = glm::ortho(diff / 2 * -1, diff / 2 + baseWidth, (float)baseHeight, 0.f);
        }
    }

    glUseProgram(program);
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
}

void GLWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);

    // draw primitives based on the current draw mode
    //glDrawArrays(drawMode, 0, num_shapes);

    int start = 0;
    for(int k = 0; k < num_shapes; k++)
    {
        if(allShapes[k].sides == 2)
        {
            glDrawArrays(GL_LINES, start, 30);
            start += 30;
        }
        else
        {
            glDrawArrays(GL_TRIANGLE_FAN, start, allShapes[k].sides + 1);
            start += allShapes[k].sides + 1;
        }
    }
}

// Copied from LoadShaders.cpp in the the oglpg-8th-edition.zip
// file provided by the OpenGL Programming Guide, 8th edition.
const GLchar* GLWidget::readShader(const char* filename) {
#ifdef WIN32
        FILE* infile;
        fopen_s( &infile, filename, "rb" );
#else
    FILE* infile = fopen( filename, "rb" );
#endif // WIN32

    if ( !infile ) {
#ifdef _DEBUG
        std::cerr << "Unable to open file '" << filename << "'" << std::endl;
#endif /* DEBUG */
        return NULL;
    }

    fseek( infile, 0, SEEK_END );
    int len = ftell( infile );
    fseek( infile, 0, SEEK_SET );

    GLchar* source = new GLchar[len+1];

    fread( source, 1, len, infile );
    fclose( infile );

    source[len] = 0;

    return const_cast<const GLchar*>(source);
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

