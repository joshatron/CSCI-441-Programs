#ifndef SHAPE_H
#define SHAPE_H

#ifndef GLM_FORCE_RADIANS
    #define GLM_FORCE_RADIANS
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include "function.h"

#ifndef M_PI
    #define M_PI 3.14159265
#endif

using std::vector;
using std::shared_ptr;
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
        vec3 centerLoc;
        double angle;//radians

        void updateBrickLocs(double brickWidth, double brickHeight, double brickDepth, double spacing, double scaleX, double scaleY, double scaleZ, int wallDepth);
        Shape(double xd, double zd, double h, double a, int s, vec3 center, bool st, double fb, double fe, double fs, shared_ptr<Function> func);
    
    private:
        void drawWall(double brickWidth, double brickHeight, double brickDepth, double spacing, double scaleX, double scaleY, double scaleZ, vec3 startLoc, vec3 endLoc, bool start);
        void updateBrickLocsRect(double brickWidth, double brickHeight, double brickDepth, double spacing, double scaleX, double scaleY, double scaleZ, int wallDepth);
        double xDiameter;
        double zDiameter;
        double height;
        int sides;
        bool starting;
        double functionBegin;
        double functionEnd;
        double functionScale;
        shared_ptr<Function> function;
};

#endif
