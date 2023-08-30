//
// Created by Arthur on 8/27/2023.
//

#ifndef FCG_TRAB_FINAL_COLLISIONS_H
#define FCG_TRAB_FINAL_COLLISIONS_H


#include "Model.h"

class collisions {

    public:
        static bool CubeToBox(glm::vec3 cubeBbox_min, glm::vec3 cubeBbox_max, glm::vec3 boxBbox_min, glm::vec3 boxBbox_max);
        static bool CylinderToCylinder(glm::vec3 cylinder1Bbox_min, glm::vec3 cylinder1Bbox_max, glm::vec3 cylinder2Bbox_min, glm::vec3 cylinder2Bbox_max);
        static bool CubeToCylinder(glm::vec3 cylinderBbox_min, glm::vec3 cylinderBbox_max, glm::vec3 cubeBbox_min, glm::vec3 cubeBbox_max);
};


#endif //FCG_TRAB_FINAL_COLLISIONS_H
