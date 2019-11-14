#include "Simulator.h"
#include "Particle.h"
#include "Constraints.h"
#include <glm/glm.hpp>

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
        std::vector<Triangle>&& triangles,
        std::vector<Constraint>&& constraints
    )
    {
        mParticles.swap(particles);
        mTriangles.swap( triangles);
        mConstraints.swap(constraints);
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
            if (p.mInvMass == 0)
                continue;

            p.mVelocity += t * p.mInvMass* getExtForce(p);
        }
        
        //predicate the current position with velocity
        for (unsigned i = 0, n = (unsigned)mParticles.size(); i < n; ++i)
        {
            Particle& p = mParticles[i];
            p.mCurPos += p.mVelocity * t;
        }

        //generate collison constraint
        

        //project constraint
        projectConstraint();

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
        assert(particle.mInvMass != 0);
        return  mSetting.GRAVITY/ particle.mInvMass;
    }

    void Simualtor::projectConstraint()
    {
        for( auto& constraint : mConstraints)
        {
            constraint.solve( this );
        }
    }

     void Simualtor::solveConstraint(const DistanceConstraint& disCons)
     {
         auto& p0 = mParticles[disCons.mId0].mCurPos;
         auto& p1 = mParticles[disCons.mId1].mCurPos;
         
         float w0 = mParticles[disCons.mId0].mInvMass;
         float w1 = mParticles[disCons.mId1].mInvMass;
         
         if( (w0+w1) == 0)
             return;
         
         float c = glm::distance( p0, p1) - disCons.mDistance;
         auto dp0 = glm::normalize(p1-p0);
         auto dp1 = - dp0;
         
         float lamda = -c/(w0+w1);
         
         auto delat_p0 = lamda * w0 * dp0;
         auto delat_p1 = lamda * w1 * dp1;
         
         mParticles[disCons.mId0].mCurPos += delat_p0;
         mParticles[disCons.mId1].mCurPos += delat_p1;

     }





}
