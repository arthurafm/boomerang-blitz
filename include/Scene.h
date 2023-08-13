//
// Created by Arthur on 8/12/2023.
//

#ifndef FCG_TRAB_FINAL_SCENE_H
#define FCG_TRAB_FINAL_SCENE_H

// Headers de C++
#include <string>

// Headers de OpenGL
#include <glad/glad.h>

class Scene{
    public:
        std::string  name;        // Nome do objeto
        size_t       first_index; // Índice do primeiro vértice dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
        size_t       num_indices; // Número de índices do objeto dentro do vetor indices[] definido em BuildTrianglesAndAddToVirtualScene()
        GLenum       rendering_mode; // Modo de rasterização (GL_TRIANGLES, GL_TRIANGLE_STRIP, etc.)
        GLuint       vertex_array_object_id; // ID do VAO onde estão armazenados os atributos do modelo

        Scene(std::string name, size_t first_index, size_t num_indices, GLenum rendering_mode, GLuint vertex_array_object_id);
        Scene();
};


#endif //FCG_TRAB_FINAL_SCENE_H
