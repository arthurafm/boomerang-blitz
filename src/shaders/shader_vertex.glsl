#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec2 texture_coefficients;

// Textura do zumbi
uniform sampler2D ZombieTexture;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Identificador que define qual objeto está sendo desenhado no momento
#define SCENE 0
#define ROBOT 1
#define ZOMBIE 2
#define BOOMERANG 3
uniform int object_id;

// Atributos de vértice que serão gerados como saída ("out") pelo Vertex Shader.
out vec4 position_world;
out vec4 position_model;
out vec4 normal;
out vec2 texcoords;
out vec4 color_v;

void main()
{
    // Define a posição final de cada vértice em NDC.
    gl_Position = projection * view * model * model_coefficients;

    // Posição do vértice atual no sistema de coordenadas global (World).
    position_world = model * model_coefficients;

    // Posição do vértice atual no sistema de coordenadas local do modelo.
    position_model = model_coefficients;

    // Normal do vértice atual no sistema de coordenadas global (World).
    normal = inverse(transpose(model)) * normal_coefficients;
    normal.w = 0.0;

    // Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
    texcoords = texture_coefficients;

    // Gouraud shading
    if (object_id == ZOMBIE) {
        // Obtemos a posição da câmera utilizando a inversa da matriz que define a sistema de coordenadas da câmera.
        vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
        vec4 camera_position = inverse(view) * origin;

        vec4 p = position_world;

        // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
        vec4 l = normalize(vec4(0.0,3.5,0.0,0.0));
        // Normal do fragmento atual
        vec4 n = normalize(normal);

        // Vetor que define o sentido da câmera em relação ao ponto atual.
        vec4 v = normalize(camera_position - p);

        // Coordenadas de textura U e V
        float U = 0.0;
        float V = 0.0;

        // Vetor que define o sentido da reflexão especular ideal.
        vec4 r = -l + 2 * n * dot(n, l);

        // Parâmetros que definem as propriedades espectrais da superfície
        vec3 Kd; // Refletância difusa
        vec3 Ks; // Refletância especular
        vec3 Ka; // Refletância ambiente
        float q; // Expoente especular para o modelo de iluminação de Phong

        Kd = texture(ZombieTexture, texcoords).rgb;
        Ks = vec3(0.8,0.8,0.8);
        Ka = Kd / 2.0;
        q = 32.0;

        // Espectro da fonte de iluminação
        vec3 I = vec3(0.4,0.4,0.4);

        // Espectro da luz ambiente
        vec3 Ia = vec3(0.2,0.2,0.2);

        // Termo difuso utilizando a lei dos cossenos de Lambert
        vec3 lambert_diffuse_term = Kd * I * max(0, dot(n, l));

        // Termo ambiente
        vec3 ambient_term = Ka * Ia;

        // Termo especular utilizando o modelo de iluminação de Phong
        vec3 phong_specular_term = Ks * I * pow(max(0, dot(r, v)), q);

        color_v.a = 1;

        // Cor final do fragmento calculada com uma combinação dos termos difuso, especular, e ambiente.
        color_v.rgb = lambert_diffuse_term + ambient_term + phong_specular_term;

        // Cor final com correção gamma, considerando monitor sRGB.
        color_v.rgb = pow(color_v.rgb, vec3(1.0,1.0,1.0)/2.2);
    }
}

