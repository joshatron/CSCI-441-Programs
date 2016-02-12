#include "glwidget.h"
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <QTextStream>

using namespace std;


GLWidget::GLWidget(QWidget *parent) : QOpenGLWidget(parent), outline(false), drawMode(0) {
    shape = 0;
    resizeMode = 3;
    background = false;
    exactColor = true;
    mouseFollow = false;
    size = 10;
    baseWidth = 640;
    baseHeight = 480;
    width = baseWidth;
    height = baseHeight;
    num_squares = 0;
    num_circles = 0;
    num_triangles = 0;
    num_lines = 0;
    lastX = 0;
    lastY = 0;
    drawMode = GL_POINTS;
}

GLWidget::~GLWidget() {
}

void GLWidget::keyPressEvent(QKeyEvent *event) {
    switch(event->key()) {
        case Qt::Key_Q:
            shape = 0;
            break;
        case Qt::Key_W:
            shape = 1;
            break;
        case Qt::Key_E:
            shape = 2;
            break;
        case Qt::Key_R:
            shape = 3;
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
            break;
        case Qt::Key_A:
            //uniform fill
            cout << "Filling uniformly" << endl;
            break;
        case Qt::Key_S:
            //Random fill
            cout << "Filling randomly" << endl;
            break;
    }
    update();

    cout << "Shape: " << shape << endl;
    cout << "Resize: " << resizeMode << endl;
    cout << "Background: " << background << endl;
    cout << "Exact color: " << exactColor << endl;
    cout << "Mouse follow: " << mouseFollow << endl;
    cout << "Size: " << size << endl << endl;
}

void GLWidget::clearScreen()
{
    squareCenters.clear();
    num_squares = 0;
    circleCenters.clear();
    num_circles = 0;
    triangleCenters.clear();
    num_triangles = 0;
    lines.clear();
    num_lines = 0;
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_DYNAMIC_DRAW);
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    addPoint(event->x(), event->y());
    lastX = event->x();
    lastY = event->y();

    updatePoints();
    updateColors();

    //push all buffers
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * points.size(), points.data(), GL_DYNAMIC_DRAW);

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

    if(((event->x() - lastX) * (event->x() - lastX)) + ((event->y() - lastY) * (event->y() - lastY)) < size * size)
    {
        return;
    }

    addPoint(event->x(), event->y());
    lastX = event->x();
    lastY = event->y();

    updatePoints();
    updateColors();

    //push all buffers
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * points.size(), points.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * colors.size(), colors.data(), GL_DYNAMIC_DRAW);
    update();
}

void GLWidget::updatePoints()
{
    points.clear();
    
    for(int k = 0; k < num_squares; k++)
    {
        vec2 nextPoint;
        nextPoint.x = squareCenters[k].x;
        nextPoint.y = squareCenters[k].y;
        points.push_back(nextPoint);
    }

    for(int k = 0; k < num_circles; k++)
    {
        vec2 nextPoint;
        nextPoint.x = circleCenters[k].x;
        nextPoint.y = circleCenters[k].y;
        points.push_back(nextPoint);
    }

    for(int k = 0; k < num_triangles; k++)
    {
        vec2 nextPoint;
        nextPoint.x = triangleCenters[k].x;
        nextPoint.y = triangleCenters[k].y;
        points.push_back(nextPoint);
    }

    for(int k = 0; k < num_lines; k++)
    {
        vec2 nextPoint;
        nextPoint.x = lines[k].x;
        nextPoint.y = lines[k].y;
        points.push_back(nextPoint);
    }
}

