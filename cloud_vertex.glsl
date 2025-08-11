#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

out vec3 WorldPos;
out vec3 LocalPos;

// Простая хеш-функция для генерации псевдослучайных чисел
float hash(vec3 p) {
    p = fract(p * vec3(443.8975, 397.2973, 491.1871));
    p += dot(p, p.zyx + 19.19);
    return fract(p.x * p.y * p.z);
}

// Функция шума для плавных переходов
float noise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    f = f * f * (3.0 - 2.0 * f); // Сглаживание
    
    float n = mix(
        mix(
            mix(hash(i), hash(i + vec3(1.0, 0.0, 0.0)), f.x),
            mix(hash(i + vec3(0.0, 1.0, 0.0)), hash(i + vec3(1.0, 1.0, 0.0)), f.x),
            f.y
        ),
        mix(
            mix(hash(i + vec3(0.0, 0.0, 1.0)), hash(i + vec3(1.0, 0.0, 1.0)), f.x),
            mix(hash(i + vec3(0.0, 1.0, 1.0)), hash(i + vec3(1.0, 1.0, 1.0)), f.x),
            f.y
        ),
        f.z
    );
    
    return n;
}

void main()
{
    // Базовая позиция вершины
    vec3 position = aPos;
    
    // Добавляем небольшую анимацию колыхания облаков
    // Используем позицию вершины для создания уникальной анимации для каждой вершины
    float noiseValue = noise(position * 0.5 + vec3(time * 0.1));
    
    // Амплитуда колыхания зависит от высоты вершины (верхние части колышутся сильнее)
    float amplitude = 0.05 * (1.0 + position.y * 0.2);
    
    // Применяем колыхание с разной частотой по разным осям
    position.x += amplitude * sin(time * 0.3 + position.y * 2.0 + position.z) * noiseValue;
    position.y += amplitude * 0.7 * sin(time * 0.2 + position.x * 1.5) * noiseValue;
    position.z += amplitude * cos(time * 0.4 + position.y * 1.8) * noiseValue;
    
    // Передаем мировую и локальную позиции в фрагментный шейдер
    WorldPos = (model * vec4(position, 1.0)).xyz;
    LocalPos = position;
    
    gl_Position = projection * view * model * vec4(position, 1.0);
}
