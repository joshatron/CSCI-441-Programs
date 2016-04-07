#include "structure.h"

void Structure::updateBrickLocs(double brickWidth, double brickHeight, double brickDepth, double spacing, double scaleX, double scaleY, double scaleZ, int wallDepth)
{
    brickLocs.clear();

    //for each shape
    for(unsigned int k = 0; k < shapes.size(); k++)
    {
        //update shape's brick locations
        shapes.at(k).updateBrickLocs(brickWidth, brickHeight, brickDepth, spacing, scaleX, scaleY, scaleZ, wallDepth);
        //tranform for the whole shape
        mat4 toMove = translate(mat4(1.f), vec3((shapes.at(k).centerLoc.x * scaleX * (spacing + brickWidth)),
                                                (shapes.at(k).centerLoc.y * scaleY * (spacing + brickWidth)),
                                                (shapes.at(k).centerLoc.z * scaleZ * (spacing + brickWidth)))) *
                      rotate(mat4(1.f), (float)shapes.at(k).allRotate.x, vec3(0, 1, 0)) *
                      rotate(mat4(1.f), (float)shapes.at(k).allRotate.y, vec3(0, 0, 1)) *
                      rotate(mat4(1.f), (float)shapes.at(k).allRotate.z, vec3(1, 0, 0));
        for(unsigned int a = 0; a < shapes.at(k).brickLocs.size(); a++)
        {
            //translate each brick shape
            brickLocs.push_back(toMove * shapes.at(k).brickLocs.at(a));
        }
    }

    brickShape = glm::scale(mat4(1.f), vec3(brickWidth, brickHeight, brickDepth));
}
