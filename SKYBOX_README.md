# Скайбокс в Mental Engine

## Описание

Скайбокс - это технология рендеринга, которая создает иллюзию бесконечного неба или окружения вокруг 3D сцены. В Mental Engine скайбокс реализован как куб с текстурами на каждой грани.

## Структура файлов

```
textures/skybox/
├── right.ppm  # Правая грань куба
├── left.ppm   # Левая грань куба
├── top.ppm    # Верхняя грань куба
├── bottom.ppm # Нижняя грань куба
├── front.ppm  # Передняя грань куба
└── back.ppm   # Задняя грань куба
```

## Использование

### 1. Автоматическое создание текстур

Для создания тестовых текстур используйте программу `create_textures`:

```bash
./create_textures
```

Эта программа создаст:
- Боковые грани: небо с облаками
- Верхняя грань: градиент неба
- Нижняя грань: зеленая земля

### 2. Использование собственных текстур

Вы можете заменить текстуры в папке `textures/skybox/` на свои собственные изображения. Поддерживаемые форматы:

- **PPM** (рекомендуется для простоты)
- Другие форматы можно добавить, расширив функцию `loadImage()` в `engine/skybox.c`

### 3. Требования к текстурам

- Размер: рекомендуется 512x512 пикселей
- Формат: RGB (3 канала)
- Ориентация: грани должны быть правильно ориентированы

## API

### Структура MentalSkybox

```c
typedef struct MentalSkybox {
    uint32_t   VAO, VBO;
    uint32_t   cubemapTexture;
    uint32_t   shaderProgram;
    bool       isInitialized;
} MentalSkybox;
```

### Основные функции

#### Создание скайбокса
```c
MentalResult mentalCreateSkybox(MentalSkybox* pSkybox);
```

#### Загрузка текстур
```c
MentalResult mentalLoadSkyboxTextures(MentalSkybox* pSkybox, 
                                     const char* right, const char* left, 
                                     const char* top, const char* bottom, 
                                     const char* front, const char* back);
```

#### Прикрепление шейдеров
```c
MentalResult mentalAttachSkyboxShader(MentalSkybox* pSkybox, 
                                     const char* vertex_path, const char* fragment_path);
```

#### Отрисовка скайбокса
```c
MentalResult mentalDrawSkybox(MentalSkybox* pSkybox, MentalWindowManager *pManager);
```

#### Уничтожение скайбокса
```c
MentalResult mentalDestroySkybox(MentalSkybox *pSkybox);
```

## Шейдеры

### Вершинный шейдер (skybox_vertex.glsl)
```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
```

### Фрагментный шейдер (skybox_fragment.glsl)
```glsl
#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
}
```

## Интеграция в основной цикл

Скайбокс автоматически интегрирован в `MentalWindowManager` и отрисовывается в начале каждого кадра:

```c
// В функции mentalRunWM
// Draw skybox first (background)
if (mentalDrawSkybox(&pManager->skybox, pManager) != MENTAL_OK) {
    MENTAL_DEBUG("Failed to draw skybox.");
}
```

## Создание собственных текстур

### Программное создание

Вы можете модифицировать `create_textures.c` для создания собственных текстур:

```c
// Пример создания градиента
void create_custom_gradient(RGB* pixels, int width, int height) {
    for (int y = 0; y < height; y++) {
        float t = (float)y / (height - 1);
        RGB color;
        color.r = (unsigned char)(255 * (1 - t));
        color.g = (unsigned char)(100 * t);
        color.b = (unsigned char)(255 * t);
        
        for (int x = 0; x < width; x++) {
            pixels[y * width + x] = color;
        }
    }
}
```

### Использование внешних изображений

1. Подготовьте 6 изображений для каждой грани куба
2. Конвертируйте их в PPM формат
3. Разместите в папке `textures/skybox/`
4. Переименуйте согласно схеме: right.ppm, left.ppm, top.ppm, bottom.ppm, front.ppm, back.ppm

## Производительность

- Скайбокс отрисовывается как единый куб
- Использует оптимизированные шейдеры
- Автоматически настраивает тест глубины для корректного рендеринга
- Минимальное влияние на производительность

## Устранение неполадок

### Текстуры не загружаются
- Проверьте пути к файлам
- Убедитесь, что файлы существуют и доступны для чтения
- Проверьте формат файлов (должен быть PPM)

### Скайбокс не отображается
- Проверьте, что шейдеры успешно скомпилированы
- Убедитесь, что текстуры загружены корректно
- Проверьте логи на наличие ошибок

### Артефакты рендеринга
- Убедитесь, что текстуры имеют одинаковый размер
- Проверьте, что грани правильно ориентированы
- Убедитесь, что тест глубины настроен корректно
