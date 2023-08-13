//
// Created by Arthur on 8/12/2023.
//

#include "Camera.h"

// Construtor do objeto Camera
Camera::Camera() {

    this->cartesianPosition = glm::vec4(0.0f,0.0f,2.5f,1.0f);

    this->useFreeCamera = true;

    this->updateViewVector();

    this->nearPlane = -0.1f;
    this->farPlane = -10.0f;


}

// Ponto "c", centro da câmera em coordenadas cartesianas
glm::vec4 Camera::getPosition() {

    return this->cartesianPosition;
}

// Ponto "l", para onde a câmera (look-at) estará sempre olhando.
glm::vec4 Camera::getLookAt() {

    return glm::vec4(0.0f,-1.0f,0.0f,1.0f);
}

// Vetor "view", sentido para onde a câmera está virada
glm::vec4 Camera::getViewVector() {

    return this->viewVector;
}

// Atualiza o View Vector em câmera Look-At
void Camera::updateViewVector() {
    this->cartesianPosition.x = this->sphericPosition.distance * cos(this->sphericPosition.phi)*sin(this->sphericPosition.theta);
    this->cartesianPosition.y = this->sphericPosition.distance * sin(this->sphericPosition.phi);
    this->cartesianPosition.z = this->sphericPosition.distance * cos(this->sphericPosition.phi)*cos(this->sphericPosition.theta);
    this->viewVector = normalize(this->getLookAt() - this->cartesianPosition);
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
    return Matrix_Camera_View(this->cartesianPosition, this->viewVector, Camera::upVector);
}

glm::mat4 Camera::getPerspective(float aspectRatio) {
    return Matrix_Perspective(FOV, aspectRatio, this->nearPlane, this->farPlane);
}

void Camera::updateCamera(float delta_t) {

    float cameraSpeed = 2.0f;
    if (this->useFreeCamera) {
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
    }
    else {
        this->updateViewVector();
    }
}

bool Camera::isUseFreeCamera() const{
    return useFreeCamera;
}

void Camera::revertFreeCamera() {
    this->useFreeCamera = !this->useFreeCamera;
}