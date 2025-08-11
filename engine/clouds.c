#include "mental.h"
#include "component.h"
#include "wm.h"
#include "perlin.h"
#include <math.h>

// Cloud component functions
void __mental_create_clouds(MentalComponent *pComponent) {
    // Создаем одно реалистичное облако с высокой детализацией
    const int numClouds = 1; // Только одно облако
    const int sphereSegments = 30; // Высокая детализация для реалистичности
    
    // Вычисляем количество вершин и индексов для всех облаков
    int verticesPerSphere = (sphereSegments + 1) * (sphereSegments + 1);
    int indicesPerSphere = sphereSegments * sphereSegments * 6;
    
    int totalVertices = numClouds * verticesPerSphere;
    int totalIndices = numClouds * indicesPerSphere;
    
    // Создаем массивы для вершин и индексов
    float* vertices = malloc(totalVertices * 3 * sizeof(float));
    unsigned int* indices = malloc(totalIndices * sizeof(unsigned int));
    
    if (!vertices || !indices) {
        MENTAL_DEBUG("Failed to allocate memory for cloud geometry");
        if (vertices) free(vertices);
        if (indices) free(indices);
        return;
    }
    
    int vertexOffset = 0;
    int indexOffset = 0;
    
    // Позиция одного реалистичного облака
    float cloudPositions[1][3] = {
        {0.0f, 4.0f, 0.0f} // Облако по центру, немного выше
    };
    
    // Размер облака
    float cloudRadii[1] = {
        4.0f  // Большое облако для лучшей видимости деталей
    };
    
    // Создаем сферы для каждого облака с разными размерами и деформациями
    for (int cloud = 0; cloud < numClouds; cloud++) {
        float centerX = cloudPositions[cloud][0];
        float centerY = cloudPositions[cloud][1];
        float centerZ = cloudPositions[cloud][2];
        float cloudRadius = cloudRadii[cloud]; // Используем разные размеры
        
        // Генерируем сферу с деформациями для более реалистичного вида
        for (int lat = 0; lat <= sphereSegments; lat++) {
            float theta = lat * 3.14159f / sphereSegments;
            float sinTheta = sin(theta);
            float cosTheta = cos(theta);
            
            for (int lon = 0; lon <= sphereSegments; lon++) {
                float phi = lon * 2.0f * 3.14159f / sphereSegments;
                float sinPhi = sin(phi);
                float cosPhi = cos(phi);
                
                // Базовая сферическая форма
                float x = cosPhi * sinTheta;
                float y = cosTheta;
                float z = sinPhi * sinTheta;
                
                // Добавляем сложные деформации для максимально реалистичного вида
                // Используем многослойные синусоидальные функции с разными частотами
                // Базовая форма облака (крупные выпуклости)
                float baseDeformX = 0.3f * sin(x * 2.0f) * sin(y * 1.5f);
                float baseDeformY = 0.25f * sin(y * 2.5f) * sin(z * 2.0f);
                float baseDeformZ = 0.35f * sin(z * 1.8f) * sin(x * 2.2f);
                
                // Средние детали (меньшие выпуклости)
                float midDeformX = 0.15f * sin(x * 5.0f) * sin(y * 4.0f);
                float midDeformY = 0.12f * sin(y * 6.0f) * sin(z * 5.5f);
                float midDeformZ = 0.18f * sin(z * 4.5f) * sin(x * 6.5f);
                
                // Мелкие детали (текстура поверхности)
                float fineDeformX = 0.05f * sin(x * 12.0f) * sin(y * 10.0f);
                float fineDeformY = 0.04f * sin(y * 15.0f) * sin(z * 14.0f);
                float fineDeformZ = 0.06f * sin(z * 13.0f) * sin(x * 11.0f);
                
                // Комбинируем все уровни деформаций
                float deformX = baseDeformX + midDeformX + fineDeformX;
                float deformY = baseDeformY + midDeformY + fineDeformY;
                float deformZ = baseDeformZ + midDeformZ + fineDeformZ;
                
                // Добавляем асимметрию для более естественного вида
                if (y > 0) {
                    // Верхняя часть облака более выпуклая
                    deformY *= 1.2f;
                } else {
                    // Нижняя часть облака более плоская
                    deformY *= 0.8f;
                }
                
                // Применяем деформации и масштабирование
                x = centerX + cloudRadius * (x + deformX);
                y = centerY + cloudRadius * (y + deformY);
                z = centerZ + cloudRadius * (z + deformZ);
                
                int vertexIndex = vertexOffset + lat * (sphereSegments + 1) + lon;
                vertices[vertexIndex * 3] = x;
                vertices[vertexIndex * 3 + 1] = y;
                vertices[vertexIndex * 3 + 2] = z;
            }
        }
        
        // Генерируем индексы для сферы
        for (int lat = 0; lat < sphereSegments; lat++) {
            for (int lon = 0; lon < sphereSegments; lon++) {
                int current = lat * (sphereSegments + 1) + lon;
                int next = current + sphereSegments + 1;
                
                // Первый треугольник
                indices[indexOffset++] = vertexOffset + current;
                indices[indexOffset++] = vertexOffset + next;
                indices[indexOffset++] = vertexOffset + current + 1;
                
                // Второй треугольник
                indices[indexOffset++] = vertexOffset + current + 1;
                indices[indexOffset++] = vertexOffset + next;
                indices[indexOffset++] = vertexOffset + next + 1;
            }
        }
        
        vertexOffset += verticesPerSphere;
    }
    
    glGenVertexArrays(1, &pComponent->VAO);
    glGenBuffers(1, &pComponent->VBO);
    glGenBuffers(1, &pComponent->EBO);
    
    glBindVertexArray(pComponent->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, pComponent->VBO);
    glBufferData(GL_ARRAY_BUFFER, totalVertices * 3 * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pComponent->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalIndices * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    
    // Освобождаем память
    free(vertices);
    free(indices);
    
    // Сохраняем количество индексов для отрисовки
    pComponent->indexCount = totalIndices;
}

MentalResult mentalCreateCloudComponent(MentalComponent* pComponent) {
    if (!pComponent) {
        MENTAL_DEBUG("Cloud component pointer is null.");
        return MENTAL_POINTER_IS_NULL;
    }

    // Инициализируем компонент
    pComponent->eType = MENTAL_COMPONENT_TYPE_CLOUDS;
    pComponent->position[0] = 0.0f;
    pComponent->position[1] = 3.0f; // Облака выше земли
    pComponent->position[2] = 0.0f;
    pComponent->size = 1.0f;
    pComponent->rotation[0] = 0.0f;
    pComponent->rotation[1] = 0.0f;
    pComponent->rotation[2] = 0.0f;
    pComponent->VAO = 0;
    pComponent->VBO = 0;
    pComponent->EBO = 0;
    pComponent->shaderProgram = 0;
    pComponent->indexCount = 0;

    // Создаем геометрию облаков
    __mental_create_clouds(pComponent);

    MENTAL_DEBUG("Cloud component created successfully.");
    return MENTAL_OK;
}

MentalResult mentalDrawCloudComponent(MentalComponent* pComponent, MentalWindowManager *pManager) {
    if (!pComponent || !pManager) {
        MENTAL_DEBUG("Cloud component or WindowManager pointer is null.");
        return MENTAL_POINTER_IS_NULL;
    }

    if (pComponent->shaderProgram == 0) {
        MENTAL_DEBUG("Cloud component shader not attached.");
        return MENTAL_ERROR;
    }

    // Включаем смешивание для прозрачности
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glUseProgram(pComponent->shaderProgram);

    // Получаем матрицы
    mat4 model, view, projection;
    glm_mat4_identity(model);
    
    // Применяем трансформации
    glm_translate(model, pComponent->position);
    glm_rotate_x(model, glm_rad(pComponent->rotation[0]), model);
    glm_rotate_y(model, glm_rad(pComponent->rotation[1]), model);
    glm_rotate_z(model, glm_rad(pComponent->rotation[2]), model);
    glm_scale_uni(model, pComponent->size);

    mental_camera_get_view_matrix(&pManager->camera, view);
    mental_camera_get_projection_matrix(&pManager->camera, projection, 
                                       (float)pManager->pInfo->aSizes[0] / pManager->pInfo->aSizes[1]);

    // Передаем матрицы в шейдер
    glUniformMatrix4fv(glGetUniformLocation(pComponent->shaderProgram, "model"), 1, GL_FALSE, (float*)model);
    glUniformMatrix4fv(glGetUniformLocation(pComponent->shaderProgram, "view"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(pComponent->shaderProgram, "projection"), 1, GL_FALSE, (float*)projection);

    // Передаем время в шейдер
    GLint timeLoc = glGetUniformLocation(pComponent->shaderProgram, "time");
    if (timeLoc != -1) {
        glUniform1f(timeLoc, (float)glfwGetTime());
    }

    // Отрисовываем облака
    glBindVertexArray(pComponent->VAO);
    glDrawElements(GL_TRIANGLES, pComponent->indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Отключаем смешивание
    glDisable(GL_BLEND);

    return MENTAL_OK;
}
