//
// Created by Arthur on 8/27/2023.
//

#include "collisions.h"
#include <cmath>

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

bool collisions::CylinderToCylinder(glm::vec3 cylinder1Bbox_min, glm::vec3 cylinder1Bbox_max, glm::vec3 cylinder2Bbox_min, glm::vec3 cylinder2Bbox_max) {

    glm::vec3 cylinder1Center = 0.5f * (cylinder1Bbox_min + cylinder1Bbox_max);
    glm::vec3 cylinder2Center = 0.5f * (cylinder2Bbox_min + cylinder2Bbox_max);
    float cylinder1Radius = 0.5f * glm::distance(cylinder1Bbox_min, cylinder1Bbox_max);
    float cylinder2Radius = 0.5f * glm::distance(cylinder2Bbox_min, cylinder2Bbox_max);

    float distance = glm::distance(cylinder1Center, cylinder2Center);

    float sumOfRadii = cylinder1Radius + cylinder2Radius;

    if (distance <= sumOfRadii) {
        return true; // Colidiu
    }

    return false;

};

bool collisions::CubeToCylinder(glm::vec3 cylinderBbox_min, glm::vec3 cylinderBbox_max, glm::vec3 cubeBbox_min, glm::vec3 cubeBbox_max) {

    if (cubeBbox_max.x < cylinderBbox_min.x || cubeBbox_min.x > cylinderBbox_max.x)
        return false;

    if (cubeBbox_max.z < cylinderBbox_min.z || cubeBbox_min.z > cylinderBbox_max.z)
        return false;

    return true;

}