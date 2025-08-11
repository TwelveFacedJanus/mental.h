#include "engine/mental.h"
#include "engine/component.h"
#include "engine/wm.h"
#include <stdio.h>

int main() {
    // Инициализация менеджера окон
    MentalWindowManager wm;
    MentalWindowManagerInfo wmInfo = {
        .eType = MENTAL_STRUCTURE_TYPE_WINDOW_MANAGER_INFO,
        .aSizes = {800, 600},
        .pTitle = "3D Model Example"
    };
    
    wm.pInfo = &wmInfo;
    MentalResult result = mentalCreateWM(&wm);
    if (result != MENTAL_SUCCESS) {
        printf("Failed to create window manager\n");
        return -1;
    }
    
    // Инициализация менеджера компонентов
    MentalComponentManager cm;
    result = mentalCreateCM(&cm);
    if (result != MENTAL_SUCCESS) {
        printf("Failed to create component manager\n");
        mentalDestroyWM(&wm);
        return -1;
    }
    
    // Камера уже инициализирована в менеджере окон (wm.camera)
    
    // Создание компонента 3D модели
    result = mentalAddComponent(&cm);
    if (result != MENTAL_SUCCESS) {
        printf("Failed to add component\n");
        mentalDestroyCM(&cm);
        mentalDestroyWM(&wm);
        return -1;
    }
    
    MentalComponent* modelComponent = &cm.components[cm.count - 1];
    result = mentalCreateModel3DComponent(modelComponent);
    if (result != MENTAL_SUCCESS) {
        printf("Failed to create model component\n");
        mentalDestroyCM(&cm);
        mentalDestroyWM(&wm);
        return -1;
    }
    
    // Загрузка 3D модели
    result = mentalLoadModel3D(modelComponent, "cube.obj");
    if (result != MENTAL_SUCCESS) {
        printf("Failed to load 3D model\n");
        mentalDestroyModel3DComponent(modelComponent);
        mentalDestroyCM(&cm);
        mentalDestroyWM(&wm);
        return -1;
    }
    
    // Установка шейдеров для 3D модели
    result = mentalAttachShader(modelComponent, "model3d_vertex.glsl", "model3d_fragment.glsl");
    if (result != MENTAL_SUCCESS) {
        printf("Failed to attach shaders\n");
        mentalDestroyModel3DComponent(modelComponent);
        mentalDestroyCM(&cm);
        mentalDestroyWM(&wm);
        return -1;
    }
    
    // Установка материала для куба (золотой материал)
    vec3 ambient = {0.24725f, 0.1995f, 0.0745f};
    vec3 diffuse = {0.75164f, 0.60648f, 0.22648f};
    vec3 specular = {0.628281f, 0.555802f, 0.366065f};
    float shininess = 51.2f;
    
    mentalSetModelMaterial(modelComponent, ambient, diffuse, specular, shininess);
    
    // Попытка загрузить текстуру (если есть)
    mentalLoadModelTexture(modelComponent, "cube_texture.png");
    
    // Установка позиции и размера модели
    mentalSetPosition(modelComponent, 0.0f, 0.0f);
    mentalSetSize(modelComponent, 1.0f);
    
    // Настройка OpenGL для работы с 3D
    glEnable(GL_DEPTH_TEST);
    
    // Главный цикл
    float lastFrame = 0.0f;
    while (!glfwWindowShouldClose(wm.pNext)) {
        // Расчет времени кадра
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Обработка ввода
        mental_process_keyboard(&wm, deltaTime);
        
        // Очистка буфера
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Вращение модели
        modelComponent->rotation[1] += 30.0f * deltaTime; // Вращение вокруг оси Y (в градусах)
        
        // Отрисовка модели
        mentalDrawModel3DComponent(modelComponent, &wm);
        
        // Обмен буферов и обработка событий
        glfwSwapBuffers(wm.pNext);
        glfwPollEvents();
    }
    
    // Освобождение ресурсов
    mentalDestroyModel3DComponent(modelComponent);
    mentalDestroyCM(&cm);
    mentalDestroyWM(&wm);
    
    return 0;
}