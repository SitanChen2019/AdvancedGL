#ifndef PBD_PARTILE_H
#define PBD_PARTILE_H

#include <glm/glm.hpp>

namespace PBD
{
struct Particle
{
    glm::vec3 mVelocity;
    glm::vec3 mPrePos;
    glm::vec3 mCurPos;
    float mInvMass;
};


struct Triangle
{
    unsigned p0;
    unsigned p1;
    unsigned p2;
};

struct ParticleExt : public Particle
{

};


}



#endif
