#include "engine/mental.h"
#include "engine/component.h"
#include "engine/wm.h"
#include <cglm/cglm.h>
#include <cglm/vec3.h>
#include <stdio.h>

int main() {
    // Инициализация менеджера окон
    MentalWindowManager wm;
    MentalWindowManagerInfo wmInfo = {
        .eType = MENTAL_STRUCTURE_TYPE_WINDOW_MANAGER_INFO,
        .aSizes = {800, 600},
        .pTitle = "PBR Material Example"
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
    
    // Установка PBR шейдеров для 3D модели
    result = mentalAttachPBRShader(modelComponent);
    if (result != MENTAL_SUCCESS) {
        printf("Failed to attach PBR shaders\n");
        mentalDestroyModel3DComponent(modelComponent);
        mentalDestroyCM(&cm);
        mentalDestroyWM(&wm);
        return -1;
    }
    
    // Установка PBR материала (золото)
    vec3 albedo = {1.0f, 0.765f, 0.336f}; // Золотой цвет
    float metallic = 1.0f;                // Полностью металлический
    float roughness = 0.1f;               // Гладкая поверхность
    float ao = 1.0f;                      // Без затенения
    
    mentalSetModelPBRMaterial(modelComponent, albedo, metallic, roughness, ao);
    
    // Попытка загрузить PBR текстуры (если есть)
    // Текстуры должны иметь суффиксы _albedo, _normal, _metallic, _roughness, _ao
    mentalLoadModelTexture(modelComponent, "cube_albedo.png");
    
    // Установка позиции и размера модели
    mentalSetPosition(modelComponent, 0.0f, 0.0f);
    mentalSetSize(modelComponent, 1.0f);
    
    // Настройка OpenGL для работы с 3D
    glEnable(GL_DEPTH_TEST);
    
    // Главный цикл
    float lastFrame = 0.0f;
    float rotation = 0.0f;
    
    while (!mentalShouldCloseWM(&wm)) {
        // Расчет времени кадра
        float currentFrame = mentalGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Обработка ввода
        mentalProcessInputWM(&wm, deltaTime);
        
        // Вращение модели
        rotation += deltaTime * 50.0f;
        mentalSetRotation(modelComponent, 0.0f, rotation, 0.0f);
        
        // Очистка буфера
        mentalClearWM(&wm);
        
        // Отрисовка модели
        mentalDrawModel3DComponent(modelComponent, &wm);
        
        // Обновление экрана
        mentalSwapBuffersWM(&wm);
        mentalPollEventsWM();
    }
    
    // Очистка ресурсов
    mentalDestroyModel3DComponent(modelComponent);
    mentalDestroyCM(&cm);
    mentalDestroyWM(&wm);
    
    return 0;
}