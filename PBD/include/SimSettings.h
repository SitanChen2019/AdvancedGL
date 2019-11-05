#ifndef PDB_SIM_SETTING_H
#define PBD_SIM_SETTING_H

#include <glm/glm.hpp>

namespace PBD {
    struct SimSettings
    {
        glm::vec3 GRAVITY = glm::vec3(0, -9.8, 0);
        float DELTA_T = 0.05f;
    };
}

#endif