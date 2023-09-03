#ifndef FCG_TRAB_FINAL_CAMERA_H
#define FCG_TRAB_FINAL_CAMERA_H

#include "matrices.h"
#define FOV M_PI / 3.0f // 60º

// Estrutura de teclas pressionadas
struct Keys {
    bool W, A, S, D, M1, M2;

    Keys(){
        this->W = false;
        this->A = false;
        this->S = false;
        this->D = false;
        this->M1 = false;
        this->M2 = false;
    }
};

// Estrutura para coordenadas esféricas
struct SphericPosition {
    float phi, theta, distance;

    SphericPosition() {
        this->phi = 0.0f;
        this->theta = 0.0f;
        this->distance = 1.0f;
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
        glm::vec4 lookAt;

    public:
        Camera();
        glm::vec4 getLookAt(); // Ponto "l", para onde a câmera (look-at) estará sempre olhando
        void setLookAt(glm::vec4 vec);
        glm::vec4 getViewVector();  // Vetor "view", sentido para onde a câmera está virada
        glm::mat4 getView(); // Matriz "view"
        [[nodiscard]] glm::mat4 getPerspective(float aspectRatio) const; // Matriz "perspective"

        /* Características vetoriais */
        static glm::vec4 upVector;

        // Atualização da câmera
        void updateCamera(float delta_t);
        void updateViewVector(float angleX, float angleY);

        // Atualização das coordenadas esféricas
        void updateSphericAngles(float dx, float dy);
        void updateSphericAngles(float angle);
        void updateSphericDistance(float distance);

        // Atualização das coordendas cartesianas
        void updateCartesianCoordinates(glm::vec4 coords);

        // Controle de tipo de câmera
        [[nodiscard]] bool isUseFreeCamera() const;
        void revertFreeCamera();

        /* Variáveis de controle de pressionar teclas e botões do mouse */
        Keys keys;
    };

#endif //FCG_TRAB_FINAL_CAMERA_H
