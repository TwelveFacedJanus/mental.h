# PBR Materials в Mental Graphics Engine

## Обзор

В Mental Graphics Engine добавлена поддержка PBR (Physically Based Rendering) материалов для 3D моделей. PBR обеспечивает более реалистичное отображение материалов, основанное на физических свойствах поверхностей.

## Возможности

- Поддержка PBR материалов с параметрами albedo, metallic, roughness и ambient occlusion
- Загрузка PBR текстур (albedo, normal, metallic, roughness, ambient occlusion)
- Совместимость с существующими традиционными материалами (для обратной совместимости)
- Реализация Cook-Torrance BRDF для расчета освещения
- HDR тонирование и гамма-коррекция

## Использование

### Создание 3D модели с PBR материалом

```c
// Создание компонента 3D модели
MentalComponent* modelComponent = &cm.components[cm.count - 1];
mentalCreateModel3DComponent(modelComponent);

// Загрузка 3D модели
mentalLoadModel3D(modelComponent, "model.obj");

// Установка PBR шейдеров
mentalAttachPBRShader(modelComponent);

// Установка PBR материала (золото)
vec3 albedo = {1.0f, 0.765f, 0.336f}; // Золотой цвет
float metallic = 1.0f;                // Полностью металлический
float roughness = 0.1f;               // Гладкая поверхность
float ao = 1.0f;                      // Без затенения

mentalSetModelPBRMaterial(modelComponent, albedo, metallic, roughness, ao);

// Отрисовка модели
mentalDrawModel3DComponent(modelComponent, &wm);
```

### Загрузка PBR текстур

Для автоматической загрузки PBR текстур, файлы текстур должны иметь следующие суффиксы:

- `_albedo.png/.jpg` - базовый цвет (альбедо)
- `_normal.png/.jpg` - карта нормалей
- `_metallic.png/.jpg` - карта металличности
- `_roughness.png/.jpg` - карта шероховатости
- `_ao.png/.jpg` - карта ambient occlusion

Например, если ваша модель называется `model.obj`, то текстуры должны называться `model_albedo.png`, `model_normal.png` и т.д.

## Пример использования

В репозитории есть пример использования `pbr_example.c`, который демонстрирует загрузку и отображение 3D модели с PBR материалом.

### Компиляция и запуск примера

```bash
# Компиляция
make -f Makefile.pbr

# Запуск
./build/pbr_example
```

## Технические детали

### PBR шейдеры

- `pbr_vertex.glsl` - вершинный шейдер для PBR
- `pbr_fragment.glsl` - фрагментный шейдер для PBR

### Структура материала

```c
typedef struct Material {
    // Традиционные параметры материала (для обратной совместимости)
    vec3 ambient;          // Коэффициент отражения окружающего света
    vec3 diffuse;          // Коэффициент диффузного отражения
    vec3 specular;         // Коэффициент бликового отражения
    float shininess;       // Блеск (размер блика)
    
    // PBR параметры материала
    vec3 albedo;           // Базовый цвет (альбедо)
    float metallic;        // Металличность (0.0 - диэлектрик, 1.0 - металл)
    float roughness;       // Шероховатость (0.0 - гладкий, 1.0 - шероховатый)
    float ao;              // Ambient Occlusion (затенение в складках)
    
    bool use_pbr;          // Флаг использования PBR материала
} Material;
```

## Примеры PBR материалов

### Золото
```c
vec3 albedo = {1.0f, 0.765f, 0.336f};
float metallic = 1.0f;
float roughness = 0.1f;
float ao = 1.0f;
```

### Пластик
```c
vec3 albedo = {0.9f, 0.9f, 0.9f}; // Белый пластик
float metallic = 0.0f;             // Не металлический
float roughness = 0.5f;            // Средняя шероховатость
float ao = 1.0f;
```

### Ржавое железо
```c
vec3 albedo = {0.7f, 0.25f, 0.1f}; // Ржавый цвет
float metallic = 0.6f;              // Частично металлический
float roughness = 0.9f;             // Очень шероховатый
float ao = 0.8f;                    // Небольшое затенение
```