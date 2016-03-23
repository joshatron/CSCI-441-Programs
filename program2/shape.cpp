#include "shape.h"

Shape::Shape(double xd, double zd, double sy, double h, double a, int s, vec2 center, bool st)
{
    xDiameter = xd;
    zDiameter = zd;
    startY = sy;
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
        updateBrickLocsLine(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);
    }
    else if(sides == 4)
    {
        updateBrickLocsRect(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);
    }
    else
    {
        bool even = starting;
        float yLoc = scaleY * startY;
        for(int k = 0; k < (int)(scaleY * height); k++)
        {
            float loc = ((int)(scaleX * xDiameter) * (brickWidth + spacing)) * -.5;
            int num = (int)(scaleX * xDiameter);
            if(even)
            {
                loc += (brickWidth + spacing) * .5;
                num--;
            }
            even = !even;
            
            for(int a = 0; a < num; a++)
            {
                brickLocs.push_back(translate(mat4(1.f), vec3(loc, yLoc, 0.f)));
                loc += brickWidth + spacing;
            }
            yLoc += brickHeight + spacing;
        }
    }
}

void Shape::updateBrickLocsLine(double brickWidth, double brickHeight, double brickDepth, double spacing, double scaleX, double scaleY, double scaleZ, int wallDepth)
{
}

void Shape::updateBrickLocsRect(double brickWidth, double brickHeight, double brickDepth, double spacing, double scaleX, double scaleY, double scaleZ, int wallDepth)
{
    int width = (int)(scaleX * xDiameter);
    int depth = (int)(scaleZ * zDiameter);
    double startX = -1 * (((width - 1) / 2.) * (brickWidth + spacing));
    double startZ = -1 * (((depth - 1) / 2.) * (brickWidth + spacing));

    bool even = starting;
    float yLoc = scaleY * startY;
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
            brickLocs.push_back(translate(mat4(1.f), vec3(loc, yLoc, startZ - (brickDepth / 2))) * rotate(mat4(1.f), (float)angle, vec3(0,1,0)));
            brickLocs.push_back(translate(mat4(1.f), vec3(loc, yLoc, startZ * -1 + (brickDepth / 2))) * rotate(mat4(1.f), (float)angle, vec3(0,1,0)));
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
            brickLocs.push_back(translate(mat4(1.f), vec3(startX - (brickDepth / 2), yLoc, loc)) * rotate(mat4(1.f), (float)(90. + angle), vec3(0,1,0)));
            brickLocs.push_back(translate(mat4(1.f), vec3(startX * -1 + (brickDepth / 2), yLoc, loc)) * rotate(mat4(1.f), (float)(90. + angle), vec3(0,1,0)));
            loc += brickWidth + spacing;
        }

        yLoc += brickHeight + spacing;
    }
}
