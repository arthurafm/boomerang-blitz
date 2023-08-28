//
// Created by Arthur on 8/27/2023.
//

#ifndef FCG_TRAB_FINAL_COLLISIONS_H
#define FCG_TRAB_FINAL_COLLISIONS_H


#include "Model.h"

class collisions {

    public:
        static bool CubeToBox(glm::vec3 cubeBbox_min, glm::vec3 cubeBbox_max, glm::vec3 boxBbox_min, glm::vec3 boxBbox_max);

};


#endif //FCG_TRAB_FINAL_COLLISIONS_H
