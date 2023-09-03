#include "collisions.h"

// Colisão dos modelos com o cenário
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

// Colisão entre objetos humanóides
bool collisions::CylinderToCylinder(glm::vec3 cylinder1Bbox_min, glm::vec3 cylinder1Bbox_max, glm::vec3 cylinder2Bbox_min, glm::vec3 cylinder2Bbox_max) {

    // Cálculo de posição de cilindros
    glm::vec3 cylinder1Center = 0.5f * (cylinder1Bbox_min + cylinder1Bbox_max);
    glm::vec3 cylinder2Center = 0.5f * (cylinder2Bbox_min + cylinder2Bbox_max);
    float cylinder1Radius = 0.5f * glm::distance(cylinder1Bbox_min, cylinder1Bbox_max);
    float cylinder2Radius = 0.5f * glm::distance(cylinder2Bbox_min, cylinder2Bbox_max);

    float distance = glm::distance(cylinder1Center, cylinder2Center);

    float sumOfRadii = cylinder1Radius + cylinder2Radius;

    if (distance <= sumOfRadii) {
        return true; // Colidiu
    }

    return false; // Não colidiu

};

// Colisão entre projétil e objetos
bool collisions::CubeToCylinder(glm::vec3 cylinderBbox_min, glm::vec3 cylinderBbox_max, glm::vec3 cubeBbox_min, glm::vec3 cubeBbox_max) {

    // Cálculo de posição do cilindro
    glm::vec3 cylinderCenter = (cylinderBbox_min + cylinderBbox_max) * 0.5f;
    cylinderCenter.y = cubeBbox_min.y; // Set it to the same height as the cube
    float cylinderRadius = 0.5f * glm::distance(cylinderBbox_min, cylinderBbox_max);

    // Cálculo de intersecção
    glm::vec3 cubeHalfDimensions = (cubeBbox_max - cubeBbox_min) * 0.5f;
    glm::vec3 displacement = cylinderCenter - (cubeBbox_min + cubeHalfDimensions);
    glm::vec3 clampedDisplacement = glm::clamp(displacement, -cubeHalfDimensions, cubeHalfDimensions);
    glm::vec3 closestPoint = cubeBbox_min + cubeHalfDimensions + clampedDisplacement;
    glm::vec3 collisionVector = cylinderCenter - closestPoint;

    // Cálculo de distâncias
    float squaredDistance = glm::dot(collisionVector, collisionVector);
    float sumRadiiSquared = cylinderRadius * cylinderRadius;

    if (squaredDistance < sumRadiiSquared) {
        return true; // Colidiu
    }

    return false; // Não colidiu
}