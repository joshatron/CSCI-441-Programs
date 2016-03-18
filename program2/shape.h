#ifndef SHAPE_H
#define SHAPE_H
#include <glm/glm.hpp>
#include <vector>

#ifndef PI
    #define PI 3.14159265
#endif

using std::vector;
using glm::vec2;
using glm::mat4;


class Shape
{
    public:
        vector<mat4> brickLocs;
        vec2 centerLoc;

        void updateBrickLocs(double brickWidth, double brickHeight, double brickDepth, double spacing, int wallDepth);
    
    private:
        double xDiameter;
        double zDiameter;
        double startY;
        double height;
        double angle;//radians
        int sides;
};

#endif
