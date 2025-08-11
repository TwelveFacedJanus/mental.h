#version 330 core
out vec4 FragColor;

in vec3 WorldPos;
in vec3 LocalPos;

uniform float time;

// Улучшенные функции шума для более реалистичных облаков
float hash(vec3 p) {
    p = fract(p * vec3(123.34, 234.34, 345.65));
    p += dot(p, p + 34.45);
    return fract(p.x * p.y * p.z);
}

float noise(vec3 p) {
    vec3 i = floor(p);
    vec3 f = fract(p);
    
    // Улучшенная интерполяция для более плавных переходов
    f = f * f * f * (f * (f * 6.0 - 15.0) + 10.0); // Quintic interpolation
    
    float a = hash(i);
    float b = hash(i + vec3(1.0, 0.0, 0.0));
    float c = hash(i + vec3(0.0, 1.0, 0.0));
    float d = hash(i + vec3(1.0, 1.0, 0.0));
    float e = hash(i + vec3(0.0, 0.0, 1.0));
    float f1 = hash(i + vec3(1.0, 0.0, 1.0));
    float g = hash(i + vec3(0.0, 1.0, 1.0));
    float h = hash(i + vec3(1.0, 1.0, 1.0));
    
    return mix(mix(mix(a, b, f.x), mix(c, d, f.x), f.y),
               mix(mix(e, f1, f.x), mix(g, h, f.x), f.y), f.z);
}

// Улучшенная функция фрактального броуновского движения
float fbm(vec3 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    float lacunarity = 2.0;    // Увеличение частоты между октавами
    float gain = 0.5;          // Уменьшение амплитуды между октавами
    
    // Больше октав для более детализированных облаков
    for (int i = 0; i < 6; i++) {
        value += amplitude * noise(p * frequency + time * 0.05 * float(i));
        frequency *= lacunarity;
        amplitude *= gain;
    }
    
    return value;
}

// Функция для создания турбулентности (вариация fbm)
float turbulence(vec3 p) {
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 1.0;
    float lacunarity = 2.0;
    float gain = 0.5;
    
    for (int i = 0; i < 6; i++) {
        value += amplitude * abs(noise(p * frequency) * 2.0 - 1.0);
        frequency *= lacunarity;
        amplitude *= gain;
    }
    
    return value;
}

// Функция для генерации плотности одного реалистичного облака с многослойной структурой
float cloudDensity(vec3 pos) {
    // Масштабируем координаты для более детального шума
    vec3 coord = pos * 0.4; // Уменьшаем масштаб для более крупного облака
    
    // Анимация движения облака - более медленная и плавная
    vec3 animOffset = vec3(time * 0.03, time * 0.01, time * 0.02);
    
    // Базовый шум для основной формы облака - более плавный
    float base = fbm(coord + animOffset * 0.2);
    
    // Средняя детализация - усиливаем для более выраженной структуры
    float medium = fbm(coord * 2.5 + animOffset * 0.5) * 0.6;
    
    // Высокая детализация - для создания пушистости
    float detail = fbm(coord * 5.0 + animOffset * 0.8) * 0.3;
    
    // Очень высокая детализация для мелких деталей - тонкие завихрения
    float microDetail = fbm(coord * 10.0 + animOffset * 1.2) * 0.15;
    
    // Турбулентность для создания более сложной структуры и "рваных" краев
    float turbulence = abs(fbm(coord * 3.0 + vec3(base)) - 0.5) * 2.5;
    
    // Создаем вертикальную стратификацию (слоистость) облака
    float heightGradient = 1.0 - smoothstep(0.0, 0.8, abs(pos.y - 0.2) / 2.0);
    
    // Горизонтальная вариация плотности
    float horizontalVariation = fbm(coord * 1.5 + vec3(time * 0.02)) * 0.5 + 0.5;
    
    // Комбинируем слои шума с разными весами
    float density = base * 0.45 + medium * 0.3 + detail * 0.15 + microDetail * 0.1;
    
    // Применяем вертикальную стратификацию и горизонтальную вариацию
    density *= heightGradient * (0.8 + horizontalVariation * 0.4);
    
    // Добавляем турбулентность для создания "рваных" краев облака
    density = mix(density, density * (1.0 + turbulence * 0.7), 0.5);
    
    // Создаем более резкие переходы для реалистичности
    density = smoothstep(0.35, 0.65, density);
    
    // Добавляем вариации по высоте (облака более плотные в середине)
    // Используем более сложную функцию для естественного распределения
    float heightVariation = noise(vec3(pos.x * 0.2, 0.0, pos.z * 0.2) + animOffset * 0.5) * 0.3 + 0.7;
    float heightFactor = 1.0 - pow(2.0 * abs(pos.y * heightVariation), 2.0);
    heightFactor = max(0.0, heightFactor);
    
    // Добавляем горизонтальные вариации для более естественной формы
    float horizontalShapeVariation = fbm(vec3(pos.x * 0.3, pos.z * 0.3, time * 0.1)) * 0.4 + 0.6;
    
    // Применяем все факторы к плотности
    density *= heightFactor * horizontalShapeVariation;
    
    return density;
}

