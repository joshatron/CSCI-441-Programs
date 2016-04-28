#ifndef PARTICLE_H
#define PARTICLE_H

#ifndef GLM_FORCE_RADIANS
    #define GLM_FORCE_RADIANS
#endif

#include <glm/glm.hpp>

using glm::vec3;

class Particle
{
    public:
        vec3 location;
        vec3 velocity;
        double ttl;

        Particle(vec3 loc, vec3 vel, double age): location(loc), velocity(vel), ttl(age){}
        void update(vec3 dv, double time)
        {
            velocity += (dv * (float)time);
            location += (velocity * (float)time);
            ttl -= time;
        }
};

#endif
