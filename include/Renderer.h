//
// Created by Arthur on 8/12/2023.
//

#ifndef FCG_TRAB_FINAL_RENDERER_H
#define FCG_TRAB_FINAL_RENDERER_H

#include "LoadedObj.h"
#include "Camera.h"
#include "SceneObject.h"
#include "matrices.h"
#include "glad/glad.h"
#include "Model.h"
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <map>
#include <stb_image.h>

struct enemyData {
    glm::vec3 position;
    glm::vec3 direction;
    float rotation;
    glm::vec3 bbox_min;
    glm::vec3 bbox_max;
    float speed;

    enemyData() {
        this->position = glm::vec3(0.0f, 0.0f, 0.0f);
        this->direction = glm::vec3(0.0f, 0.0f, 0.0f);
        this->rotation = 0.0f;
        this->bbox_min = glm::vec3(0.0f, 0.0f, 0.0f);
        this->bbox_max = glm::vec3(0.0f, 0.0f, 0.0f);
        this->speed = 0.0f;
    }

};

class Renderer{
    private:
        // Variáveis que definem um programa de GPU (shaders).
        GLuint gpuProgramID;
        GLint model_uniform;
        GLint view_uniform;
        GLint projection_uniform;
        GLint object_id_uniform;
        GLint bbox_min_uniform;
        GLint bbox_max_uniform;

        // Número de texturas carregadas pela função LoadTextureImage()
        GLuint numLoadedTextures = 0;

        /* Declaração de funções de renderização */
        GLuint LoadShader_Vertex(const char* filename);   // Carrega um vertex shader
        GLuint LoadShader_Fragment(const char* filename); // Carrega um fragment shader
        void LoadShader(const char* filename, GLuint shader_id); // Função utilizada pelas duas acima
        GLuint CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id); // Cria um programa de GPU
        void DrawVirtualObject(const char* object_name);

    public:
        Renderer();
        void initialize();

        std::vector<Model> models;

        std::map<std::string, SceneObject> virtualScene;

        void LoadShadersFromFiles(); // Carrega os shaders de vértice e fragmento, criando um programa de GPU
        void LoadTextureImage(const char* filename); // Função que carrega imagens de textura

        bool render(GLFWwindow* window, bool isPaused, Camera &camera, const float &aspectRatio, float &initialTime, float &spawnTime);



};


#endif //FCG_TRAB_FINAL_RENDERER_H