// Функция для расчета объемного рассеивания света в облаке
vec3 calculateScattering(vec3 pos, float density) {
    // Направление света (солнца) с плавной анимацией
    vec3 lightDir = normalize(vec3(0.5 + sin(time * 0.03) * 0.2, 0.8 + sin(time * 0.02) * 0.2, 0.3 + cos(time * 0.025) * 0.2));
    
    // Базовый цвет облака с более выраженными вариациями в зависимости от позиции
    float posVariation = noise(pos * 0.4 + vec3(time * 0.01)) * 0.1;
    vec3 baseColor = vec3(0.98 + posVariation, 0.98 + posVariation * 0.9, 0.99 + posVariation * 0.7);
    
    // Более реалистичный цикл времени суток
    float timeOfDay = (sin(time * 0.05) + 1.0) * 0.5; // 0-1 цикл дня, более медленный
    
    // Улучшенные цвета для разного времени суток
    vec3 sunriseColor = vec3(0.95, 0.65, 0.55); // Восход (оранжево-розовый)
    vec3 morningColor = vec3(0.85, 0.75, 0.95); // Утренний цвет (нежно-розовый)
    vec3 middayColor = vec3(0.75, 0.85, 1.0);  // Дневной цвет (голубой)
    vec3 eveningColor = vec3(0.95, 0.7, 0.5);  // Вечерний цвет (оранжевый)
    vec3 sunsetColor = vec3(0.98, 0.5, 0.4);   // Закат (красно-оранжевый)
    
    // Более плавное смешивание цветов в зависимости от времени
    vec3 scatterColor;
    if (timeOfDay < 0.2) {
        // Восход -> утро
        scatterColor = mix(sunriseColor, morningColor, timeOfDay * 5.0);
    } else if (timeOfDay < 0.4) {
        // Утро -> день
        scatterColor = mix(morningColor, middayColor, (timeOfDay - 0.2) * 5.0);
    } else if (timeOfDay < 0.6) {
        // День -> вечер
        scatterColor = mix(middayColor, eveningColor, (timeOfDay - 0.4) * 5.0);
    } else if (timeOfDay < 0.8) {
        // Вечер -> закат
        scatterColor = mix(eveningColor, sunsetColor, (timeOfDay - 0.6) * 5.0);
    } else {
        // Вечер -> утро
        scatterColor = mix(eveningColor, morningColor, (timeOfDay - 0.66) * 3.0);
    }
    
    // Цвет подсветки краев с вариациями в зависимости от времени
    vec3 sunlightColor = mix(vec3(1.0, 0.9, 0.7), vec3(1.0, 0.7, 0.4), timeOfDay);
    
    // Рассчитываем направление от точки к камере
    vec3 viewDir = normalize(-pos);
    
    // Фактор обратного рассеивания (сильнее, когда смотрим против солнца)
    float backScatter = pow(max(dot(viewDir, -lightDir), 0.0), 2.0) * 0.5;
    
    // Фактор прямого рассеивания (сильнее на краях облаков)
    float forwardScatter = pow(max(dot(viewDir, lightDir), 0.0), 8.0);
    
    // Фактор подсветки краев
    float edgeLight = pow(1.0 - density, 4.0) * forwardScatter;
    
    // Комбинируем все факторы освещения
    vec3 finalColor = mix(baseColor, sunlightColor, edgeLight * 0.5);
    finalColor = mix(finalColor, scatterColor, backScatter * 0.3);
    
    // Добавляем вариации цвета в зависимости от позиции
    float colorNoise = noise(pos * 5.0 + time * 0.1) * 0.1;
    finalColor += vec3(colorNoise);
    
    return finalColor;
}

