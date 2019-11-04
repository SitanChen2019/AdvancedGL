#ifndef PBD_SIMULATOR_H
#define PBD_SIMULATOR_H

#include "exporter.h"
#include "Particle.h"

namespace PBD
{
 
    class EXPORT_SYMBOL Simualtor
    {
    public:
        Simualtor();

        void update();
        
        
    private:
        glm::vec3 getExtForce();
    private:
        std::vector<Particle> mParticles;
    };
}
#endif
