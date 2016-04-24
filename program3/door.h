#ifndef DOOR_H
#define DOOR_H

#ifndef GLM_FORCE_RADIANS
    #define GLM_FORCE_RADIANS
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef M_PI
    #define M_PI 3.14159265
#endif

using glm::vec3;
using glm::mat4;

class Door
{
    public:
        vec3 fulcrumPoint;
        bool clockwise;
        bool opening;
        double angle;
        double bigAngle;
        double smallAngle;

        Door(vec3 ful, bool clock, double big, double small);
        mat4 getTransform();
        void updateAngle(bool open, double time);
};

#endif
