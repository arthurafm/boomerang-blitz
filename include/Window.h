//
// Created by Arthur on 8/12/2023.
//

#ifndef FCG_TRAB_FINAL_WINDOW_H
#define FCG_TRAB_FINAL_WINDOW_H

#include "Camera.h"
#include "Renderer.h"

/* Headers das bibliotecas de OpenGL */
#include <glad/glad.h>   // Criação de contexto OpenGL 3.3
#include <GLFW/glfw3.h>  // Criação de janelas do sistema operacional


class Window {
    private:
        // Proporções da janela.
        int screenHeight;
        int screenWidth;
        Camera camera;
        Renderer renderer;

        /* Posições relacionadas ao cursor */
        double lastCursorPosX;
        double lastCursorPosY;

        bool isPaused;

        /* Funções callback para comunicação com o sistema operacional e interação com o usuário */
        void FramebufferSizeCallback(int width, int height);
        static void ErrorCallback(int error, const char* description);
        void KeyCallback(int key, int scancode, int action, int mode);
        void MouseButtonCallback(int button, int action, int mods);
        void CursorPosCallback(double xpos, double ypos);
        void ScrollCallback(double xoffset, double yoffset);

    public:
        Window();
        void run();
};


#endif //FCG_TRAB_FINAL_WINDOW_H
