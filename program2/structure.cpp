#include "struture.h"

void Structure::updateBrickLocs(double brickWidth, double brickHeight, double brickDepth, double spacing, int wallDepth)
{
    brickLocs.clear();

    for(unsigned int k = 0; k < shapes.size(); k++)
    {
        shapes.at(k).updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, wallDepth);
        mat4 toMove = translate(shapes.at(k).centerLoc.x, shapes.at(k).centerLoc.y, 0);
        for(unsigned int a = 0; a < shapes.at(k).brickLocs.size(); a++)
        {
            brickLocs.push_back(toMove * shapes.at(k).brickLocs.at(a));
        }
    }

    brickShape = scale(brickWidth, brickHeight, brickDepth);
}
