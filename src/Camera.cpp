//
// Created by Arthur on 8/12/2023.
//

#include "Camera.h"

// Construtor do objeto Camera
Camera::Camera() {

    this->cartesianPosition = glm::vec4(0.4f, 0.8f, 0.0f, 1.0f);

    this->useFreeCamera = true;

    this->lookAt = glm::vec4(0.2f, -0.2f, 0.0f, 1.0f);

    this->viewVector = normalize(this->getLookAt() - this->cartesianPosition);

    this->nearPlane = -0.1f;
    this->farPlane = -100.0f;
}

// Ponto "l", para onde a câmera (look-at) estará sempre olhando.
glm::vec4 Camera::getLookAt() {
    return this->lookAt;
}

void Camera::setLookAt(glm::vec4 vec) {
    this->lookAt = vec;
}

// Vetor "view", sentido para onde a câmera está virada
glm::vec4 Camera::getViewVector() {

    return this->viewVector;
}

// Atualiza o View Vector em câmera Look-At
void Camera::updateViewVector() {
    glm::vec4 vec;
    vec.x = this->sphericPosition.distance * cos(this->sphericPosition.phi) * sin(this->sphericPosition.theta);
    vec.y = this->sphericPosition.distance * sin(this->sphericPosition.phi);
    vec.z = this->sphericPosition.distance * cos(this->sphericPosition.phi) * cos(this->sphericPosition.theta);
    vec.w = 1.0f;
    this->viewVector = normalize(this->getLookAt() - vec);
}

// Atualiza o View Vector em câmera livre
void Camera::updateViewVector(float angleX, float angleY) {

    if (angleX == 0 && angleY == 0) {
        return;
    }
    else {
        // Trava da rotação vertical
        glm::vec4 side = crossproduct(Camera::upVector, this->viewVector); // Calcula o lado, para rotacionar verticalmente
        glm::vec4 aux = this->viewVector * Matrix_Rotate(-angleY, side);   // Rotação no eixo lado (vertical)

        if(dotproduct(side, crossproduct(Camera::upVector, aux)) > 0) { // Testa se o novo valor de lado é igual ao antigo
            this->viewVector = aux;
        }

        this->viewVector = normalize(this->viewVector
                                        * Matrix_Rotate(angleX, Camera::upVector));
    }
}

void Camera::updateSphericAngles(float dx, float dy) {

    float newPhi = this->sphericPosition.phi + 0.01f * dy;

    // Em coordenadas esféricas, phi deve ficar entre -pi/2 e +pi/2.
    if (newPhi > M_PI_2) {
        newPhi = M_PI_2;
    }
    if (newPhi < -M_PI_2) {
        newPhi = -M_PI_2;
    }

    this->sphericPosition.phi = newPhi;
    this->sphericPosition.theta -= 0.01f * dx;
}

void Camera::updateSphericDistance(float distance){

    this->sphericPosition.distance -= 0.1f * distance;

    // Uma câmera look-at nunca pode estar exatamente "em cima" do ponto para onde ela está olhando.
    const float verysmallnumber = std::numeric_limits<float>::epsilon();
    if (this->sphericPosition.distance < verysmallnumber)
        this->sphericPosition.distance = verysmallnumber;
}

glm::mat4 Camera::getView() {
    if (this->useFreeCamera) {
        return Matrix_Camera_View(this->cartesianPosition, this->viewVector, Camera::upVector);
    }
    else {
        glm::vec4 vec;
        vec.x = this->sphericPosition.distance * cos(this->sphericPosition.phi) * sin(this->sphericPosition.theta);
        vec.y = this->sphericPosition.distance * sin(this->sphericPosition.phi);
        vec.z = this->sphericPosition.distance * cos(this->sphericPosition.phi) * cos(this->sphericPosition.theta);
        vec.w = 1.0f;

        return Matrix_Camera_View(vec, this->viewVector, Camera::upVector);
    }
}

glm::mat4 Camera::getPerspective(float aspectRatio) {
    return Matrix_Perspective(FOV, aspectRatio, this->nearPlane, this->farPlane);
}

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

    if (!this->useFreeCamera) {
        this->updateViewVector();
    }
}

void Camera::updateCamera(glm::vec4 pos) {
    this->cartesianPosition = pos;
}

bool Camera::isUseFreeCamera() const{
    return useFreeCamera;
}

void Camera::revertFreeCamera() {
    this->useFreeCamera = !this->useFreeCamera;
}
