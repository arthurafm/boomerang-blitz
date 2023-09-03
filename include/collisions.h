#ifndef FCG_TRAB_FINAL_COLLISIONS_H
#define FCG_TRAB_FINAL_COLLISIONS_H


#include "Model.h"

class collisions {

    public:
        // Colisão entre objetos e o cenário
        static bool CubeToBox(glm::vec3 cubeBbox_min, glm::vec3 cubeBbox_max, glm::vec3 boxBbox_min, glm::vec3 boxBbox_max);
        // Colisão entre robô e zumbis
        static bool CylinderToCylinder(glm::vec3 cylinder1Bbox_min, glm::vec3 cylinder1Bbox_max, glm::vec3 cylinder2Bbox_min, glm::vec3 cylinder2Bbox_max);
        // Colisão entre bumerange e zumbis
        static bool CubeToCylinder(glm::vec3 cylinderBbox_min, glm::vec3 cylinderBbox_max, glm::vec3 cubeBbox_min, glm::vec3 cubeBbox_max);
};


#endif //FCG_TRAB_FINAL_COLLISIONS_H
