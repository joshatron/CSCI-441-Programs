#include "structure.h"

void Structure::updateBrickLocs(double brickWidth, double brickHeight, double brickDepth, double spacing, double scaleX, double scaleY, double scaleZ, int wallDepth)
{
    brickLocs.clear();

    for(unsigned int k = 0; k < shapes.size(); k++)
    {
        shapes.at(k).updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);
        mat4 toMove = translate(mat4(1.f), vec3(shapes.at(k).centerLoc.x * scaleX + (shapes.at(k).centerLoc.x * scaleX * spacing),
                                                shapes.at(k).centerLoc.y * scaleY + (shapes.at(k).centerLoc.y * scaleY * spacing),
                                                shapes.at(k).centerLoc.z * scaleZ + (shapes.at(k).centerLoc.z * scaleZ * spacing))) *
                      rotate(mat4(1.f), (float)shapes.at(k).angle, vec3(0, 1, 0));
        for(unsigned int a = 0; a < shapes.at(k).brickLocs.size(); a++)
        {
            brickLocs.push_back(toMove * shapes.at(k).brickLocs.at(a));
        }
    }

    brickShape = glm::scale(mat4(1.f), vec3(brickWidth, brickHeight, brickDepth));
}
