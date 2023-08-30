//
// Created by Arthur on 8/27/2023.
//

#ifndef FCG_TRAB_FINAL_MODEL_H
#define FCG_TRAB_FINAL_MODEL_H

#include "glm/vec4.hpp"
#include "LoadedObj.h"
#include "SceneObject.h"
#include "Camera.h"
#include <string>

class Model {
    private:
        glm::vec3 scale;
        std::string name;
        LoadedObj obj;
        int objectId;
        glm::vec3 position;
        glm::vec3 direction;
        float rotation;

        float x_difference;
        float z_difference;

        glm::vec3 originalPosition;

        void ComputeNormals();
        void BuildTrianglesAndAddToVirtualScene(std::map<std::string, SceneObject> &virtualScene);

    public:
        Model(int id, glm::vec3 position, glm::vec3 scale, glm::vec3 direction, float rotation, const char* name, const char* path, std::map<std::string, SceneObject> &virtualScene);

        void updatePlayer(float delta_t, Camera &camera, const Model& box);

        glm::vec3 bbox_min; // Axis-Aligned Bounding Box do objeto
        glm::vec3 bbox_max;

        void updateBbox();

        glm::vec3 getPosition();
        glm::vec3 getDirection();
        glm::vec3 getScale();
        [[nodiscard]] float getRotation() const;
        std::string getName();
        [[nodiscard]] int getId() const;
        glm::vec3 getOriginalPosition();

        void setDirection(glm::vec3 direction);
        void setPosition(glm::vec3 position);
        void updateOriginalPosition();

};


#endif //FCG_TRAB_FINAL_MODEL_H
