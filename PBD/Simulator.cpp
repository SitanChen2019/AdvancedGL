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

    void Simualtor::init(std::vector<Particle>* pClientParticles,
        std::vector<Triangle>* pTriangles
    )
    {
        mpClientParticles = pClientParticles;
        mpTriangles = pTriangles;

        mParticles.reserve(mpClientParticles->size());
        for (unsigned i = 0, n = (unsigned)mParticles.size(); i < n; ++i)
        {
            Particle& p = mParticles[i];
            p = (*mpClientParticles)[i];
        }
    }

    void Simualtor::deinit()
    {
        mpClientParticles = nullptr;;
        mpTriangles = nullptr;

        mParticles.clear();
    }

    void Simualtor::preUpdate()
    {
        if (mpClientParticles == nullptr)
            return;

        //copy from client
        for (unsigned i = 0, n = (unsigned)mParticles.size(); i < n; ++i)
        {
            Particle& p = mParticles[i];
            p = (*mpClientParticles)[i];
        }

        //save pre-posotion
        for (unsigned i = 0, n = (unsigned)mParticles.size(); i < n; ++i)
        {
            ParticleExt& p = mParticles[i];
            p.mPrePos = p.mCurPos;
        }
    }

    void Simualtor::update()
    {
        if (mpClientParticles == nullptr)
            return;

        float t = mSetting.DELTA_T;

        //update velocity by extental force
        for(unsigned i= 0, n = (unsigned)mParticles.size(); i < n; ++i )
        {
            ParticleExt& p = mParticles[i];
            p.mVelocity += t * p.mInvMass* getExtForce(p);
            p.mCurPos += p.mVelocity * t;
        }
        
        //predicate the current position with velocity
        for (unsigned i = 0, n = (unsigned)mParticles.size(); i < n; ++i)
        {
            ParticleExt& p = mParticles[i];
            p.mCurPos += p.mVelocity * t;
        }

        //generate collison constraint
        

        //project constraint
        

        //update velocity
        for (unsigned i = 0, n = (unsigned)mParticles.size(); i < n; ++i)
        {
            ParticleExt& p = mParticles[i];
            p.mVelocity = (p.mCurPos - p.mPrePos) / t;
        }
        
        //velocity update
    }

    void Simualtor::postUpdate()
    {
        if (mpClientParticles == nullptr)
            return;

        //copy back to client
        for (unsigned i = 0, n = (unsigned)mParticles.size(); i < n; ++i)
        {
            ParticleExt& p = mParticles[i];
            (*mpClientParticles)[i] = (Particle)(p);
        }
    }

    
    glm::vec3 Simualtor::getExtForce(const ParticleExt& particle)
    {
        return  mSetting.GRAVITY;
    }
    
}
