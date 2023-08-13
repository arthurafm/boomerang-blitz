#version 330 core

// Atributos de vértice recebidos como entrada ("in") pelo Vertex Shader.
layout (location = 0) in vec4 model_coefficients;
layout (location = 1) in vec4 normal_coefficients;
layout (location = 2) in vec2 texture_coefficients;

// Matrizes computadas no código C++ e enviadas para a GPU
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Atributos de vértice que serão gerados como saída ("out") pelo Vertex Shader. Interpolados pelo rasterizador
out vec4 position_world;
out vec4 normal;

void main()
{
    // A variável gl_Position define a posição final de cada vértice em NDC.

    gl_Position = projection * view * model * model_coefficients;

    position_world = model * model_coefficients;

    // Normal do vértice atual no sistema de coordenadas global (World).
    normal = inverse(transpose(model)) * normal_coefficients;
    normal.w = 0.0;
}

