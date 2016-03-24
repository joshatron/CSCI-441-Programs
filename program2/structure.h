#ifndef STRUCTURE_H
#define STRUCTURE_H

#ifndef GLM_FORCE_RADIANS
    #define GLM_FORCE_RADIANS
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "shape.h"

#ifndef PI
    #define PI 3.14159265
#endif

using std::vector;
using glm::vec3;
using glm::mat4;
using glm::translate;
using glm::rotate;
using glm::scale;

class Structure
{
    public:
        vector<Shape> shapes;
        vector<mat4> brickLocs;
        mat4 brickShape;

        void updateBrickLocs(double brickWidth, double brickHeight, double brickDepth, double spacing, double scaleX, double scaleY, double scaleZ, int wallDepth);
};

#endif