void GLWidget::updateColors()
{
    colors.clear();
    
    for(int k = 0; k < num_squares; k++)
    {
        vec3 nextColor;
        nextColor.r = squareCenters[k].r;
        nextColor.g = squareCenters[k].g;
        nextColor.b = squareCenters[k].b;
        colors.push_back(nextColor);
    }

    for(int k = 0; k < num_circles; k++)
    {
        vec3 nextColor;
        nextColor.r = circleCenters[k].r;
        nextColor.g = circleCenters[k].g;
        nextColor.b = circleCenters[k].b;
        colors.push_back(nextColor);
    }

    for(int k = 0; k < num_triangles; k++)
    {
        vec3 nextColor;
        nextColor.r = triangleCenters[k].r;
        nextColor.g = triangleCenters[k].g;
        nextColor.b = triangleCenters[k].b;
        colors.push_back(nextColor);
    }

    for(int k = 0; k < num_lines; k++)
    {
        vec3 nextColor;
        nextColor.r = lines[k].r;
        nextColor.g = lines[k].g;
        nextColor.b = lines[k].b;
        colors.push_back(nextColor);
    }
}

void GLWidget::addPoint(int x, int y)
{
    Shape newPoint;
    //same
    if(resizeMode == 0)
    {
        newPoint.x = x;
        newPoint.y = y;
    }
    //width
    else if(resizeMode == 1)
    {
        newPoint.x = x * ((float)baseWidth / width);
        if(((float)width / (float)height) > ((float)baseWidth / (float)baseHeight))
        {
            float newH = baseHeight * (1.f * width / baseWidth);
            newPoint.y = (baseHeight / newH) * y;
        }
        else
        {
            float newH = baseHeight * (1.f * width / baseWidth);
            float diff = ((float)height - newH) * (baseHeight / newH);
            newPoint.y = (baseHeight / newH) * y - (diff / 2);
        }
    }
    //height
    else if(resizeMode == 2)
    {
        newPoint.y = y * ((float)baseHeight / height);
        if(((float)width / (float)height) < ((float)baseWidth / (float)baseHeight))
        {
            float newW = baseWidth * (1.f * height / baseHeight);
            newPoint.x = (baseWidth / newW) * x;
        }
        else
        {
            float newW = baseWidth * (1.f * height / baseHeight);
            float diff = ((float)width - newW) * (baseWidth / newW);
            newPoint.x = (baseWidth / newW) * x - (diff / 2);
        }
    }
    //both
    else
    {
        if(((float)width / (float)height) < ((float)baseWidth / (float)baseHeight))
        {
            float newH = baseHeight * (1.f * width / baseWidth);
            float diff = ((float)height - newH) * (baseHeight / newH);
            newPoint.y = (baseHeight / newH) * y - (diff / 2);
            newPoint.x = x * ((float)baseWidth / width);
        }
        else
        {
            float newW = baseWidth * (1.f * height / baseHeight);
            float diff = ((float)width - newW) * (baseWidth / newW);
            newPoint.x = (baseWidth / newW) * x - (diff / 2);
            newPoint.y = y * ((float)baseHeight / height);
        }
    }

    //check to see if it is out of bounds
    if(newPoint.x < 0 || newPoint.x > baseWidth || newPoint.y < 0 || newPoint.y > baseHeight)
    {
        cout << "out of bounds!" << endl;
        return;
    }

    newPoint.rotation = 0;
    newPoint.radius = size;
    newPoint.r = 1;
    newPoint.g = 1;
    newPoint.b = 1;

    //push to correct section
    if(shape == 0)
    {
        newPoint.r = 1;
        newPoint.g = 0;
        newPoint.b = 0;
        squareCenters.push_back(newPoint);
        num_squares++;
    }
    else if(shape == 1)
    {
        newPoint.r = 0;
        newPoint.g = 1;
        newPoint.b = 0;
        circleCenters.push_back(newPoint);
        num_circles++;
    }
    else if(shape == 2)
    {
        newPoint.r = 0;
        newPoint.g = 0;
        newPoint.b = 1;
        triangleCenters.push_back(newPoint);
        num_triangles++;
    }
    else
    {
        //generate lines and push
    }
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
    glDrawArrays(drawMode, 0, points.size());
    
    // draw points so we can always see them
    // glPointSize adjusts the size of point
    // primitives
    glDrawArrays(GL_POINTS, 0, points.size());
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

