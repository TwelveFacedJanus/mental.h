#version 330 core
out vec4 FragColor;

in vec3 WorldPos;

uniform float time;

// Улучшенная реализация шума Перлина
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    
    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);
}

float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    
    for (int i = 0; i < 6; i++) {
        value += amplitude * noise(p * frequency);
        amplitude *= 0.5;
        frequency *= 2.0;
    }
    
    return value;
}

// Функция для создания гор
float mountainNoise(vec2 p) {
    // Основной шум для гор
    float mountain = fbm(p * 0.3);
    
    // Среднечастотный шум для холмов
    float hills = fbm(p * 1.0) * 0.5;
    
    // Высокочастотный шум для деталей
    float detail = fbm(p * 3.0) * 0.25;
    
    // Комбинируем шумы для создания реалистичных гор
    return mountain * 0.6 + hills * 0.3 + detail * 0.1;
}

// Функция для определения высоты в данной точке
float getHeight(vec2 pos) {
    return mountainNoise(pos) * 3.0; // Увеличиваем масштаб высоты
}

// Функция для определения цвета на основе высоты
vec3 getColorByHeight(float height) {
    // Очень низкие участки - темная земля
    if (height < 0.2) {
        return mix(vec3(0.3, 0.2, 0.1), vec3(0.545, 0.271, 0.075), height / 0.2);
    }
    // Низкие участки - земля
    else if (height < 0.4) {
        return mix(vec3(0.545, 0.271, 0.075), vec3(0.6, 0.4, 0.2), (height - 0.2) / 0.2);
    }
    // Средние участки - холмы
    else if (height < 0.7) {
        return mix(vec3(0.6, 0.4, 0.2), vec3(0.5, 0.5, 0.3), (height - 0.4) / 0.3);
    }
    // Высокие участки - скалы
    else if (height < 0.9) {
        return mix(vec3(0.5, 0.5, 0.3), vec3(0.6, 0.6, 0.6), (height - 0.7) / 0.2);
    }
    // Очень высокие участки - снежные вершины
    else {
        return mix(vec3(0.6, 0.6, 0.6), vec3(0.95, 0.95, 0.95), (height - 0.9) / 0.1);
    }
}

void main()
{
    // Используем реальную высоту из геометрии
    float height = (WorldPos.y + 1.0) / 5.0; // Нормализуем высоту к [0, 1] с учетом большей высоты
    height = clamp(height, 0.0, 1.0);
    
    // Используем мировую позицию для дополнительных деталей
    vec2 terrainPos = WorldPos.xz * 0.05;
    
    // Определяем цвет на основе высоты
    vec3 terrainColor = getColorByHeight(height);
    
    // Добавляем вариации для более реалистичного вида
    float variation = noise(terrainPos * 20.0) * 0.05;
    terrainColor += vec3(variation);
    
    // Добавляем тени для большей глубины
    float shadow = 1.0 - height * 0.3;
    terrainColor *= shadow;
    
    // Ограничиваем цвет
    terrainColor = clamp(terrainColor, 0.0, 1.0);
    
    FragColor = vec4(terrainColor, 1.0);
}
