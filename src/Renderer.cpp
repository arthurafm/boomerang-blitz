//
// Created by Arthur on 8/12/2023.
//

#include "Renderer.h"
#include "collisions.h"

#define SCENERY 0
#define ROBOT 1
#define ZOMBIE 2
#define BOOMERANG 3

// Construtor do renderizador
Renderer::Renderer() {
    this->gpuProgramID = 0;
    this->numLoadedTextures = 0;
}

void Renderer::initialize() {
    // Habilitamos o Z-buffer.
    glEnable(GL_DEPTH_TEST);

    // Habilitamos o Backface Culling.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

// Função que carrega os shaders de vértices e de fragmentos que serão utilizados para renderização.
void Renderer::LoadShadersFromFiles()
{
    GLuint vertex_shader_id = LoadShader_Vertex("../src/shaders/shader_vertex.glsl");
    GLuint fragment_shader_id = LoadShader_Fragment("../src/shaders/shader_fragment.glsl");

    // Deletamos o programa de GPU anterior, caso ele exista.
    if (this->gpuProgramID != 0)
        glDeleteProgram(this->gpuProgramID);

    // Criamos um programa de GPU utilizando os shaders carregados acima.
    this->gpuProgramID = CreateGpuProgram(vertex_shader_id, fragment_shader_id);

    // Buscamos o endereço das variáveis definidas dentro do Vertex Shader.
    // Utilizaremos estas variáveis para enviar dados para a placa de vídeo.
    this->model_uniform      = glGetUniformLocation(this->gpuProgramID, "model"); // Variável da matriz "model"
    this->view_uniform       = glGetUniformLocation(this->gpuProgramID, "view"); // Variável da matriz "view" em shader_vertex.glsl
    this->projection_uniform = glGetUniformLocation(this->gpuProgramID, "projection"); // Variável da matriz "projection" em shader_vertex.glsl
    this->object_id_uniform  = glGetUniformLocation(this->gpuProgramID, "object_id"); // Variável "object_id" em shader_fragment.glsl
    this->bbox_min_uniform   = glGetUniformLocation(this->gpuProgramID, "bbox_min");
    this->bbox_max_uniform   = glGetUniformLocation(this->gpuProgramID, "bbox_max");

    // Variáveis em "shader_fragment.glsl" para acesso das imagens de textura
    glUseProgram(this->gpuProgramID);
    glUniform1i(glGetUniformLocation(this->gpuProgramID, "PlaneTexture"), 0);
    glUniform1i(glGetUniformLocation(this->gpuProgramID, "RobotTexture"), 1);
    glUniform1i(glGetUniformLocation(this->gpuProgramID, "ZombieTexture"), 2);
    glUniform1i(glGetUniformLocation(this->gpuProgramID, "BoomerangTexture"), 3);
    glUseProgram(0);
}

// Carrega um Vertex Shader de um arquivo GLSL.
GLuint Renderer::LoadShader_Vertex(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo será aplicado nos vértices.
    GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);

    // Carregamos e compilamos o shader
    this->LoadShader(filename, vertex_shader_id);

    // Retorna o ID gerado acima
    return vertex_shader_id;
}

// Carrega um Fragment Shader de um arquivo GLSL.
GLuint Renderer::LoadShader_Fragment(const char* filename)
{
    // Criamos um identificador (ID) para este shader, informando que o mesmo será aplicado nos fragmentos.
    GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

    // Carregamos e compilamos o shader
    this->LoadShader(filename, fragment_shader_id);

    // Retorna o ID gerado acima
    return fragment_shader_id;
}

