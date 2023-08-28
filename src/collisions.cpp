//
// Created by Arthur on 8/27/2023.
//

#include "collisions.h"

bool collisions::CubeToBox(glm::vec3 cubeBbox_min, glm::vec3 cubeBbox_max, glm::vec3 boxBbox_min, glm::vec3 boxBbox_max) {

    // Checa para sobreposição
    if (cubeBbox_min.x >= boxBbox_min.x &&
        cubeBbox_max.x <= boxBbox_max.x &&
        cubeBbox_min.z >= boxBbox_min.z &&
        cubeBbox_max.z <= boxBbox_max.z) {
        return false; // Está dentro da caixa
    }
    else {
        return true; // Saiu da caixa
    }

}