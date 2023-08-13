//
// Created by Arthur on 8/12/2023.
//

#ifndef FCG_TRAB_FINAL_RENDERER_H
#define FCG_TRAB_FINAL_RENDERER_H

#include "LoadedObj.h"
#include "Camera.h"
#include "Scene.h"
#include "matrices.h"
#include "glad/glad.h"
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <map>

class Renderer{
    private:
        // Variáveis que definem um programa de GPU (shaders).
        GLuint gpuProgramID;
        GLint model_uniform;
        GLint view_uniform;
        GLint projection_uniform;
        GLint object_id_uniform;

        std::map<std::string, Scene> virtualScene;

        /* Declaração de funções de renderização */
        GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
        GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
        void LoadShader(const char* filename, GLuint shader_id); // Função utilizada pelas duas acima
        GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU
        void DrawVirtualObject(const char* object_name);

    public:
        Renderer();
        void initialize();
        void LoadShadersFromFiles(); // Carrega os shaders de vértice e fragmento, criando um programa de GPU
        void BuildTrianglesAndAddToVirtualScene(LoadedObj*); // Constrói representação de um ObjModel como malha de triângulos para renderização
        void ComputeNormals(LoadedObj* model); // Computa normais de um ObjModel, caso não existam.
        void render(GLFWwindow* window, Camera &camera, const float &screenRatio, float &initialTime);



};


#endif //FCG_TRAB_FINAL_RENDERER_H