// Função auxilar. Carrega código de GPU de um arquivo GLSL e faz sua compilação.
void Renderer::LoadShader(const char* filename, GLuint shader_id)
{
    // Lê "filename" e colocamos seu conteúdo em memória, apontado por "shader_string"
    std::ifstream file;
    try {
        file.exceptions(std::ifstream::failbit);
        file.open(filename);
    } catch ( std::exception& e ) {
        fprintf(stderr, "ERROR: Cannot open file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }
    std::stringstream shader;
    shader << file.rdbuf();
    std::string str = shader.str();
    const GLchar* shader_string = str.c_str();
    const GLint   shader_string_length = static_cast<GLint>( str.length() );

    // Define o código do shader GLSL, contido na string "shader_string"
    glShaderSource(shader_id, 1, &shader_string, &shader_string_length);

    // Compila o código do shader GLSL (em tempo de execução)
    glCompileShader(shader_id);

    // Verificamos se ocorreu algum erro ou "warning" durante a compilação
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);

    GLint log_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_length);

    GLchar* log = new GLchar[log_length];
    glGetShaderInfoLog(shader_id, log_length, &log_length, log);

    if ( log_length != 0 )
    {
        std::string  output;

        if ( !compiled_ok )
        {
            output += "ERROR: OpenGL compilation of \"";
            output += filename;
            output += "\" failed.\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }
        else
        {
            output += "WARNING: OpenGL compilation of \"";
            output += filename;
            output += "\".\n";
            output += "== Start of compilation log\n";
            output += log;
            output += "== End of compilation log\n";
        }

        fprintf(stderr, "%s", output.c_str());
    }

    delete [] log;
}

// Função que carrega uma imagem para ser utilizada como textura
void Renderer::LoadTextureImage(const char* filename)
{
    printf("Carregando imagem \"%s\"... ", filename);

    // Primeiro fazemos a leitura da imagem do disco
    stbi_set_flip_vertically_on_load(true);
    int width;
    int height;
    int channels;
    unsigned char *data = stbi_load(filename, &width, &height, &channels, 3);

    if ( data == NULL )
    {
        fprintf(stderr, "ERROR: Cannot open image file \"%s\".\n", filename);
        std::exit(EXIT_FAILURE);
    }

    printf("OK (%dx%d).\n", width, height);

    // Agora criamos objetos na GPU com OpenGL para armazenar a textura
    GLuint texture_id;
    GLuint sampler_id;
    glGenTextures(1, &texture_id);
    glGenSamplers(1, &sampler_id);

    // Veja slides 95-96 do documento Aula_20_Mapeamento_de_Texturas.pdf
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Parâmetros de amostragem da textura.
    glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Agora enviamos a imagem lida do disco para a GPU
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    GLuint textureunit = this->numLoadedTextures;
    glActiveTexture(GL_TEXTURE0 + textureunit);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindSampler(textureunit, sampler_id);

    stbi_image_free(data);

    this->numLoadedTextures += 1;
}

// Esta função cria um programa de GPU, o qual contém obrigatoriamente um Vertex Shader e um Fragment Shader.
GLuint Renderer::CreateGpuProgram(GLuint vertex_shader_id, GLuint fragment_shader_id)
{
    // Criamos um identificador (ID) para este programa de GPU
    GLuint program_id = glCreateProgram();

    // Definição dos dois shaders GLSL que devem ser executados pelo programa
    glAttachShader(program_id, vertex_shader_id);
    glAttachShader(program_id, fragment_shader_id);

    // Linkagem dos shaders acima ao programa
    glLinkProgram(program_id);

    // Verificamos se ocorreu algum erro durante a linkagem
    GLint linked_ok = GL_FALSE;
    glGetProgramiv(program_id, GL_LINK_STATUS, &linked_ok);

    // Imprime no terminal qualquer erro de linkagem
    if ( linked_ok == GL_FALSE )
    {
        GLint log_length = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &log_length);

        // Alocamos memória para guardar o log de compilação.
        // A chamada "new" em C++ é equivalente ao "malloc()" do C.
        GLchar* log = new GLchar[log_length];

        glGetProgramInfoLog(program_id, log_length, &log_length, log);

        std::string output;

        output += "ERROR: OpenGL linking of program failed.\n";
        output += "== Start of link log\n";
        output += log;
        output += "\n== End of link log\n";

        // A chamada "delete" em C++ é equivalente ao "free()" do C
        delete [] log;

        fprintf(stderr, "%s", output.c_str());
    }

    // Os "Shader Objects" podem ser marcados para deleção após serem linkados
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    // Retornamos o ID gerado acima
    return program_id;
}

