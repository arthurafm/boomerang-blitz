#include "Camera.h"

// Vetor "up"
glm::vec4 Camera::upVector = glm::vec4(0.0f,1.0f,0.0f,0.0f);

// Construtor do objeto Camera
Camera::Camera() {

    this->cartesianPosition = glm::vec4(0.4f, 0.8f, 0.0f, 1.0f);

    this->useFreeCamera = false;

    this->lookAt = glm::vec4(0.5f, 0.8f, 0.0f, 1.0f);

    this->viewVector = normalize(this->getLookAt() - this->cartesianPosition);

    this->nearPlane = -0.1f;
    this->farPlane = -100.0f;
}

// Ponto "l", para onde a câmera (look-at) estará sempre olhando.
glm::vec4 Camera::getLookAt() {
    return this->lookAt;
}

// Seta o ponto de Look-At
void Camera::setLookAt(glm::vec4 vec) {
    this->lookAt = vec;
}

// Vetor "view", sentido para onde a câmera está virada
glm::vec4 Camera::getViewVector() {
    return this->viewVector;
}

// Atualiza o View Vector em câmera Look-At
void Camera::updateViewVector() {

    // Calcula a posição cartesiana a partir da posição esférica
    glm::vec4 vec = lookAt;
    vec.x += this->sphericPosition.distance * cos(this->sphericPosition.phi) * sin(this->sphericPosition.theta);
    vec.y += this->sphericPosition.distance * sin(this->sphericPosition.phi);
    vec.z += this->sphericPosition.distance * cos(this->sphericPosition.phi) * cos(this->sphericPosition.theta);
    vec.w = 1.0f;

    // Calcula o view vector
    this->viewVector = normalize(this->getLookAt() - vec);
}

// Atualiza o View Vector em câmera livre
void Camera::updateViewVector(float angleX, float angleY) {

    // Caso angulo não tenha sido alterado, retorna
    if (angleX == 0 && angleY == 0) {
        return;
    }
    else {
        // Trava da rotação vertical
        glm::vec4 side = crossproduct(Camera::upVector, this->viewVector); // Calcula o lado, para rotacionar verticalmente
        glm::vec4 aux = this->viewVector * Matrix_Rotate(-angleY, side);   // Rotação no eixo lado (vertical)

        // Testa se o novo valor de lado é igual ao antigo
        if(dotproduct(side, crossproduct(Camera::upVector, aux)) > 0) {
            this->viewVector = aux;
        }

        // Atualiza vetor view
        this->viewVector = normalize(this->viewVector
                                        * Matrix_Rotate(angleX, Camera::upVector));
    }
}

// Atualiza as coordenadas esféricas em câmera Look-At
void Camera::updateSphericAngles(float dx, float dy) {

    float newPhi = this->sphericPosition.phi + 0.003f * dy;

    // Na Look-At, o ângulo da câmera deve estar entre 0 e pi/4 - evitar player de olhar para dentro do chão.
    if (newPhi > (M_PI_2 / 2)) {
        newPhi = (M_PI_2 / 2);
    }
    if (newPhi < 0) {
        newPhi = 0;
    }

    // Atualiza as coordendas esféricas
    this->sphericPosition.phi = newPhi;
    this->sphericPosition.theta -= 0.003f * dx;
}

// Atualiza as coordenadas esféricas em câmera livre
void Camera::updateSphericAngles(float angle) {
    this->sphericPosition.theta -= angle;
}

// Atualiza a nova distância em coordendas esféricas
void Camera::updateSphericDistance(float distance) {

    this->sphericPosition.distance -= 0.1f * distance;
    if (this->useFreeCamera) {
        this->useFreeCamera = false;
    }

    if (this->sphericPosition.distance < 0.5f) {
        this->sphericPosition.distance = 0.5f;
        this->useFreeCamera = true;
    }
}

// Atualiza as coordenadas cartesianas
void Camera::updateCartesianCoordinates(glm::vec4 coords){
    this->cartesianPosition = coords;
}

// Calcula matriz View
glm::mat4 Camera::getView() {
    // Para câmera livre
    if (this->useFreeCamera) {
        return Matrix_Camera_View(this->cartesianPosition, this->viewVector, Camera::upVector);
    }
    // Para câmera Look-At
    else {
        glm::vec4 pos;
        pos.x = this->sphericPosition.distance * cos(this->sphericPosition.phi) * sin(this->sphericPosition.theta);
        pos.y = this->sphericPosition.distance * sin(this->sphericPosition.phi);
        pos.z = this->sphericPosition.distance * cos(this->sphericPosition.phi) * cos(this->sphericPosition.theta);
        pos.w = 1.0f;

        glm::vec4 position = lookAt + pos;
        position.w = 1.0f;

        return Matrix_Camera_View(position, this->viewVector, Camera::upVector);
    }
}

// Calcula matriz Perspective
glm::mat4 Camera::getPerspective(float aspectRatio) const {
    return Matrix_Perspective(FOV, aspectRatio, this->nearPlane, this->farPlane);
}

// Atualiza a câmera
void Camera::updateCamera(float delta_t) {

    float cameraSpeed = 2.0f;
    glm::vec4 w = -(normalize(this->getViewVector()));
    glm::vec4 u = normalize(crossproduct(Camera::upVector, w));
    if (this->keys.W){
        this->cartesianPosition -= w * cameraSpeed * delta_t;
    }
    if (this->keys.S){
        this->cartesianPosition += w * cameraSpeed * delta_t;
    }
    if (this->keys.A){
        this->cartesianPosition -= u * cameraSpeed * delta_t;
    }
    if (this->keys.D){
        this->cartesianPosition += u * cameraSpeed * delta_t;
    }
    this->cartesianPosition.y = 0.8f;

    // Caso em Look-At, atualiza o view vector
    if (!this->useFreeCamera) {
        this->updateViewVector();
    }
}

// Getters e Setters

bool Camera::isUseFreeCamera() const{
    return useFreeCamera;
}

void Camera::revertFreeCamera() {
    this->useFreeCamera = !this->useFreeCamera;
}
