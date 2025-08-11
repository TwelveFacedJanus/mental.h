#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

out vec3 WorldPos;

void main()
{
    // Передаем мировую позицию в фрагментный шейдер
    WorldPos = aPos;
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
