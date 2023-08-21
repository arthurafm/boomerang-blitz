//
// Created by Arthur on 8/12/2023.
//

#ifndef FCG_TRAB_FINAL_CAMERA_H
#define FCG_TRAB_FINAL_CAMERA_H

#include "matrices.h"
#define FOV M_PI / 3.0f // 60º

struct Keys {
    bool W, A, S, D, M1, M2, M3;

    Keys(){
        this->W = false;
        this->A = false;
        this->S = false;
        this->D = false;
        this->M1 = false;
        this->M2 = false;
        this->M3 = false;
    }
};

struct SphericPosition {
    float phi, theta, distance;

    SphericPosition() {
        this->phi = 0.0f;
        this->theta = 0.0f;
        this->distance = 2.5f;
    }
};

class Camera {

    private:

        /* Variável que define a posição da câmera em coordenadas cartesianas */
        glm::vec4 cartesianPosition;

        /* Variável que define a posição da câmera em coordenadas esféricas */
        SphericPosition sphericPosition;

        /* Variável que controla o tipo de câmera utilizada */
        bool useFreeCamera; // true = 1ª pessoa, false = 3ª pessoa

        /* Planos de visão */
        float nearPlane;
        float farPlane;

        /* Variáveis vetoriais */
        glm::vec4 viewVector;
        void updateViewVector();

    public:
        Camera();
        glm::vec4 getLookAt(); // Ponto "l", para onde a câmera (look-at) estará sempre olhando
        glm::vec4 getViewVector();  // Vetor "view", sentido para onde a câmera está virada
        glm::mat4 getView();
        glm::mat4 getPerspective(float aspectRatio);

        /* Características vetoriais */
        static constexpr glm::vec4 upVector = glm::vec4(0.0f,1.0f,0.0f,0.0f);

        void updateCamera(float delta_t);
        void updateViewVector(float angleX, float angleY);

        void updateSphericAngles(float dx, float dy);
        void updateSphericDistance(float distance);

        bool isUseFreeCamera() const;
        void revertFreeCamera();

        /* Variáveis de controle de pressionar teclas e botões do mouse */
        Keys keys;

    };

#endif //FCG_TRAB_FINAL_CAMERA_H
