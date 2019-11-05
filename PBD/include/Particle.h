#ifndef PBD_PARTILE_H
#define PBD_PARTILE_H
#include <defs.h>
#include <glm/glm.hpp>

namespace PBD
{
 struct EXPORT_SYMBOL  Particle
{
    glm::vec3 mVelocity;
    glm::vec3 mPrePos;
    glm::vec3 mCurPos;
    float mInvMass;
};


 struct  EXPORT_SYMBOL Triangle
{
    unsigned p0;
    unsigned p1;
    unsigned p2;
};



}



#endif
