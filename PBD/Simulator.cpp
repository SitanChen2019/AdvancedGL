#include "Simulator.h"
#include "Particle.h"

namespace PBD
{
    Simualtor& Simualtor::singleton()
    {
        static Simualtor s_instance;
        return s_instance;
    }
    
    Simualtor::Simualtor()
    {

    }

    void Simualtor::init(std::vector<Particle>&& particles,
        std::vector<Triangle>&& triangles
    )
    {
        mParticles.swap(particles);
            mTriangles.swap( triangles);
    }

    void Simualtor::deinit()
    {

        mTriangles.clear();
        mParticles.clear();
    }

    void Simualtor::preUpdate()
    {
        //save pre-posotion
        for (unsigned i = 0, n = (unsigned)mParticles.size(); i < n; ++i)
        {
            Particle& p = mParticles[i];
            p.mPrePos = p.mCurPos;
        }
    }

    void Simualtor::update()
    {
        float t = mSetting.DELTA_T;

        //update velocity by extental force
        for(unsigned i= 0, n = (unsigned)mParticles.size(); i < n; ++i )
        {
            Particle& p = mParticles[i];
            p.mVelocity += t * p.mInvMass* getExtForce(p);
            p.mCurPos += p.mVelocity * t;
        }
        
        //predicate the current position with velocity
        for (unsigned i = 0, n = (unsigned)mParticles.size(); i < n; ++i)
        {
            Particle& p = mParticles[i];
            p.mCurPos += p.mVelocity * t;
        }

        //generate collison constraint
        

        //project constraint
        

        //update velocity
        for (unsigned i = 0, n = (unsigned)mParticles.size(); i < n; ++i)
        {
            Particle& p = mParticles[i];
            p.mVelocity = (p.mCurPos - p.mPrePos) / t;
        }
        
        //velocity update
    }

    void Simualtor::postUpdate()
    {

    }

    
    glm::vec3 Simualtor::getExtForce(const Particle& particle)
    {
        return  mSetting.GRAVITY;
    }
    
}
