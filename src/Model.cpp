//
// Created by Arthur on 8/27/2023.
//

#include "Model.h"
#include "matrices.h"
#include "glad/glad.h"

Model::Model(int id, glm::vec3 position, glm::vec3 scale, const char* name, const char* path, std::map<std::string, Scene> &virtualScene) {
    this->objectId = id;
    this->position = position;
    this->scale = scale;
    this->name = name;
    this->obj = LoadedObj(path);
    this->ComputeNormals();
    this->BuildTrianglesAndAddToVirtualScene(virtualScene);
}

// Função que computa as normais de um ObjModel, caso elas não tenham sido especificadas.
void Model::ComputeNormals()
{
    if ( !(this->obj).attrib.normals.empty() )
        return;

    // Primeiro computamos as normais para todos os TRIÂNGULOS.
    // Segundo, computamos as normais dos VÉRTICES através do método proposto por Gouraud.

    size_t num_vertices = (this->obj).attrib.vertices.size() / 3;

    std::vector<int> num_triangles_per_vertex(num_vertices, 0);
    std::vector<glm::vec4> vertex_normals(num_vertices, glm::vec4(0.0f,0.0f,0.0f,0.0f));

    for (size_t shape = 0; shape < (this->obj).shapes.size(); ++shape)
    {
        size_t num_triangles = (this->obj).shapes[shape].mesh.num_face_vertices.size();

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert((this->obj).shapes[shape].mesh.num_face_vertices[triangle] == 3);

            glm::vec4  vertices[3];
            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = (this->obj).shapes[shape].mesh.indices[3*triangle + vertex];
                const float vx = (this->obj).attrib.vertices[3*idx.vertex_index + 0];
                const float vy = (this->obj).attrib.vertices[3*idx.vertex_index + 1];
                const float vz = (this->obj).attrib.vertices[3*idx.vertex_index + 2];
                vertices[vertex] = glm::vec4(vx,vy,vz,1.0);
            }

            const glm::vec4  a = vertices[0];
            const glm::vec4  b = vertices[1];
            const glm::vec4  c = vertices[2];

            const glm::vec4 n = crossproduct(b - a, c - a);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = (this->obj).shapes[shape].mesh.indices[3*triangle + vertex];
                num_triangles_per_vertex[idx.vertex_index] += 1;
                vertex_normals[idx.vertex_index] += n;
                (this->obj).shapes[shape].mesh.indices[3*triangle + vertex].normal_index = idx.vertex_index;
            }
        }
    }

    (this->obj).attrib.normals.resize( 3*num_vertices );

    for (size_t i = 0; i < vertex_normals.size(); ++i)
    {
        glm::vec4 n = vertex_normals[i] / (float)num_triangles_per_vertex[i];
        n /= norm(n);
        (this->obj).attrib.normals[3*i + 0] = n.x;
        (this->obj).attrib.normals[3*i + 1] = n.y;
        (this->obj).attrib.normals[3*i + 2] = n.z;
    }
}