// Função que desenha um objeto armazenado em virtualScene.
void Renderer::DrawVirtualObject(const char* object_name)
{
    // "Ligamos" o VAO.
    glBindVertexArray(this->virtualScene[object_name].vertex_array_object_id);

    // Setamos as variáveis "bbox_min" e "bbox_max" do fragment shader
    // com os parâmetros da axis-aligned bounding box (AABB) do modelo.
    glm::vec3 bbox_min = this->virtualScene[object_name].bbox_min;
    glm::vec3 bbox_max = this->virtualScene[object_name].bbox_max;
    glUniform4f(this->bbox_min_uniform, bbox_min.x, bbox_min.y, bbox_min.z, 1.0f);
    glUniform4f(this->bbox_max_uniform, bbox_max.x, bbox_max.y, bbox_max.z, 1.0f);

    // GPU rasteriza os vértices dos eixos XYZ apontados pelo VAO como linhas.
    glDrawElements(
            this->virtualScene[object_name].rendering_mode,
            this->virtualScene[object_name].num_indices,
            GL_UNSIGNED_INT,
            (void*)(this->virtualScene[object_name].first_index * sizeof(GLuint))
    );

    // "Desligamos" o VAO.
    glBindVertexArray(0);
}

bool boomerangIsThrown, secondaryAttackStarts, primaryAttackStarts = false;
float rotationBoomerang = 0.0f;
float t = 0.0f;

