#include "shape.h"
#include <iostream>

using std::cout;
using std::endl;

Shape::Shape(double xd, double zd, double h, double a, int s, vec3 center, bool st)
{
    xDiameter = xd;
    zDiameter = zd;
    height = h;
    angle = a;
    sides = s;
    centerLoc = center;
    starting = st;
}

void Shape::updateBrickLocs(double brickWidth, double brickHeight, double brickDepth, double spacing, double scaleX, double scaleY, double scaleZ, int wallDepth)
{
    brickLocs.clear();

    if(sides == 1)
    {
        drawWall(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, vec3(-1 * (xDiameter / 2), 0, -1 * (zDiameter / 2)), vec3((xDiameter / 2), height, (zDiameter / 2)), starting);
    }
    else if(sides == 2)
    {
        drawWall(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, vec3(-1 * (xDiameter / 2), 0, -1 * (zDiameter / 2)), vec3((xDiameter / 2), height, (zDiameter / 2)), starting);
        drawWall(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, vec3(-1 * (xDiameter / 2), 0, (zDiameter / 2)), vec3((xDiameter / 2), height, -1 * (zDiameter / 2)), starting);
    }
    else if(sides == 4)
    {
        updateBrickLocsRect(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);
    }
    else
    {
        vector<vec2> points;
        double ang = M_PI / (sides / 2);
        for(double k = ang / 2; k < 2 * M_PI; k += ang)
        {
            points.push_back(vec2(sin(k) * xDiameter, cos(k) * zDiameter));
        }

        bool even = starting;
        for(int k = 0; k < sides; k++)
        {
            vec3 start((float)points.at(k).x, 0.f, (float)points.at(k).y);
            vec3 end((float)points.at((k + 1) % points.size()).x, (float)height, (float)points.at((k + 1) % points.size()).y);
            drawWall(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, start, end, even);
            even = !even;
        }
    }
}

void Shape::drawWall(double brickWidth, double brickHeight, double brickDepth, double spacing, double scaleX, double scaleY, double scaleZ, vec3 startLoc, vec3 endLoc, bool start)
{
    double length = sqrt(pow((startLoc.x - endLoc.x) * (xDiameter * scaleX) * (brickWidth + spacing), 2) + pow((startLoc.z - endLoc.z) * (zDiameter * scaleZ) * (brickWidth + spacing), 2)) + brickWidth;
    double angle = -1 * atan2((startLoc.z - endLoc.z) * (scaleZ / scaleX), (startLoc.x - endLoc.x));
    vec3 center = vec3((startLoc.x + endLoc.x) * (xDiameter * scaleX) * (brickWidth + spacing) / 2, 0, (startLoc.z + endLoc.z) * (zDiameter * scaleZ) * (brickWidth + spacing) / 2);
    double num = (int)(length / (brickWidth + spacing));
    double startX = -1 * (((num - 1) / 2.) * (brickWidth + spacing));
    mat4 transform = translate(mat4(1.f), vec3(center.x, 0, center.z)) * rotate(mat4(1.f), (float)angle, vec3(0,1,0));

    double y = startLoc.y;
    bool even = start;
    while(y < endLoc.y * scaleY * (brickHeight + spacing) - brickHeight)
    {
        double loc = startX;
        int rowNum = num;
        if(even)
        {
            loc += (brickWidth + spacing) / 2.;
            rowNum--;
        }
        for(int k = 0; k < rowNum; k++)
        {
            brickLocs.push_back(transform * translate(mat4(1.f), vec3(loc, y, 0)));
            loc += brickWidth + spacing;
        }
        
        y += brickHeight + spacing;
        even = !even;
    }
}

void Shape::updateBrickLocsRect(double brickWidth, double brickHeight, double brickDepth, double spacing, double scaleX, double scaleY, double scaleZ, int wallDepth)
{
    int width = (int)(scaleX * xDiameter);
    int depth = (int)(scaleZ * zDiameter);
    double startX = -1 * (((width - 1) / 2.) * (brickWidth + spacing));
    double startZ = -1 * (((depth - 1) / 2.) * (brickWidth + spacing));

    bool even = starting;
    float yLoc = 0;
    for(int k = 0; k < (int)(scaleY * height); k++)
    {
        float loc = startX;
        int widthNum = width;
        if(even)
        {
            loc += (brickWidth + spacing) * .5;
            widthNum--;
        }

        for(int a = 0; a < widthNum; a++)
        {
            brickLocs.push_back(translate(mat4(1.f), vec3(loc, yLoc, startZ - (brickDepth / 2))) * rotate(mat4(1.f), 0.f, vec3(0,1,0)));
            brickLocs.push_back(translate(mat4(1.f), vec3(loc, yLoc, startZ * -1 + (brickDepth / 2))) * rotate(mat4(1.f), 0.f, vec3(0,1,0)));
            loc += brickWidth + spacing;
        }

        loc = startZ;
        int depthNum = depth;
        if(!even)
        {
            loc += (brickWidth + spacing) * .5;
            depthNum--;
        }
        even = !even;

        for(int a = 0; a < depthNum; a++)
        {
            brickLocs.push_back(translate(mat4(1.f), vec3(startX - (brickDepth / 2), yLoc, loc)) * rotate(mat4(1.f), (float)(M_PI / 2), vec3(0,1,0)));
            brickLocs.push_back(translate(mat4(1.f), vec3(startX * -1 + (brickDepth / 2), yLoc, loc)) * rotate(mat4(1.f), (float)(M_PI / 2), vec3(0,1,0)));
            loc += brickWidth + spacing;
        }

        yLoc += brickHeight + spacing;
    }
}
