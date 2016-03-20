#include "shape.h"

Shape::Shape(double xd, double zd, double sy, double h, double a, int s, vec2 center)
{
    xDiameter = xd;
    zDiameter = zd;
    startY = sy;
    height = h;
    angle = a;
    sides = s;
    centerLoc = center;
}

void Shape::updateBrickLocs(double brickWidth, double brickHeight, double brickDepth, double spacing, double scale, int wallDepth)
{
    brickLocs.clear();

    bool even = false;
    float yLoc = scale * startY;
    for(int k = 0; k < (int)(scale * height); k++)
    {
        float loc = scale * xDiameter * -.5 * brickWidth + ((brickWidth + spacing) * .5);
        int num = (int)(scale * xDiameter);
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
