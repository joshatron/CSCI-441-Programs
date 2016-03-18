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

void Shape::updateBrickLocs(double brickWidth, double brickHeight, double brickDepth, double spacing, int wallDepth)
{
    brickLocs.clear();

    brickLocs.push_back(translate(0, 0, 0));
    brickLocs.push_back(translate(5, 0, 0));
    brickLocs.push_back(translate(-5, 0, 0));
    brickLocs.push_back(translate(0, 5, 0));
    brickLocs.push_back(translate(0, -5, 0));
}
