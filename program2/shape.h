#ifndef SHAPE_H
#define SHAPE_H

#ifndef GLM_FORCE_RADIANS
    #define GLM_FORCE_RADIANS
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#ifndef M_PI
    #define M_PI 3.14159265
#endif

using std::vector;
using glm::vec2;
using glm::vec3;
using glm::mat4;
using glm::translate;
using glm::rotate;
using glm::distance;


class Shape
{
    public:
        vector<mat4> brickLocs;
        vec2 centerLoc;
        double angle;//radians

        void updateBrickLocs(double brickWidth, double brickHeight, double brickDepth, double spacing, double scaleX, double scaleY, double scaleZ, int wallDepth);
        Shape(double xd, double zd, double sy, double h, double a, int s, vec2 center, bool st);
    
    private:
        void drawWall(double brickWidth, double brickHeight, double brickDepth, double spacing, double scale, double scaleY, vec3 startLoc, vec3 endLoc, bool start);
        void updateBrickLocsRect(double brickWidth, double brickHeight, double brickDepth, double spacing, double scaleX, double scaleY, double scaleZ, int wallDepth);
        double xDiameter;
        double zDiameter;
        double startY;
        double height;
        int sides;
        bool starting;
};

#endif
