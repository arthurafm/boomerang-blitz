#version 330 core

in vec4 position_world;
in vec4 normal;

// Posição do vértice atual no sistema de coordenadas local do modelo.
in vec4 position_model;

// Coordenadas de textura obtidas do arquivo OBJ (se existirem!)
in vec2 texcoords;

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

// Parâmetros da axis-aligned bounding box (AABB) do modelo
uniform vec4 bbox_min;
uniform vec4 bbox_max;

// Variáveis para acesso das imagens de textura
uniform sampler2D PlaneTexture;
uniform sampler2D RobotTexture;
uniform sampler2D BoomerangTexture;

// Cor gerada por Gouraud
in vec4 color_v;

// Cor final do fragmento.
out vec4 color;

void main()
{
    // Obtemos a posição da câmera utilizando a inversa da matriz que define a sistema de coordenadas da câmera.
    vec4 origin = vec4(0.0, 0.0, 0.0, 1.0);
    vec4 camera_position = inverse(view) * origin;

    vec4 p = position_world;

    // Normal do fragmento atual
    vec4 n = normalize(normal);

    // Vetor que define o sentido da fonte de luz em relação ao ponto atual.
    vec4 l = normalize(vec4(0.0,3.5,0.0,0.0));

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

    // Phong shading
    if (object_id != ZOMBIE) {
        if ( object_id == SCENE )
        {
            // Propriedades espectrais do cenário
            Kd = texture(PlaneTexture, texcoords).rgb;
            Ks = vec3(0.1,0.1,0.1);
            Ka = vec3(0.0,0.0,0.0);
            q = 50.0;
        }
        else if ( object_id == ROBOT )
        {
            // Propriedades espectrais do zumbi
            Kd = texture(RobotTexture, texcoords).rgb;
            Ks = vec3(0.8,0.8,0.8);
            Ka = Kd / 2.0;
            q = 32.0;
        }
        else if ( object_id == BOOMERANG ) {
            // Propriedades espectrais da esfera
            Kd = texture(BoomerangTexture, texcoords).rgb;
            Ks = vec3(0.8,0.8,0.8);
            Ka = Kd / 2.0;
            q = 32.0;
        }
        else // Objeto desconhecido = preto
        {
            Kd = vec3(0.0,0.0,0.0);
            Ks = vec3(0.0,0.0,0.0);
            Ka = vec3(0.0,0.0,0.0);
            q = 1.0;
        }
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

        color.a = 1;

        // Cor final do fragmento calculada com uma combinação dos termos difuso, especular, e ambiente.
        color.rgb = lambert_diffuse_term + ambient_term + phong_specular_term;

        // Cor final com correção gamma, considerando monitor sRGB.
        color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
    }
    else {
        color = color_v;
    }
} 

