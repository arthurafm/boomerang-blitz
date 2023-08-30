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

    // Calculate the center of the cylinder's base
    glm::vec3 cylinderCenter = (cylinderBbox_min + cylinderBbox_max) * 0.5f;
    cylinderCenter.y = cubeBbox_min.y; // Set it to the same height as the cube

    float cylinderRadius = 0.5f * glm::distance(cylinderBbox_min, cylinderBbox_max);

    // Calculate the half dimensions of the cube
    glm::vec3 cubeHalfDimensions = (cubeBbox_max - cubeBbox_min) * 0.5f;

    // Calculate the displacement between the cube's center and the cylinder's base center
    glm::vec3 displacement = cylinderCenter - (cubeBbox_min + cubeHalfDimensions);

    // Clamp the displacement to be within the half dimensions of the cube
    glm::vec3 clampedDisplacement = glm::clamp(displacement, -cubeHalfDimensions, cubeHalfDimensions);

    // Calculate the closest point on the cube to the cylinder's base center
    glm::vec3 closestPoint = cubeBbox_min + cubeHalfDimensions + clampedDisplacement;

    // Calculate the vector between the closest point and the cylinder's base center
    glm::vec3 collisionVector = cylinderCenter - closestPoint;

    // Calculate the squared distance between the cylinder's base center and the closest point on the cube
    float squaredDistance = glm::dot(collisionVector, collisionVector);

    // Calculate the sum of the radii squared
    float sumRadiiSquared = cylinderRadius * cylinderRadius;

    // If the squared distance is less than the sum of the radii squared, there's a collision
    if (squaredDistance < sumRadiiSquared) {
        return true;
    }

    return false;
}