#version 330 core

in vec4 position_world;
in vec4 normal;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int object_id;

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
    vec4 l = normalize(vec4(1.0,1.0,0.5,0.0));

    // Vetor que define o sentido da câmera em relação ao ponto atual.
    vec4 v = normalize(camera_position - p);

    // Vetor que define o sentido da reflexão especular ideal.
    vec4 r = -l + 2 * n * dot(n, l);

    // Parâmetros que definem as propriedades espectrais da superfície
    vec3 Kd; // Refletância difusa
    vec3 Ks; // Refletância especular
    vec3 Ka; // Refletância ambiente
    float q; // Expoente especular para o modelo de iluminação de Phong

    // Propriedades espectrais do plano
    Kd = vec3(0.2,0.2,0.2);
    Ks = vec3(0.3,0.3,0.3);
    Ka = vec3(0.0,0.0,0.0);
    q = 20.0;

    // Espectro da fonte de iluminação
    vec3 I = vec3(1.0,1.0,1.0);

    // Espectro da luz ambiente
    vec3 Ia = vec3(0.2,0.2,0.2);

    // Termo difuso utilizando a lei dos cossenos de Lambert
    vec3 lambert_diffuse_term = Kd * I * max(0, dot(n, l));

    // Termo ambiente
    vec3 ambient_term = Ka * Ia;

    // Termo especular utilizando o modelo de iluminação de Phong
    vec3 phong_specular_term = Ks * I * pow(max(0, dot(r, v)), q);

    // Alpha default = 1 = 100% opaco = 0% transparente
    color.a = 1;

    // Cor final do fragmento calculada com uma combinação dos termos difuso, especular, e ambiente.
    color.rgb = lambert_diffuse_term + ambient_term + phong_specular_term;

    // Cor final com correção gamma.
    color.rgb = pow(color.rgb, vec3(1.0,1.0,1.0)/2.2);
} 

