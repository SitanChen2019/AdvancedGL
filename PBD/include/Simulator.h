#ifndef PBD_SIMULATOR_H
#define PBD_SIMULATOR_H

#include "defs.h"
#include "Particle.h"
#include <vector>
#include "SimSettings.h"

namespace PBD
{
 
    class EXPORT_SYMBOL Simualtor
    {
    public:
        static Simualtor& singleton();


        void preUpdate();
        void update();
        void postUpdate();
        
        void init(std::vector<Particle>* pClientParticles,
            std::vector<Triangle>* pTriangles
            );
        void deinit();

    private:
        Simualtor();
        _worker glm::vec3  getExtForce( const ParticleExt& );

    private:
        SimSettings mSetting;

        std::vector<Particle>* mpClientParticles = nullptr;;
        std::vector<Triangle>* mpTriangles = nullptr;

        std::vector<ParticleExt> mParticles;
    };
}
#endif
