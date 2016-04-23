#include "face.h"

Face::Face(vec3 tl, vec3 tr, vec3 bl, vec3 br, vec2 tluv, vec2 truv, vec2 bluv, vec2 bruv, bool o)
{
    topLeft = tl;
    topRight = tr;
    bottomLeft = bl;
    bottomRight = br;
    topLeftUV = tluv;
    topRightUV = truv;
    bottomLeftUV = bluv;
    bottomRightUV = bruv;
    outside = o;
}
