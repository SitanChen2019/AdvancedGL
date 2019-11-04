#include "Simulator.h"

namespace PBD
{
    
    Simualtor::Simualtor()
    {

    }

    void Simualtor::update()
    {
        float t = 0.1f;
        
        //update velocity by extental force
        for(unsigned i= 0, n = (unsigned)mParticles.size(); i < n; ++i )
        {
            Particle& p = mParticles[i];
            p.mVelocity += t * p.mInvMass* getExtForce(p);
        }
        
        //predicate the current position with velocity
        
        //generate collison constraint
        
        //project constraint
        
        //update position and velocity
        
        //velocity update
    }

    
    glm::vec3 Simualtor::getExtForce()
    {
    
    }
    
}
