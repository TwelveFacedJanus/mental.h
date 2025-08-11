#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 WorldPos;

uniform vec2 resolution;
uniform float time;
uniform float size;

// Улучшенный шум Перлина
float hash(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    
    float a = hash(i);
    float b = hash(i + vec2(1.0, 0.0));
    float c = hash(i + vec2(0.0, 1.0));
    float d = hash(i + vec2(1.0, 1.0));
    
    vec2 u = f * f * (3.0 - 2.0 * f);
    
    return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}

// Фрактальный шум для объемных облаков
float fbm(vec2 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    
    for(int i = 0; i < 6; i++) {
        value += amplitude * noise(p * frequency);
        amplitude *= 0.5;
        frequency *= 2.0;
    }
    
    return value;
}

// Объемные облака с несколькими слоями
float volumetricClouds(vec2 uv) {
    // Анимация облаков с разными скоростями
    vec2 movement1 = vec2(time * 0.05, time * 0.02);
    vec2 movement2 = vec2(time * 0.08, time * 0.03);
    vec2 movement3 = vec2(time * 0.12, time * 0.01);
    
    // Основной слой облаков
    float base_clouds = fbm(uv * 1.5 + movement1);
    
    // Детализированный слой
    float detail_clouds = fbm(uv * 3.0 + movement2);
    
    // Высокочастотные детали
    float high_freq = fbm(uv * 6.0 + movement3);
    
    // Комбинируем слои
    float clouds = base_clouds * 0.6 + detail_clouds * 0.3 + high_freq * 0.1;
    
    // Добавляем вариации по высоте
    float height_gradient = smoothstep(0.0, 0.4, uv.y) * smoothstep(1.0, 0.6, uv.y);
    clouds *= height_gradient;
    
    // Добавляем горизонтальные вариации
    float horizontal_variation = 1.0 + 0.3 * sin(uv.x * 10.0 + time * 0.5);
    clouds *= horizontal_variation;
    
    return clouds;
}

// Создание неба с градиентом
vec3 createSky(vec2 uv) {
    // Градиент неба от горизонта к зениту
    vec3 horizon_color = vec3(0.7, 0.8, 1.0);  // Светло-голубой на горизонте
    vec3 zenith_color = vec3(0.3, 0.5, 0.9);   // Темно-синий в зените
    
    // Добавляем вариации в зависимости от времени
    float time_variation = sin(time * 0.1) * 0.1;
    zenith_color += vec3(time_variation * 0.1);
    
    return mix(horizon_color, zenith_color, uv.y);
}

void main()
{
    vec2 uv = TexCoord;
    
    // Создаем объемные облака
    float cloud_density = volumetricClouds(uv);
    
    // Создаем небо
    vec3 sky_color = createSky(uv);
    
    // Цвета облаков
    vec3 cloud_white = vec3(0.98, 0.98, 0.98);     // Белый
    vec3 cloud_gray = vec3(0.8, 0.8, 0.85);        // Серый
    vec3 cloud_shadow = vec3(0.6, 0.65, 0.75);     // Тень
    
    // Создаем объемный эффект облаков
    float cloud_threshold = 0.4;
    float cloud_softness = 0.2;
    
    // Основная маска облаков
    float cloud_mask = smoothstep(cloud_threshold - cloud_softness, 
                                 cloud_threshold + cloud_softness, cloud_density);
    
    // Тень облаков
    float shadow_mask = smoothstep(cloud_threshold - cloud_softness * 2.0, 
                                  cloud_threshold, cloud_density);
    
    // Смешиваем цвета
    vec3 final_color = sky_color;
    
    // Добавляем облака
    final_color = mix(final_color, cloud_white, cloud_mask * 0.9);
    
    // Добавляем тени
    final_color = mix(final_color, cloud_shadow, shadow_mask * 0.4);
    
    // Добавляем атмосферную перспективу
    float distance = length(WorldPos);
    float fog_density = 0.05;
    float fog = 1.0 - exp(-distance * fog_density);
    vec3 fog_color = vec3(0.8, 0.85, 0.9);
    final_color = mix(final_color, fog_color, fog * 0.2);
    
    // Добавляем легкое свечение на краях облаков
    float edge_glow = smoothstep(0.0, 0.1, cloud_mask) * smoothstep(1.0, 0.9, cloud_mask);
    final_color += edge_glow * 0.1;
    
    // Финальная коррекция цвета
    final_color = pow(final_color, vec3(0.9)); // Легкая гамма-коррекция
    
    FragColor = vec4(final_color, 1.0);
}