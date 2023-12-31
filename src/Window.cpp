/* Headers padrões em C */
#include <cmath>
#include <cstdio>
#include <cstdlib>

/* Headers de C++ */
#include <string>

/* Headers de classes do projeto */
#include "Window.h"
#include "Renderer.h"
#include "SceneObject.h"
#include "Model.h"

// Definição de constantes para identificação de modelos
#define SCENERY 0
#define ROBOT 1
#define ZOMBIE 2
#define BOOMERANG 3

// Construtor do objeto Window
Window::Window() {
    this->screenHeight = 800;
    this->screenWidth = 800;
    this->camera = Camera();
    this->renderer = Renderer();
    this->lastCursorPosX = 0;
    this->lastCursorPosY = 0;
    this->isPaused_ = true;
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

    // Criação de uma janela do sistema operacional, com o título de "Boomerang Blitz"
    GLFWwindow* window;

    window = glfwCreateWindow(this->screenWidth, this->screenHeight, "Boomerang Blitz", NULL, NULL);
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
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height) {
        Window *self = static_cast<Window *>(glfwGetWindowUserPointer(window));
        self->FramebufferSizeCallback(width, height);
    });

    glfwSetWindowSize(window, this->screenWidth, this->screenHeight); // Definição de screenRatio.

    // Chamadas OpenGL deverão ser renderizadas nessa janela
    glfwMakeContextCurrent(window);

    // Carregamento das funções de OpenGL 3.3, utilizando a GLAD
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    // Carregamos os shaders de vértices e de fragmentos que serão utilizados para renderização.
    this->renderer.LoadShadersFromFiles();

    // Carregamos imagens para serem utilizadas como textura
    this->renderer.LoadTextureImage("../data/textures/floor.jpg");
    this->renderer.LoadTextureImage("../data/textures/robot.tga");
    this->renderer.LoadTextureImage("../data/textures/zombie.png");
    this->renderer.LoadTextureImage("../data/textures/wood.jpg");

    // Cria modelo do cenário
    Model scenery(SCENERY,
                  glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(8.0f, 8.0f, 8.0f),
                  glm::vec3(0.0f, 0.0f, 0.0f),
                  0.0f,
                  "the_scene",
                  "../data/objects/scenery.obj",
                  this->renderer.virtualScene);

    this->renderer.models.push_back(scenery);

    // Cria modelo do robô
    Model robot(ROBOT,
                 glm::vec3(0.4f, 0.0f, 0.0f),
                 glm::vec3(1.0f, 1.0f, 1.0f),
                 glm::vec3(0.0f, 0.0f, 1.0f),
                 0.0f,
                 "the_robot",
                 "../data/objects/robot.obj",
                 this->renderer.virtualScene);

    this->renderer.models.push_back(robot);

    // Cria modelo do zumbi
    Model zombie(ZOMBIE,
                 glm::vec3(-0.4f, 0.0f, 0.0f),
                 glm::vec3(0.5f, 0.5f, 0.5f),
                 glm::vec3(0.0f, 0.0f, 0.0f),
                 0.0f,
                 "the_zombie",
                 "../data/objects/zombie.obj",
                 this->renderer.virtualScene);

    this->renderer.models.push_back(zombie);

    // Cria modelo do bumerange
    Model boomerang(BOOMERANG,
                    glm::vec3(0.0f, 0.7f, 0.0f),
                    glm::vec3(0.01f, 0.01f, 0.01f),
                    glm::vec3(0.0f, 0.0f, 0.0f),
                    M_PI_2,
                    "the_boomerang",
                    "../data/objects/boomerang.obj",
                    this->renderer.virtualScene);

    this->renderer.models.push_back(boomerang);

    // Inicializa o renderizador
    this->renderer.initialize();

    // Variável para movimentação baseada em tempo
    auto prevTime = (float) glfwGetTime();
    auto spawnTime = prevTime;

    // Renderização até o usuário fechar a janela
    while (!glfwWindowShouldClose(window)) {
        if (!this->renderer.render(window, this->isPaused_, this->camera, ((float) this->screenWidth / (float) this->screenHeight), prevTime, spawnTime)) {
            break;
        }
    }

    // Finaliza o uso do sistema operacional
    glfwTerminate();

}

// Definição de callback de redimensionamento de tela
void Window::FramebufferSizeCallback(int width, int height)
{
    // Atualiza tamanho da janela
    glViewport(0, 0, width, height);
    this->screenHeight = height;
    this->screenWidth = width;
}

// Definimos o callback para impressão de erros da GLFW no terminal
void Window::ErrorCallback(int error, const char* description)
{
    fprintf(stderr, "ERROR: GLFW: %s\n", description);
}

// Definimos o callback de pressionar teclas
void Window::KeyCallback(int key, int scancode, int action, int mode) {

    // Se o usuário pressionar a tecla ESC, fechamos a janela.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(glfwGetCurrentContext(), GL_TRUE);

    // Se o usuário apertar a tecla C, a câmera é trocada
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        this->camera.revertFreeCamera();
    }

    // Se o usuário apertar a tecla Space, o jogo é pausado
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        this->isPaused_ = !this->isPaused_;
        if (this->isPaused_) {
            glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else {
            glfwSetInputMode(glfwGetCurrentContext(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

    // Caso esteja pausado, para todos os movimentos
    if (this->isPaused_) {
        this->camera.keys.W = false;
        this->camera.keys.A = false;
        this->camera.keys.S = false;
        this->camera.keys.D = false;

        return;
    }

    // Se o usuário apertar as teclas de movimento
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

// Definimos o callback de pressionar os botões do mouse
void Window::MouseButtonCallback(int button, int action, int mods) {

    // Caso o jogo esteja pausado
    if (this->isPaused_) {
        return;
    }

    // Caso o usuário pressione os botões do mouse
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        this->camera.keys.M1 = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        this->camera.keys.M1 = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        this->camera.keys.M2 = true;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        this->camera.keys.M2 = false;
    }
}

// Definimos o callback de movimentar o mouse
void Window::CursorPosCallback(double xpos, double ypos) {

    // Deslocamento do cursor do mouse em x e y de coordenadas de tela!
    float dx = xpos - this->lastCursorPosX;
    float dy = ypos - this->lastCursorPosY;

    // Caso esteja pausado, não atualiza a posição do cursor
    if (this->isPaused_) {
        this->lastCursorPosX = xpos;
        this->lastCursorPosY = ypos;
        return;
    }

    // Atualizamos parâmetros da câmera com os deslocamentos
    if (this->camera.isUseFreeCamera()) {

        // Em câmera livre, calcula o ângulo rotação horizontal conforme a porcentagem da tela movida
        float angleX = dx/((float) this->screenWidth/2)  * 2 * M_PI;
        float angleY = dy/((float) this->screenHeight/2) * 2 * M_PI;
        this->camera.updateViewVector(angleX, angleY);
        this->camera.updateSphericAngles(angleX);
    }
    else {
        // Em câmera look-at, atualiza os angulos esféricos para os do cursor
        this->camera.updateSphericAngles(dx, dy);
    }

    // Atualizamos as variáveis globais para armazenar a posição atual do cursor como sendo a última posição conhecida do cursor.
    this->lastCursorPosX = xpos;
    this->lastCursorPosY = ypos;
}

// Definimos o callback de scrollar o mouse
void Window::ScrollCallback(double xoffset, double yoffset) {
    // Atualizamos a distância da câmera para a origem utilizando a
    // movimentação da "rodinha", simulando um ZOOM.
    this->camera.updateSphericDistance(yoffset);
}
