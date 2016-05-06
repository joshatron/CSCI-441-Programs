#include "fire.h"
#include <iostream>

using glm::normalize;
using std::cout;
using std::endl;

void Fire::update(double time)
{
    vec3 acceleration = vec3(0,1,0);
    int numToAdd = (int)(time * rate);
    if(numToAdd == 0)
    {
        numToAdd = 1;
    }

    for(int k = 0; k < numToAdd; k++)
    {
        vec3 vel = vec3((float)rand() * 2 / RAND_MAX - 1, 0, (float)rand() * 2 / RAND_MAX - 1);
        Particle p(center, vel, 1.5 + (float)rand() / RAND_MAX);
        particles.push_back(p);
    }

    for(int k = 0; k < particles.size(); k++)
    {
        vec3 back(0,0,0);
        if(particles.at(k).location.x > 0)
        {
            back.x = -.5;
        }
        else
        {
            back.x = .5;
        }
        if(particles.at(k).location.z > 0)
        {
            back.z = -.5;
        }
        else
        {
            back.z = .5;
        }
        particles.at(k).update(acceleration + back, time);
        if(particles.at(k).ttl < 0)
        {
            particles.erase(particles.begin() + k);
            k--;
        }
    }
}