// Constrói triângulos para futura renderização a partir de um ObjModel.
void Model::BuildTrianglesAndAddToVirtualScene(std::map<std::string, Scene> &virtualScene)
{
    GLuint vertex_array_object_id;
    glGenVertexArrays(1, &vertex_array_object_id);
    glBindVertexArray(vertex_array_object_id);

    std::vector<GLuint> indices;
    std::vector<float>  model_coefficients;
    std::vector<float>  normal_coefficients;
    std::vector<float>  texture_coefficients;

    for (size_t shape = 0; shape < (this->obj).shapes.size(); ++shape)
    {
        size_t first_index = indices.size();
        size_t num_triangles = (this->obj).shapes[shape].mesh.num_face_vertices.size();

        const float minval = std::numeric_limits<float>::min();
        const float maxval = std::numeric_limits<float>::max();

        glm::vec3 bbox_min = glm::vec3(maxval,maxval,maxval);
        glm::vec3 bbox_max = glm::vec3(minval,minval,minval);

        for (size_t triangle = 0; triangle < num_triangles; ++triangle)
        {
            assert((this->obj).shapes[shape].mesh.num_face_vertices[triangle] == 3);

            for (size_t vertex = 0; vertex < 3; ++vertex)
            {
                tinyobj::index_t idx = (this->obj).shapes[shape].mesh.indices[3*triangle + vertex];

                indices.push_back(first_index + 3*triangle + vertex);

                const float vx = (this->obj).attrib.vertices[3*idx.vertex_index + 0];
                const float vy = (this->obj).attrib.vertices[3*idx.vertex_index + 1];
                const float vz = (this->obj).attrib.vertices[3*idx.vertex_index + 2];
                model_coefficients.push_back( vx ); // X
                model_coefficients.push_back( vy ); // Y
                model_coefficients.push_back( vz ); // Z
                model_coefficients.push_back( 1.0f ); // W

                if ( idx.normal_index != -1 )
                {
                    const float nx = (this->obj).attrib.normals[3*idx.normal_index + 0];
                    const float ny = (this->obj).attrib.normals[3*idx.normal_index + 1];
                    const float nz = (this->obj).attrib.normals[3*idx.normal_index + 2];
                    normal_coefficients.push_back( nx ); // X
                    normal_coefficients.push_back( ny ); // Y
                    normal_coefficients.push_back( nz ); // Z
                    normal_coefficients.push_back( 0.0f ); // W
                }

                if ( idx.texcoord_index != -1 )
                {
                    const float u = (this->obj).attrib.texcoords[2*idx.texcoord_index + 0];
                    const float v = (this->obj).attrib.texcoords[2*idx.texcoord_index + 1];
                    texture_coefficients.push_back( u );
                    texture_coefficients.push_back( v );
                }
            }
        }

        size_t last_index = indices.size() - 1;

        Scene theobject((this->obj).shapes[shape].name, first_index, last_index - first_index + 1, GL_TRIANGLES, vertex_array_object_id, bbox_min, bbox_max);

        virtualScene[(this->obj).shapes[shape].name] = theobject;
    }

    GLuint VBO_model_coefficients_id;
    glGenBuffers(1, &VBO_model_coefficients_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_model_coefficients_id);
    glBufferData(GL_ARRAY_BUFFER, model_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, model_coefficients.size() * sizeof(float), model_coefficients.data());
    GLuint location = 0; // "(location = 0)" em "shader_vertex.glsl"
    GLint  number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
    glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if ( !normal_coefficients.empty() )
    {
        GLuint VBO_normal_coefficients_id;
        glGenBuffers(1, &VBO_normal_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_normal_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, normal_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, normal_coefficients.size() * sizeof(float), normal_coefficients.data());
        location = 1; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 4; // vec4 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if ( !texture_coefficients.empty() )
    {
        GLuint VBO_texture_coefficients_id;
        glGenBuffers(1, &VBO_texture_coefficients_id);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_texture_coefficients_id);
        glBufferData(GL_ARRAY_BUFFER, texture_coefficients.size() * sizeof(float), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, texture_coefficients.size() * sizeof(float), texture_coefficients.data());
        location = 2; // "(location = 1)" em "shader_vertex.glsl"
        number_of_dimensions = 2; // vec2 em "shader_vertex.glsl"
        glVertexAttribPointer(location, number_of_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(location);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    GLuint indices_id;
    glGenBuffers(1, &indices_id);

    // "Ligamos" o buffer. Note que o tipo agora é GL_ELEMENT_ARRAY_BUFFER.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());

    // "Desligamos" o VAO.
    glBindVertexArray(0);
}

glm::vec3 Model::getPosition(){
    return this->position;
}

glm::vec3 Model::getScale(){
    return this->scale;
}

std::string Model::getName(){
    return this->name;
}

int Model::getId() const{
    return this->objectId;
}

void Model::updatePlayer(float delta_t, Camera &camera) {

    float speed = 2.0f;
    glm::vec4 w = -(normalize(camera.getViewVector()));
    glm::vec4 u = normalize(crossproduct(Camera::upVector, w));
    if (!camera.isUseFreeCamera()) {
        camera.setLookAt(glm::vec4(this->position, 1.0f));
    }
    if (camera.keys.W){
        this->position -= glm::vec3(w.x, 0.0f, w.z) * speed * delta_t;
    }
    if (camera.keys.S){
        this->position += glm::vec3(w.x, 0.0f, w.z) * speed * delta_t;
    }
    if (camera.keys.A){
        this->position -= glm::vec3(u.x, 0.0f, u.z) * speed * delta_t;
    }
    if (camera.keys.D){
        this->position += glm::vec3(u.x, 0.0f, u.z) * speed * delta_t;
    }

}