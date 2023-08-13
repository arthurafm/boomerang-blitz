//
// Created by Arthur on 8/12/2023.
//

/* Headers padrões em C */
#include <cmath>
#include <cstdio>
#include <cstdlib>

/* Headers de C++ */
#include <string>

/* Headers de classes do projeto */
#include "Window.h"
#include "Renderer.h"
#include "LoadedObj.h"
#include "Scene.h"

// Construtor do objeto Window
Window::Window() {
    this->screenHeight = 800;
    this->screenWidth = 800;
    this->camera = Camera();
    this->renderer = Renderer();
    this->lastCursorPosX = 0;
    this->lastCursorPosY = 0;
}

void Window::run() {

    // Inicialização da biblioteca GLFW, utilizada para renderizar uma janela
    int success = glfwInit();
    if (!success) {
        fprintf(stderr, "ERROR: glfwInit() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definição do callback de erros para o terminal
    glfwSetErrorCallback(ErrorCallback);

    // Utilização de OpenGL versão 3.3 ou superior
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Utilização de funções modernas de OpenGL
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Criação de uma janela do sistema operacional, de 800x800, com o título de "Boomerang Blitz"
    GLFWwindow* window;
    window = glfwCreateWindow(800, 800, "Boomerang Blitz", NULL, NULL);
    if (!window) {
        glfwTerminate();
        fprintf(stderr, "ERROR: glfwCreateWindow() failed.\n");
        std::exit(EXIT_FAILURE);
    }

    // Definição de funções de callbacks
    glfwSetWindowUserPointer(window, this);

    // Teclas
    glfwSetKeyCallback(window, [](GLFWwindow *window, int key, int scancode, int action, int mode) {
        Window *self = static_cast<Window *>(glfwGetWindowUserPointer(window));
        self->KeyCallback(key, scancode, action, mode);
    });
    // Botões do mouse
    glfwSetMouseButtonCallback(window, [](GLFWwindow *window, int button, int action, int mods) {
        Window *self = static_cast<Window *>(glfwGetWindowUserPointer(window));
        self->MouseButtonCallback(button, action, mods);
    });
    // Cursor do mouse
    glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos) {
        Window *self = static_cast<Window *>(glfwGetWindowUserPointer(window));
        self->CursorPosCallback(xpos, ypos);
    });
    // Scroll do mouse
    glfwSetScrollCallback(window, [](GLFWwindow *window, double xoffset, double yoffset) {
        Window *self = static_cast<Window *>(glfwGetWindowUserPointer(window));
        self->ScrollCallback(xoffset, yoffset);
    });
    // Tamanho da janela
    glfwSetWindowSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        Window *self = static_cast<Window *>(glfwGetWindowUserPointer(window));
        self->FramebufferSizeCallback(width, height);
    });

    glfwSetWindowSize(window, 800, 800); // Definição de screenRatio.

    // Chamadas OpenGL deverão ser renderizadas nessa janela
    glfwMakeContextCurrent(window);

    // Carregamento das funções de OpenGL 3.3, utilizando a GLAD
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    this->renderer.initialize();

    this->renderer.LoadShadersFromFiles();

    // Lê .obj do plano
    LoadedObj planemodel("../data/plane.obj");
    this->renderer.ComputeNormals(&planemodel);
    this->renderer.BuildTrianglesAndAddToVirtualScene(&planemodel);

    // Variável para movimentação baseada em tempo
    auto prevTime = (float) glfwGetTime();

    // Renderização até o usuário fechar a janela
    while (!glfwWindowShouldClose(window)) {
        this->renderer.render(window, this->camera, (float) (this->screenWidth / this->screenHeight), prevTime);
    }

    // Finaliza o uso do sistema operacional
    glfwTerminate();

}

// Definição de callback de redimensionamento de tela
void Window::FramebufferSizeCallback(int width, int height)
{
    glViewport(0, 0, width, height);
    this->screenHeight = height;
    this->screenWidth = width;
}

// Definimos o callback para impressão de erros da GLFW no terminal
void Window::ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

void Window::KeyCallback(int key, int scancode, int action, int mode) {

    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(glfwGetCurrentContext(), GL_TRUE);

    // Se o usuário apertar a tecla C, a câmera é trocada
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        this->camera.revertFreeCamera();
    }

    // Se o usuário apertar as teclas de movimento da câmera
    if (key == GLFW_KEY_W) {
        if (action == GLFW_PRESS) {
            this->camera.keys.W = true;
        }
        else if (action == GLFW_RELEASE) {
            this->camera.keys.W = false;
        }
    }
    if (key == GLFW_KEY_A) {
        if (action == GLFW_PRESS) {
            this->camera.keys.A = true;
        }
        else if (action == GLFW_RELEASE) {
            this->camera.keys.A = false;
        }
    }
    if (key == GLFW_KEY_S) {
        if (action == GLFW_PRESS) {
            this->camera.keys.S = true;
        }
        else if (action == GLFW_RELEASE) {
            this->camera.keys.S = false;
        }
    }
    if (key == GLFW_KEY_D) {
        if (action == GLFW_PRESS) {
            this->camera.keys.D = true;
        }
        else if (action == GLFW_RELEASE) {
            this->camera.keys.D = false;
        }
    }
}

void Window::MouseButtonCallback(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        // Se o usuário pressionou o botão esquerdo do mouse, guardamos a posição atual do cursor.
        glfwGetCursorPos(glfwGetCurrentContext(), &this->lastCursorPosX, &this->lastCursorPosY);
        this->camera.keys.M1 = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        // Quando o usuário soltar o botão esquerdo do mouse, atualizamos a variável
        this->camera.keys.M1 = false;
    }
}

void Window::CursorPosCallback(double xpos, double ypos) {
    // Abaixo executamos o seguinte: caso o botão esquerdo do mouse esteja
    // pressionado, computamos quanto que o mouse se movimento desde o último
    // instante de tempo, e usamos esta movimentação para atualizar os
    // parâmetros que definem a posição da câmera dentro da cena virtual.
    // Assim, temos que o usuário consegue controlar a câmera.

    if (!this->camera.keys.M1)
        return;

    // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
    float dx = xpos - this->lastCursorPosX;
    float dy = ypos - this->lastCursorPosY;

    // Atualizamos parâmetros da câmera com os deslocamentos
    if (this->camera.isUseFreeCamera()) {

        // Calcula o ângulo rotação horizontal de acordo com a porcentagem da tela movida (máximo = 2PI)
        float angleX = dx/((float) this->screenWidth/2)  * 2 * M_PI;
        float angleY = dy/((float) this->screenHeight/2) * 2 * M_PI;
        this->camera.updateViewVector(angleX, angleY);
    }
    else {
        this->camera.updateSphericAngles(dx, dy);
    }

    // Atualizamos as variáveis globais para armazenar a posição atual do
    // cursor como sendo a última posição conhecida do cursor.
    this->lastCursorPosX = xpos;
    this->lastCursorPosY = ypos;
}

void Window::ScrollCallback(double xoffset, double yoffset) {
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    this->camera.updateSphericDistance(yoffset);
}