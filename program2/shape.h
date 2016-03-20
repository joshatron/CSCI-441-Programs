#ifndef SHAPE_H
#define SHAPE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#ifndef PI
    #define PI 3.14159265
#endif

using std::vector;
using glm::vec2;
using glm::vec3;
using glm::mat4;
using glm::translate;


class Shape
{
    public:
        vector<mat4> brickLocs;
        vec2 centerLoc;

        void updateBrickLocs(double brickWidth, double brickHeight, double brickDepth, double spacing, double scaleX, double scaleY, double scaleZ, int wallDepth);
        Shape(double xd, double zd, double sy, double h, double a, int s, vec2 center);
    
    private:
        double xDiameter;
        double zDiameter;
        double startY;
        double height;
        double angle;//radians
        int sides;
};

#endif