// Функция для симуляции объемного рассеивания света в облаке
vec4 volumetricScattering(vec3 pos, float density) {
    // Направление к камере
    vec3 viewDir = normalize(-pos);
    
    // Базовый цвет и рассеивание
    vec3 color = calculateScattering(pos, density);
    
    // Симуляция объемного рассеивания света с более плавной анимацией
    // Делаем несколько шагов вдоль направления к свету
    vec3 lightDir = normalize(vec3(0.5 + sin(time * 0.03) * 0.15, 1.0 + sin(time * 0.02) * 0.05, 0.3 + cos(time * 0.025) * 0.15));
    float lightAccumulation = 0.0;
    float stepSize = 0.15; // Меньший размер шага для более точного рассеивания
    
    // Увеличиваем количество шагов для более реалистичного объемного рассеивания
    for (int i = 0; i < 8; i++) {
        vec3 samplePos = pos + lightDir * stepSize * float(i);
        float sampleDensity = cloudDensity(samplePos);
        // Экспоненциальное затухание света через облако
        lightAccumulation += (1.0 - sampleDensity) * 0.15 * exp(-float(i) * 0.2);
    }
    
    // Ограничиваем накопление света
    lightAccumulation = clamp(lightAccumulation, 0.0, 1.0);
    
    // Добавляем эффект подсветки объема с более теплым оттенком для солнечного света
    vec3 sunlightColor = mix(vec3(1.0, 0.9, 0.7), vec3(1.0, 0.8, 0.5), sin(time * 0.02) * 0.5 + 0.5);
    color += lightAccumulation * sunlightColor * 0.4;
    
    // Добавляем легкий эффект серебристой подсветки для краев облака
    float rimLight = pow(1.0 - abs(dot(normalize(pos), lightDir)), 4.0);
    color += rimLight * vec3(0.9, 0.95, 1.0) * 0.2 * density;
    
    // Рассчитываем прозрачность с учетом объемности и более мягкими краями
    float baseAlpha = density * 0.85;
    float edgeAlpha = pow(density, 1.7) * 0.95;
    float finalAlpha = mix(baseAlpha, edgeAlpha, 0.7);
    
    // Добавляем вариации прозрачности для более естественного вида
    finalAlpha *= (1.0 + noise(pos * 5.0) * 0.15);
    
    // Добавляем эффект глубины - более далекие части облака менее прозрачны
    float depth = length(pos);
    finalAlpha *= mix(1.0, 0.65, clamp(depth * 0.12, 0.0, 1.0));
    
    return vec4(color, finalAlpha);
}

void main()
{
    // Используем локальную позицию для генерации облака с небольшим масштабированием
    vec3 cloudPos = LocalPos * 1.8; // Немного уменьшаем масштаб для более компактного облака
    
    // Добавляем небольшое смещение для создания эффекта плавного движения
    cloudPos.x += sin(time * 0.01) * 0.1;
    cloudPos.z += cos(time * 0.015) * 0.08;
    
    // Генерируем плотность облака
    float density = cloudDensity(cloudPos);
    
    // Если плотность слишком низкая, отбрасываем фрагмент для оптимизации
    // Увеличиваем порог для более четких краев
    if (density < 0.015) {
        discard;
    }
    
    // Применяем объемное рассеивание света
    vec4 cloudWithScattering = volumetricScattering(cloudPos, density);
    
    // Добавляем эффект атмосферной дымки для дальних облаков с более плавным переходом
    float distance = length(cloudPos);
    float atmosphericFactor = 1.0 - exp(-distance * 0.06);
    
    // Динамический цвет атмосферной дымки в зависимости от времени суток
    float dayTime = sin(time * 0.01) * 0.5 + 0.5; // Цикл дня и ночи
    vec3 daySkyColor = vec3(0.8, 0.85, 0.95); // Дневное небо
    vec3 sunsetSkyColor = vec3(0.9, 0.7, 0.5); // Закатное небо
    vec3 atmosphericColor = mix(daySkyColor, sunsetSkyColor, pow(sin(time * 0.01 + 1.0) * 0.5 + 0.5, 2.0));
    
    // Смешиваем с атмосферной дымкой, усиливая эффект на краях
    float edgeFactor = pow(1.0 - density, 2.0) * 0.5;
    cloudWithScattering.rgb = mix(cloudWithScattering.rgb, atmosphericColor, atmosphericFactor * 0.35 + edgeFactor);
    
    // Добавляем легкое свечение для создания эффекта подсветки
    float glowFactor = pow(1.0 - density, 3.0) * 0.2;
    cloudWithScattering.rgb += atmosphericColor * glowFactor;
    
    // Ограничиваем значения
    cloudWithScattering.rgb = clamp(cloudWithScattering.rgb, 0.0, 1.0);
    cloudWithScattering.a = clamp(cloudWithScattering.a, 0.0, 1.0);
    
    FragColor = cloudWithScattering;
}
