#ifndef PBD_SIMULATOR_H
#define PBD_SIMULATOR_H

#include "defs.h"
#include "Particle.h"
#include <vector>
#include "SimSettings.h"
#include "Constraints.h"

namespace PBD
{
 
    class EXPORT_SYMBOL Simualtor
    {
    public:
        static Simualtor& singleton();


        void preUpdate();
        void update();
        void postUpdate();
        
        void init(std::vector<Particle>&& particles,
            std::vector<Triangle>&& triangles,
            std::vector<Constraint>&& constraints
            );
        void deinit();

        std::vector<Particle>& getParticles()
        {
            return mParticles;
        }

        const std::vector<Triangle>& getTriangles() const
        {
            return mTriangles;
        }
        void solveConstraint(const DistanceConstraint& disCons);

    private:
        Simualtor();
        _worker glm::vec3  getExtForce( const Particle& );

        void projectConstraint();


    private:
        SimSettings mSetting;

        std::vector<Particle> mParticles;
        std::vector<Triangle> mTriangles;
        std::vector<Constraint> mConstraints;
    };
}
#endif
