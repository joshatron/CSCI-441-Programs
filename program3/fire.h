#ifndef FIRE_H
#define FIRE_H

#ifndef GLM_FORCE_RADIANS
    #define GLM_FORCE_RADIANS
#endif

#include <glm/glm.hpp>
#include <vector>
#include "particle.h"

using std::vector;
using glm::vec3;

class Fire
{
    public:
        vector<Particle> particles;
        Fire(double r, vec3 cen): rate(r), center(cen){}
        void update(double time);
    private:
        double rate;
        vec3 center;
};

#endif
