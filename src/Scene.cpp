//
// Created by Arthur on 8/12/2023.
//

#include "Scene.h"

Scene::Scene(std::string name, size_t first_index, size_t num_indices, GLenum rendering_mode, GLuint vertex_array_object_id) {
    this->name = name;
    this->first_index = first_index;
    this->num_indices = num_indices;
    this->rendering_mode = rendering_mode;
    this->vertex_array_object_id = vertex_array_object_id;
}

Scene::Scene() {

}