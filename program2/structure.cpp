#include "structure.h"

void Structure::updateBrickLocs(double brickWidth, double brickHeight, double brickDepth, double spacing, double scale, int wallDepth)
{
    brickLocs.clear();

    for(unsigned int k = 0; k < shapes.size(); k++)
    {
        shapes.at(k).updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, scale, wallDepth);
        mat4 toMove = translate(mat4(1.f), vec3(shapes.at(k).centerLoc.x, 0, shapes.at(k).centerLoc.y));
        for(unsigned int a = 0; a < shapes.at(k).brickLocs.size(); a++)
        {
            brickLocs.push_back(toMove * shapes.at(k).brickLocs.at(a));
        }
    }

    brickShape = glm::scale(mat4(1.f), vec3(brickWidth, brickHeight, brickDepth));
}
