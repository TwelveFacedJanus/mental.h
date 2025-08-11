#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;  // Время для анимации облаков

out vec2 TexCoord;
out vec3 WorldPos;

void main()
{
    WorldPos = vec3(model * vec4(aPos, 1.0));
    TexCoord = aPos.xy + 0.5;  // Нормализованные координаты текстуры
    
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}