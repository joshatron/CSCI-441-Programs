#include "door.h"
#include <iostream>

using glm::scale;
using glm::translate;
using glm::rotate;
using std::cout;
using std::endl;

Door::Door(vec3 ful, bool clock, double big, double small)
{
    fulcrumPoint = ful;
    clockwise = clock;
    opening = false;
    bigAngle = big;
    smallAngle = small;

    if(clockwise)
    {
        angle = small;
    }
    else
    {
        angle = big;
    }
}

mat4 Door::getTransform()
{
    mat4 scaleMatrix = scale(mat4(1.f), vec3(10, 20, 1));
    double zMove = -.5;
    if(clockwise)
    {
        zMove = .5;
    }
    mat4 smallTranslate = translate(mat4(1.f), vec3(5, 10, zMove));
    mat4 angleMatrix = rotate(mat4(1.f), (float)angle, vec3(0,1,0));
    mat4 translateMatrix = translate(mat4(1.f), fulcrumPoint);

    return (translateMatrix * angleMatrix * smallTranslate * scaleMatrix);
}

void Door::updateAngle(bool open, double time)
{
    double rate = 4;
    opening = open;
    
    if((open && clockwise) || (!open && !clockwise))
    {
        angle += time * rate;
        if(angle > bigAngle)
        {
            angle = bigAngle;
        }
    }
    else
    {
        angle -= time * rate;
        if(angle < smallAngle)
        {
            angle = smallAngle;
        }
    }
}
