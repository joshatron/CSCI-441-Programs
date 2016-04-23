#ifndef FACE_H
#define FACE_H

#ifndef GLM_FORCE_RADIANS
    #define GLM_FORCE_RADIANS
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef M_PI
    #define M_PI 3.14159265
#endif

using glm::vec2;
using glm::vec3;
using glm::mat4;
using glm::translate;
using glm::rotate;


class Face
{
    public:
        vec3 topLeft, topRight, bottomLeft, bottomRight;
        vec2 topLeftUV, topRightUV, bottomLeftUV, bottomRightUV;
        bool outside;

        Face(vec3 tl, vec3 tr, vec3 bl, vec3 br, vec2 tluv, vec2 truv, vec2 bluv, vec2 bruv, bool o);
};

#endif
