//
// Created by Arthur on 8/27/2023.
//

#include "Model.h"
#include "matrices.h"
#include "glad/glad.h"
#include "collisions.h"

// Inicializa atributos, computa normais e constrói triângulos
Model::Model(int id, glm::vec3 position, glm::vec3 scale, glm::vec3 direction, float rotation, const char* name, const char* path, std::map<std::string, SceneObject> &virtualScene) {
    this->objectId = id;
    this->position = position;
    this->originalPosition = position;
    this->scale = scale;
    this->direction = normalize(direction);
    this->rotation = rotation;
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
void Model::BuildTrianglesAndAddToVirtualScene(std::map<std::string, SceneObject> &virtualScene)
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

                bbox_min.x = std::min(bbox_min.x, vx);
                bbox_min.y = std::min(bbox_min.y, vy);
                bbox_min.z = std::min(bbox_min.z, vz);
                bbox_max.x = std::max(bbox_max.x, vx);
                bbox_max.y = std::max(bbox_max.y, vy);
                bbox_max.z = std::max(bbox_max.z, vz);

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

        SceneObject theobject((this->obj).shapes[shape].name, first_index, last_index - first_index + 1, GL_TRIANGLES, vertex_array_object_id, bbox_min, bbox_max);

        theobject.bbox_max = bbox_max;
        theobject.bbox_min = bbox_min;

        this->bbox_max = bbox_max;
        this->bbox_min = bbox_min;

        this->x_difference = (this->bbox_max.x - this->bbox_min.x) / 2 * this->scale.x;
        this->z_difference = (this->bbox_max.z - this->bbox_min.z) / 2 * this->scale.z;

        this->updateBbox();

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

// Getters

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

float Model::getRotation() const{
    return this->rotation;
}

// Setters

void Model::setPosition(glm::vec3 position){
    this->position.x = position.x;
    this->position.z = position.z;
}

void Model::setDirection(glm::vec3 direction) {
    this->direction = direction;
}

void Model::updateOriginalPosition() {
    this->originalPosition = this->position;
}

glm::vec3 Model::getOriginalPosition() {
    return this->originalPosition;
}

glm::vec3 Model::getDirection(){
    return this->direction;
}

// Atualiza a bounding box a partir da posição atual e os "raios" salvos
void Model::updateBbox(){
    this->bbox_max = glm::vec3(this->position.x + this->x_difference, this->position.y, this->position.z + this->z_difference);
    this->bbox_min = glm::vec3(this->position.x - this->x_difference, this->position.y, this->position.z - this->z_difference);
}

// Atualiza a posição do player
void Model::updatePlayer(float delta_t, Camera &camera, const Model& box) {

    // Velocidade do personagem
    float speed = 4.0f;

    glm::vec3 newPosition = this->position;

    // Cálculo de rotação a partir da direção original do player
    this->rotation = atan2f(this->direction.z, this->direction.x) - atan2f(camera.getViewVector().z, camera.getViewVector().x);

    // Cálculo de posição a partir do input do usuário

    glm::vec4 w = -(normalize(camera.getViewVector()));
    glm::vec4 u = normalize(crossproduct(Camera::upVector, w));

    if (camera.keys.W){
        newPosition -= glm::vec3(w.x, 0.0f, w.z) * speed * delta_t;
    }
    if (camera.keys.S){
        newPosition += glm::vec3(w.x, 0.0f, w.z) * speed * delta_t;
    }
    if (camera.keys.A){
        newPosition -= glm::vec3(u.x, 0.0f, u.z) * speed * delta_t;
    }
    if (camera.keys.D){
        newPosition += glm::vec3(u.x, 0.0f, u.z) * speed * delta_t;
    }

    // Atualização da bounding box
    glm::vec3 newBbox_min = glm::vec3(newPosition.x - this->x_difference, newPosition.y, newPosition.z - this->z_difference);
    glm::vec3 newBbox_max = glm::vec3(newPosition.x + this->x_difference, newPosition.y, newPosition.z + this->z_difference);

    // Checa colisão com o cenário
    if (!collisions::CubeToBox(newBbox_min, newBbox_max, box.bbox_min, box.bbox_max)) {
        // Caso não ocorre, atualiza a posição do personagem
        this->position = newPosition;
        camera.updateCartesianCoordinates(glm::vec4(newPosition.x, newPosition.y + 0.7f, newPosition.z, 1.0f));
    }

    // Caso em câmera look-at, atualiza o ponto de Look-At
    if (!camera.isUseFreeCamera()) {
        glm::vec3 pos = this->position;
        camera.setLookAt(glm::vec4(pos.x, pos.y + 0.7f, pos.z, 1.0f));
    }

}

// Atualiza a posição do bumerange
bool Model::updateBoomerang(bool &boomerangIsThrown,
                            bool &primaryAttackStarts,
                            bool &secondaryAttackStarts,
                            bool &M1,
                            bool &M2,
                            bool &isPaused,
                            float &rotationBoomerang,
                            float &t,
                            float &delta_t,
                            glm::vec3 &robotPosition,
                            float robotRotation,
                            glm::vec3 &sceneryBboxMin,
                            glm::vec3 &sceneryBboxMax,
                            glm::mat4 &model){

    // Velocidade do bumerange
    float boomerangSpeed = 6.0f;

    // Caso o ataque foi inicializado agora
    if (boomerangIsThrown && !primaryAttackStarts && !secondaryAttackStarts) {

        // Atualiza posição, posição original e bounding box a posição do player
        this->setPosition(glm::vec3(robotPosition.x, robotPosition.y, robotPosition.z));
        this->updateOriginalPosition();
        this->updateBbox();

        // Calcula a direção de ataque
        glm::vec3 attackDirection = glm::vec3(robotPosition.x, robotPosition.y, robotPosition.z + 1.0f) - robotPosition;
        this->setDirection(normalize(glm::vec3(  attackDirection.x * cos(robotRotation) + attackDirection.z * sin(robotRotation),
                                                  this->getPosition().y,
                                                  -attackDirection.x * sin(robotRotation) + attackDirection.z * cos(robotRotation))));

        // Determina qual ataque foi realizado
        if (M1) {
            primaryAttackStarts = true;
        }
        if (M2) {
            secondaryAttackStarts = true;
        }
    }
    // Caso o primeiro ataque foi realizado
    if (primaryAttackStarts) {

        // Desliga a inicialização do ataque
        boomerangIsThrown = false;

        // Alcance do ataque
        float attackRange = 5.0f;

        // Caso a distãncia entre a posição atual e a posição original seja menor ou igual que o alcance do ataque
        if (glm::distance(glm::vec2(this->getPosition().x, this->getPosition().z), glm::vec2(this->getOriginalPosition().x, this->getOriginalPosition().z)) <= attackRange) {

            // Caso não esteja pausado, atualiza posição e rotação
            if (!isPaused) {
                this->setPosition(this->getPosition() + this->getDirection() * delta_t * boomerangSpeed);
                rotationBoomerang += 0.1f;
            }

            // Caso não colida com o cenário
            if (!collisions::CubeToBox(this->bbox_min, this->bbox_max, sceneryBboxMin, sceneryBboxMax)) {

                // A matriz de modelo do bumerange é atualizada
                model = Matrix_Translate(this->getPosition().x, this->getPosition().y, this->getPosition().z);
                model *= Matrix_Scale(this->getScale().x, this->getScale().y, this->getScale().z);
                model *= Matrix_Rotate_X(this->getRotation());
                model *= Matrix_Rotate_Z(rotationBoomerang);

                // A bounding box é atualizada
                this->updateBbox();

                // Confirma a renderização do bumerange
                return true;
            }
        }
        // Ao final do ataque
        else {
            // Reseta a rotação do ataque e desliga o booleano de ataque
            rotationBoomerang = 0.0f;
            primaryAttackStarts = false;

            // Não renderiza
            return false;
        }
    }
    // Caso o segundo ataque foi realizado
    if (secondaryAttackStarts) {

        // Desliga a inicialização do ataque
        boomerangIsThrown = false;

        // Alcance do ataque
        float attackRange = 7.5f;

        // Caso a distãncia entre a posição atual e a posição original seja menor ou igual que o alcance do ataque
        if (glm::distance(glm::vec2(this->getPosition().x, this->getPosition().z), glm::vec2(this->getOriginalPosition().x, this->getOriginalPosition().z)) <= attackRange) {

            // Caso não esteja pausado, calcula curva de bézier de grau 2 e atualiza posição e rotação
            if (!isPaused) {

                // Cálculo da posição do ponto intermediário
                float distanceToIntermediate = attackRange * 0.7f;
                float angleToIntermediate = M_PI_2;

                // Cálculo de Curva de Bézier
                glm::vec3 p2;
                p2.x = this->getOriginalPosition().x + distanceToIntermediate * (cos(angleToIntermediate) * this->getDirection().x - sin(angleToIntermediate) * this->getDirection().z);
                p2.y = this->getPosition().y;
                p2.z = this->getOriginalPosition().z + distanceToIntermediate * (sin(angleToIntermediate) * this->getDirection().x + cos(angleToIntermediate) * this->getDirection().z);

                glm::vec3 p3 = this->getOriginalPosition() + attackRange * this->getDirection();

                glm::vec3 c12 = this->getOriginalPosition() + t * (p2 - this->getOriginalPosition());
                glm::vec3 c23 = p2 + t * (p3 - p2);
                glm::vec3 c = c12 + t * (c23 - c12);

                // Atualiza valores de posição e rotação
                this->setPosition(c);
                rotationBoomerang += 0.1f;
                t += 0.5f * delta_t * boomerangSpeed;
            }

            // Caso não colida com o cenário
            if (!collisions::CubeToBox(this->bbox_min, this->bbox_max, sceneryBboxMin, sceneryBboxMax)) {

                // A matriz de modelo do bumerange é atualizada
                model = Matrix_Translate(this->getPosition().x, this->getPosition().y, this->getPosition().z);
                model *= Matrix_Scale(this->getScale().x, this->getScale().y, this->getScale().z);
                model *= Matrix_Rotate_X(this->getRotation());
                model *= Matrix_Rotate_Z(rotationBoomerang);

                // A bounding box é atualizada
                this->updateBbox();

                // Confirma a renderização do bumerange
                return true;
            }
        }
        // Ao final do ataque
        else {
            // Reseta a rotação do ataque e desliga o booleano de ataque
            rotationBoomerang = 0.0f;
            t = 0.0f;
            secondaryAttackStarts = false;

            // Não renderiza
            return false;
        }
    }
}
