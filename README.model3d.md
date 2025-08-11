# Загрузка 3D моделей в Mental Graphics Engine

## Обзор

В Mental Graphics Engine добавлена возможность загрузки и отображения 3D моделей в формате OBJ. Эта функциональность позволяет загружать 3D модели с текстурами и отображать их с настраиваемыми материалами и освещением.

## Возможности

- Загрузка 3D моделей в формате OBJ
- Поддержка текстур (PNG, JPG)
- Настраиваемые материалы с параметрами ambient, diffuse, specular и shininess
- Расширенное освещение с учетом свойств материалов
- Трансформации модели (позиция, размер, вращение)

## Использование

### Создание компонента 3D модели

```c
// Создание компонента 3D модели
MentalComponent* modelComponent = mentalCreateModel3DComponent(&cm);

// Загрузка 3D модели
MentalResult result = mentalLoadModel3D(modelComponent, "path/to/model.obj");

// Установка шейдеров
mentalAttachShader(modelComponent, "model3d_vertex.glsl", "model3d_fragment.glsl");

// Установка позиции и размера модели
mentalSetPosition(modelComponent, 0.0f, 0.0f, 0.0f);
mentalSetSize(modelComponent, 1.0f);
mentalSetRotation(modelComponent, 0.0f, 0.0f, 0.0f);

// Установка материала (например, золото)
vec3 ambient = {0.24725f, 0.1995f, 0.0745f};
vec3 diffuse = {0.75164f, 0.60648f, 0.22648f};
vec3 specular = {0.628281f, 0.555802f, 0.366065f};
float shininess = 51.2f;
mentalSetModelMaterial(modelComponent, ambient, diffuse, specular, shininess);

// Загрузка текстуры (опционально)
mentalLoadModelTexture(modelComponent, "path/to/texture.png");

// Отрисовка модели
mentalDrawModel3DComponent(modelComponent, &camera);

// Освобождение ресурсов
mentalDestroyModel3DComponent(modelComponent);
```

### Пример использования

В репозитории есть пример использования `model3d_example.c`, который демонстрирует загрузку и отображение 3D модели куба.

Для компиляции примера используйте:

```bash
make -f Makefile.model3d
```

Для запуска примера:

```bash
./build/model3d_example
```

## Поддерживаемые форматы OBJ

Текущая реализация поддерживает следующие форматы OBJ:

- Формат с вершинами, текстурными координатами и нормалями: `f v/vt/vn v/vt/vn v/vt/vn`
- Формат с вершинами и нормалями: `f v//vn v//vn v//vn`
- Простой формат с вершинами: `f v v v`

## Ограничения

- Поддерживаются только треугольные грани
- Не поддерживаются материалы из MTL файлов (но можно задавать программно)
- Освещение с одним источником света

## Зависимости

- OpenGL
- GLEW
- GLFW
- stb_image (для загрузки текстур)

## Примеры материалов

Ниже приведены примеры настроек для различных материалов:

### Золото
```c
vec3 ambient = {0.24725f, 0.1995f, 0.0745f};
vec3 diffuse = {0.75164f, 0.60648f, 0.22648f};
vec3 specular = {0.628281f, 0.555802f, 0.366065f};
float shininess = 51.2f;
```

### Серебро
```c
vec3 ambient = {0.19225f, 0.19225f, 0.19225f};
vec3 diffuse = {0.50754f, 0.50754f, 0.50754f};
vec3 specular = {0.508273f, 0.508273f, 0.508273f};
float shininess = 51.2f;
```

### Бронза
```c
vec3 ambient = {0.2125f, 0.1275f, 0.054f};
vec3 diffuse = {0.714f, 0.4284f, 0.18144f};
vec3 specular = {0.393548f, 0.271906f, 0.166721f};
float shininess = 25.6f;
```

### Пластик (красный)
```c
vec3 ambient = {0.0f, 0.0f, 0.0f};
vec3 diffuse = {0.5f, 0.0f, 0.0f};
vec3 specular = {0.7f, 0.6f, 0.6f};
float shininess = 32.0f;
```

### Резина (черная)
```c
vec3 ambient = {0.02f, 0.02f, 0.02f};
vec3 diffuse = {0.01f, 0.01f, 0.01f};
vec3 specular = {0.4f, 0.4f, 0.4f};
float shininess = 10.0f;
```