void Renderer::render(GLFWwindow* window, Camera &camera, const float &aspectRatio, float &initialTime) {
    // Define a cor de "fundo" do framebuffer como branco.
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // "Pintamos" todos os pixels do framebuffer com a cor definida acima,
    // e também resetamos todos os pixels do Z-buffer (depth buffer).
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Pedimos para a GPU utilizar o programa de GPU criado acima (contendo
    // os shaders de vértice e fragmentos).
    glUseProgram(this->gpuProgramID);

    // Enviamos as matrizes "view" e "projection" para a placa de vídeo.
    glUniformMatrix4fv(this->view_uniform       , 1 , GL_FALSE , glm::value_ptr(camera.getView()));
    glUniformMatrix4fv(this->projection_uniform , 1 , GL_FALSE , glm::value_ptr(camera.getPerspective(aspectRatio)));

    // Variáveis ligadas a movimentação baseada em tempo
    float currentTime = glfwGetTime();
    float delta_t = currentTime - initialTime;
    initialTime = currentTime;

    camera.updateCamera(delta_t);

    if (camera.keys.M1 || camera.keys.M2) {
        boomerangIsThrown = true;
    }

    glm::mat4 model = Matrix_Identity();

    for (Model &object : this->models) {

        // Se é o bumerange
        if (object.getId() == BOOMERANG) {

            if (boomerangIsThrown && !primaryAttackStarts && !secondaryAttackStarts) {
                object.setPosition(glm::vec3(this->models[ROBOT].getPosition().x, object.getPosition().y, this->models[ROBOT].getPosition().z));
                object.updateOriginalPosition();
                glm::vec3 attackDirection = glm::vec3(this->models[ROBOT].getPosition().x, this->models[ROBOT].getPosition().y, this->models[ROBOT].getPosition().z + 1.0f) - this->models[ROBOT].getPosition();
                object.setDirection(normalize(glm::vec3(  attackDirection.x * cos(this->models[ROBOT].getRotation()) + attackDirection.z * sin(this->models[ROBOT].getRotation()),
                                                object.getPosition().y,
                                                -attackDirection.x * sin(this->models[ROBOT].getRotation()) + attackDirection.z * cos(this->models[ROBOT].getRotation()))));
                if (camera.keys.M1) {
                    primaryAttackStarts = true;
                }
                if (camera.keys.M2) {
                    secondaryAttackStarts = true;
                }
            }
            if (primaryAttackStarts) {
                boomerangIsThrown = false;
                float attackRange = 1.0f;
                if (glm::distance(glm::vec2(object.getPosition().x, object.getPosition().z), glm::vec2(object.getOriginalPosition().x, object.getOriginalPosition().z)) <= attackRange) {
                    object.setPosition(object.getPosition() + object.getDirection() * delta_t);

                    model = Matrix_Translate(object.getPosition().x, object.getPosition().y, object.getPosition().z);
                    model *= Matrix_Scale(object.getScale().x, object.getScale().y, object.getScale().z);
                    model *= Matrix_Rotate_X(object.getRotation());
                    model *= Matrix_Rotate_Z(rotationBoomerang);
                    rotationBoomerang += 0.1f;

                    object.updateBbox();

                    glUniformMatrix4fv(this->model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                    glUniform1i(this->object_id_uniform, object.getId());
                    this->DrawVirtualObject(object.getName().c_str());
                }
                else {
                    rotationBoomerang = 0.0f;
                    primaryAttackStarts = false;
                }
            }
            if (secondaryAttackStarts) {
                boomerangIsThrown = false;
                float attackRange = 1.5f;
                if (glm::distance(glm::vec2(object.getPosition().x, object.getPosition().z), glm::vec2(object.getOriginalPosition().x, object.getOriginalPosition().z)) <= attackRange) {
                    float distanceToIntermediate = attackRange * 0.7f;
                    float angleToIntermediate = M_PI_2;

                    glm::vec3 p2;
                    p2.x = object.getOriginalPosition().x + distanceToIntermediate * (cos(angleToIntermediate) * object.getDirection().x - sin(angleToIntermediate) * object.getDirection().z);
                    p2.y = object.getPosition().y;
                    p2.z = object.getOriginalPosition().z + distanceToIntermediate * (sin(angleToIntermediate) * object.getDirection().x + cos(angleToIntermediate) * object.getDirection().z);

                    glm::vec3 p3 = object.getOriginalPosition() + attackRange * object.getDirection();

                    glm::vec3 c12 = object.getOriginalPosition() + t * (p2 - object.getOriginalPosition());
                    glm::vec3 c23 = p2 + t * (p3 - p2);
                    glm::vec3 c = c12 + t * (c23 - c12);

                    object.setPosition(c);
                    model = Matrix_Translate(object.getPosition().x, object.getPosition().y, object.getPosition().z);
                    model *= Matrix_Scale(object.getScale().x, object.getScale().y, object.getScale().z);
                    model *= Matrix_Rotate_X(object.getRotation());
                    model *= Matrix_Rotate_Z(rotationBoomerang);
                    rotationBoomerang += 0.1f;
                    t += 0.5f * delta_t;

                    object.updateBbox();

                    glUniformMatrix4fv(this->model_uniform, 1, GL_FALSE, glm::value_ptr(model));
                    glUniform1i(this->object_id_uniform, object.getId());
                    this->DrawVirtualObject(object.getName().c_str());
                }
                else {
                    rotationBoomerang = 0.0f;
                    t = 0.0f;
                    secondaryAttackStarts = false;
                }
            }
        }
        else {
            model = Matrix_Translate(object.getPosition().x, object.getPosition().y, object.getPosition().z);
            model *= Matrix_Scale(object.getScale().x, object.getScale().y, object.getScale().z);

            object.updateBbox();

            model *= Matrix_Rotate_Y(object.getRotation());

            // Se é o robô
            if (object.getId() == ROBOT) {
                object.updatePlayer(delta_t, camera, this->models[SCENERY]);
            }
            if (object.getId() == ZOMBIE) {
                if (collisions::CylinderToCylinder(object.bbox_min, object.bbox_max, this->models[ROBOT].bbox_min, this->models[ROBOT].bbox_max)) {
//                    printf("\n\nColidiu com robô!\n\n");
                }
                if (collisions::CubeToCylinder(object.bbox_min, object.bbox_max, this->models[BOOMERANG].bbox_min, this->models[BOOMERANG].bbox_max)
                    && (primaryAttackStarts || secondaryAttackStarts)) {
                    printf("\n\nColidiu com bumerange!\n\n");
                }
            }

            glUniformMatrix4fv(this->model_uniform, 1, GL_FALSE, glm::value_ptr(model));
            glUniform1i(this->object_id_uniform, object.getId());
            this->DrawVirtualObject(object.getName().c_str());
        }
    }

    glfwSwapBuffers(window);

    // Verifica interrupção
    glfwPollEvents();
}