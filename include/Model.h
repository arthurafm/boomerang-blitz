#ifndef FCG_TRAB_FINAL_MODEL_H
#define FCG_TRAB_FINAL_MODEL_H

#include "glm/vec4.hpp"
#include "LoadedObj.h"
#include "SceneObject.h"
#include "Camera.h"
#include <string>

class Model {
    private:

        // Características do modelo
        glm::vec3 scale;
        std::string name;
        LoadedObj obj;
        int objectId;
        glm::vec3 position;
        glm::vec3 direction;
        float rotation;

        // Posição original do modelo - para cálculos de direção e movimentação
        glm::vec3 originalPosition;

        // Funções para adição na cena virutal
        void ComputeNormals();
        void BuildTrianglesAndAddToVirtualScene(std::map<std::string, SceneObject> &virtualScene);

    public:
        Model(int id, glm::vec3 position, glm::vec3 scale, glm::vec3 direction, float rotation, const char* name, const char* path, std::map<std::string, SceneObject> &virtualScene);

        // Move o jogador
        void updatePlayer(float delta_t, Camera &camera, const Model& box);

        // Move o bumerange
        bool updateBoomerang(bool &boomerangIsThrown,
                                bool &primaryAttackStarts,
                                bool &secondaryAttackStarts,
                                bool &M1,
                                bool &M2,
                                bool &isPaused,
                                float &rotationBoomerang,
                                float &t,
                                float &delta_t,
                                glm::vec3 &robotPosition,
                                float robotRotation,
                                glm::vec3 &sceneryBboxMin,
                                glm::vec3 &sceneryBboxMax,
                                glm::mat4 &model);

        // "Raio" da bounding box
        float x_difference;
        float z_difference;

        // Axis-Aligned Bounding Box do objeto
        glm::vec3 bbox_min;
        glm::vec3 bbox_max;

        // Atualiza bounding box
        void updateBbox();

        // Getters para informações de modelo
        glm::vec3 getPosition();
        glm::vec3 getDirection();
        glm::vec3 getScale();
        [[nodiscard]] float getRotation() const;
        std::string getName();
        [[nodiscard]] int getId() const;
        glm::vec3 getOriginalPosition();

        // Setters para atualização de inforamções de modelo
        void setDirection(glm::vec3 direction);
        void setPosition(glm::vec3 position);
        void updateOriginalPosition();

};


#endif //FCG_TRAB_FINAL_MODEL_H
