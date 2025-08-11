#include "component.h"
#include "wm.h"
#include "perlin.h"

#include "../stb_image.h"

void __mental_create_rectangle(MentalComponent *pComponent)
{
    float vertices[] = {
        // positions        
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    
    glGenVertexArrays(1, &pComponent->VAO);
    glGenBuffers(1, &pComponent->VBO);
    glGenBuffers(1, &pComponent->EBO);
    
    glBindVertexArray(pComponent->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, pComponent->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pComponent->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void __mental_create_triangle(MentalComponent *pComponent)
{
    float vertices[] = {
        // positions        
         0.0f,  0.5f, 0.0f,  // top center
        -0.5f, -0.5f, 0.0f,  // bottom left
         0.5f, -0.5f, 0.0f   // bottom right
    };
    
    glGenVertexArrays(1, &pComponent->VAO);
    glGenBuffers(1, &pComponent->VBO);
    
    glBindVertexArray(pComponent->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, pComponent->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}
void __mental_draw_rectangle(MentalComponent* pComponent)
{
    glBindVertexArray(pComponent->VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void __mental_draw_triangle(MentalComponent* pComponent)
{
    glBindVertexArray(pComponent->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void mentalSetPosition(MentalComponent* pComponent, float x, float y)
{
    if (!pComponent) return;
    pComponent->position[0] = x;
    pComponent->position[1] = y;
}

void mentalSetSize(MentalComponent* pComponent, float size)
{
    if (!pComponent) return;
    pComponent->size = size;
}


MentalResult mentalAttachShader(MentalComponent* pComponent, const char* vertex_path, const char* fragment_path)
{
    // 1. Read vertex shader from file
    FILE* vertex_file = fopen(vertex_path, "r");
    if (!vertex_file) {
        MENTAL_DEBUG("Failed to open vertex shader file");
        return MENTAL_FILE_OPEN_FAILED;
    }

    // Get file size
    fseek(vertex_file, 0, SEEK_END);
    long vertex_size = ftell(vertex_file);
    fseek(vertex_file, 0, SEEK_SET);

    // Allocate buffer and read file
    char* vertex_source = (char*)malloc(vertex_size + 1);
    fread(vertex_source, 1, vertex_size, vertex_file);
    vertex_source[vertex_size] = '\0';
    fclose(vertex_file);

    // 2. Read fragment shader from file
    FILE* fragment_file = fopen(fragment_path, "r");
    if (!fragment_file) {
        MENTAL_DEBUG("Failed to open fragment shader file");
        free(vertex_source);
        return MENTAL_FILE_OPEN_FAILED;
    }

    // Get file size
    fseek(fragment_file, 0, SEEK_END);
    long fragment_size = ftell(fragment_file);
    fseek(fragment_file, 0, SEEK_SET);

    // Allocate buffer and read file
    char* fragment_source = (char*)malloc(fragment_size + 1);
    fread(fragment_source, 1, fragment_size, fragment_file);
    fragment_source[fragment_size] = '\0';
    fclose(fragment_file);

    // 3. Create and compile shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    if (vertexShader == 0) {
        MENTAL_DEBUG("Failed to create vertex shader.");
        free(vertex_source);
        free(fragment_source);
        return MENTAL_SHADER_COMPILE_FAILED;
    }
    
    glShaderSource(vertexShader, 1, (const char**)&vertex_source, NULL);
    glCompileShader(vertexShader);
    
    // Check for shader compile errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        MENTAL_DEBUG("Vertex shader compilation failed: %s", infoLog);
        free(vertex_source);
        free(fragment_source);
        glDeleteShader(vertexShader);
        return MENTAL_SHADER_COMPILE_FAILED;
    }

    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        MENTAL_DEBUG("Failed to create fragment shader.");
        free(vertex_source);
        free(fragment_source);
        glDeleteShader(vertexShader);
        return MENTAL_SHADER_COMPILE_FAILED;
    }
    
    glShaderSource(fragmentShader, 1, (const char**)&fragment_source, NULL);
    glCompileShader(fragmentShader);
    
    // Check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        MENTAL_DEBUG("Fragment shader compilation failed: %s", infoLog);
        free(vertex_source);
        free(fragment_source);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return MENTAL_SHADER_COMPILE_FAILED;
    }
    
    // 4. Link shaders
    pComponent->shaderProgram = glCreateProgram();
    if (pComponent->shaderProgram == 0) {
        MENTAL_DEBUG("Failed to create shader program.");
        free(vertex_source);
        free(fragment_source);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return MENTAL_SHADER_LINK_FAILED;
    }
    
    glAttachShader(pComponent->shaderProgram, vertexShader);
    glAttachShader(pComponent->shaderProgram, fragmentShader);
    glLinkProgram(pComponent->shaderProgram);
    
    // Check for linking errors
    glGetProgramiv(pComponent->shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(pComponent->shaderProgram, 512, NULL, infoLog);
        MENTAL_DEBUG("Shader program linking failed: %s", infoLog);
        free(vertex_source);
        free(fragment_source);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return MENTAL_SHADER_LINK_FAILED;
    }

    // 5. Cleanup
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    free(vertex_source);
    free(fragment_source);

    return MENTAL_OK;
}

MentalResult mentalCreateComponent(MentalComponent* pComponent) {
    if (!pComponent) {
        MENTAL_DEBUG("Component is null");
        return MENTAL_POINTER_IS_NULL;
    }
    
    switch(pComponent->eType) {
        case MENTAL_COMPONENT_TYPE_RECTANGLE:
            __mental_create_rectangle(pComponent);
            break;
        case MENTAL_COMPONENT_TYPE_TRIANGLE:
            __mental_create_triangle(pComponent);
            break;
        case MENTAL_COMPONENT_TYPE_GROUND:
            __mental_create_ground(pComponent);
            break;
        default:
            return MENTAL_UNKNOWN_COMPONENT_TYPE;
    }
    
    return MENTAL_OK;
}

MentalResult mentalDrawComponent(MentalComponent* pComponent, MentalWindowManager *pManager) {
    if (!pComponent) {
        MENTAL_DEBUG("Mental component is null.");
        return MENTAL_POINTER_IS_NULL;
    }
    
    if (pComponent->shaderProgram == 0) {
        MENTAL_DEBUG("Shader program is not initialized.");
        return MENTAL_ERROR;
    }
    
    if (pComponent->VAO == 0) {
        MENTAL_DEBUG("VAO is not initialized.");
        return MENTAL_ERROR;
    }
    
    // Get window size
    glfwGetWindowSize(pManager->pNext, &pManager->pInfo->aSizes[0], &pManager->pInfo->aSizes[1]);
    float aspect_ratio = (float)pManager->pInfo->aSizes[0] / (float)pManager->pInfo->aSizes[1];
    
    // Get matrices
    mat4 view, projection;
    mental_camera_get_view_matrix(&pManager->camera, view);
    mental_camera_get_projection_matrix(&pManager->camera, projection, aspect_ratio);
    
    glUseProgram(pComponent->shaderProgram);
    
    // Pass matrices to shader
    GLint viewLoc = glGetUniformLocation(pComponent->shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(pComponent->shaderProgram, "projection");
    if (viewLoc != -1) glUniformMatrix4fv(viewLoc, 1, GL_FALSE, (float*)view);
    if (projLoc != -1) glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float*)projection);
    
    // Pass other uniforms
    GLint modelLoc = glGetUniformLocation(pComponent->shaderProgram, "model");
    GLint resLoc = glGetUniformLocation(pComponent->shaderProgram, "resolution");
    
    // Create model matrix (position, rotation, scale)
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model, pComponent->position);
    glm_rotate(model, glm_rad(pComponent->rotation[0]), (vec3){1.0f, 0.0f, 0.0f});
    glm_rotate(model, glm_rad(pComponent->rotation[1]), (vec3){0.0f, 1.0f, 0.0f});
    glm_rotate(model, glm_rad(pComponent->rotation[2]), (vec3){0.0f, 0.0f, 1.0f});
    glm_scale_uni(model, pComponent->size);
    
    if (modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, (float*)model);
    if (resLoc != -1) glUniform2f(resLoc, (float)pManager->pInfo->aSizes[0], (float)pManager->pInfo->aSizes[1]);
    
    // Pass size uniform for fragment shader
    GLint sizeLoc = glGetUniformLocation(pComponent->shaderProgram, "size");
    if (sizeLoc != -1) glUniform1f(sizeLoc, pComponent->size);
    
    // Pass time uniform for cloud animation
    GLint timeLoc = glGetUniformLocation(pComponent->shaderProgram, "time");
    if (timeLoc != -1) glUniform1f(timeLoc, (float)glfwGetTime());
    
    // Draw the component
    switch(pComponent->eType) {
        case MENTAL_COMPONENT_TYPE_RECTANGLE:
            __mental_draw_rectangle(pComponent);
            break;
        case MENTAL_COMPONENT_TYPE_TRIANGLE:
            __mental_draw_triangle(pComponent);
            break;
        default:
            return MENTAL_UNKNOWN_COMPONENT_TYPE;
    }
    
    return MENTAL_OK;
}

MentalResult mentalDestroyComponent(MentalComponent *pComponent) {
    if (!pComponent) { return MENTAL_POINTER_IS_NULL; }

    if (pComponent->VAO != 0) {
        glDeleteVertexArrays(1, &pComponent->VAO);
        pComponent->VAO = 0;
    }
    if (pComponent->VBO != 0) {
        glDeleteBuffers(1, &pComponent->VBO);
        pComponent->VBO = 0;
    }
    if (pComponent->EBO != 0) {
        glDeleteBuffers(1, &pComponent->EBO);
        pComponent->EBO = 0;
    }
    if (pComponent->shaderProgram != 0) {
        glDeleteProgram(pComponent->shaderProgram);
        pComponent->shaderProgram = 0;
    }
    return MENTAL_OK;

}
void mental_camera_get_view_matrix(MentalCamera* cam, mat4 dest) {
    vec3 center;
    glm_vec3_add(cam->position, cam->front, center);
    glm_lookat(cam->position, center, cam->up, dest);
}

void mental_camera_get_projection_matrix(MentalCamera* cam, mat4 dest, float aspect_ratio) {
    glm_perspective(glm_rad(cam->zoom), aspect_ratio, 0.1f, 100.0f, dest);
}

void mental_camera_process_zoom(MentalCamera* cam, float yoffset) {
    cam->zoom -= yoffset;
    if (cam->zoom < 1.0f) cam->zoom = 1.0f;
    if (cam->zoom > 90.0f) cam->zoom = 90.0f;
}

void mental_camera_init(MentalCamera* cam, float pos_x, float pos_y, float pos_z) {
    // Initialize camera position
    glm_vec3_copy((vec3){pos_x, pos_y, pos_z}, cam->position);
    
    // World up vector
    glm_vec3_copy((vec3){0.0f, 1.0f, 0.0f}, cam->world_up);
    
    // Camera orientation
    cam->yaw = -90.0f;   // Default yaw (points to -Z)
    cam->pitch = 0.0f;   // Default pitch (level)
    
    // Camera movement settings
    cam->movement_speed = 2.5f;
    cam->mouse_sensitivity = 0.1f;
    cam->zoom = 45.0f;   // Field of view
    
    // Initialize camera vectors
    mental_camera_update_vectors(cam);
}

void mental_camera_update_vectors(MentalCamera* cam) {
    vec3 front = {
        cosf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch)),
        sinf(glm_rad(cam->pitch)),
        sinf(glm_rad(cam->yaw)) * cosf(glm_rad(cam->pitch))
    };
    glm_vec3_normalize_to(front, cam->front);
    
    glm_vec3_cross(cam->front, cam->world_up, cam->right);
    glm_vec3_normalize(cam->right);
    
    glm_vec3_cross(cam->right, cam->front, cam->up);
    glm_vec3_normalize(cam->up);
}

void mental_process_keyboard(MentalWindowManager* wm, float delta_time) {
    float velocity = wm->camera.movement_speed * delta_time;
    
    if (glfwGetKey(wm->pNext, GLFW_KEY_W) == GLFW_PRESS) {
        vec3 temp;
        glm_vec3_scale(wm->camera.front, velocity, temp);
        glm_vec3_add(wm->camera.position, temp, wm->camera.position);
    }
    if (glfwGetKey(wm->pNext, GLFW_KEY_S) == GLFW_PRESS) {
        vec3 temp;
        glm_vec3_scale(wm->camera.front, -velocity, temp);
        glm_vec3_add(wm->camera.position, temp, wm->camera.position);
    }
    if (glfwGetKey(wm->pNext, GLFW_KEY_A) == GLFW_PRESS) {
        vec3 temp;
        glm_vec3_scale(wm->camera.right, -velocity, temp);
        glm_vec3_add(wm->camera.position, temp, wm->camera.position);
    }
    if (glfwGetKey(wm->pNext, GLFW_KEY_D) == GLFW_PRESS) {
        vec3 temp;
        glm_vec3_scale(wm->camera.right, velocity, temp);
        glm_vec3_add(wm->camera.position, temp, wm->camera.position);
    }
}

void mental_process_mouse(MentalCamera* cam, float xoffset, float yoffset) {
    xoffset *= cam->mouse_sensitivity;
    yoffset *= cam->mouse_sensitivity;

    cam->yaw += xoffset;
    cam->pitch += yoffset;

    if (cam->pitch > 89.0f) cam->pitch = 89.0f;
    if (cam->pitch < -89.0f) cam->pitch = -89.0f;

    mental_camera_update_vectors(cam);
}

// Ground component functions
void __mental_create_ground(MentalComponent *pComponent) {
    // Создаем более детальную сетку для земли с горами
    const int gridSize = 50; // Размер сетки
    const float terrainSize = 20.0f; // Размер местности
    const float step = terrainSize / gridSize;
    
    // Вычисляем количество вершин и индексов
    int vertexCount = (gridSize + 1) * (gridSize + 1);
    int indexCount = gridSize * gridSize * 6;
    
    // Создаем массивы для вершин и индексов
    float* vertices = malloc(vertexCount * 3 * sizeof(float));
    unsigned int* indices = malloc(indexCount * sizeof(unsigned int));
    
    if (!vertices || !indices) {
        MENTAL_DEBUG("Failed to allocate memory for ground geometry");
        if (vertices) free(vertices);
        if (indices) free(indices);
        return;
    }
    
    // Инициализируем шум Перлина для генерации высоты
    mental_init_perlin();
    
    // Генерируем вершины с реальной высотой
    int vertexIndex = 0;
    for (int z = 0; z <= gridSize; z++) {
        for (int x = 0; x <= gridSize; x++) {
            float xPos = (x * step) - terrainSize * 0.5f;
            float zPos = (z * step) - terrainSize * 0.5f;
            
            // Генерируем высоту на основе шума Перлина
            float noiseX = xPos * 0.05f; // Уменьшаем масштаб для более крупных гор
            float noiseZ = zPos * 0.05f;
            
            // Генерируем несколько слоев шума для более реалистичного рельефа
            float height1 = mental_perlin_noise(noiseX, 0.0f, noiseZ) * 3.0f;
            float height2 = mental_perlin_noise(noiseX * 2.0f, 0.0f, noiseZ * 2.0f) * 1.5f;
            float height3 = mental_perlin_noise(noiseX * 4.0f, 0.0f, noiseZ * 4.0f) * 0.75f;
            
            float height = height1 + height2 + height3; // Комбинируем слои
            
            // Базовая высота земли
            float yPos = -1.0f + height;
            
            vertices[vertexIndex * 3] = xPos;
            vertices[vertexIndex * 3 + 1] = yPos;
            vertices[vertexIndex * 3 + 2] = zPos;
            
            vertexIndex++;
        }
    }
    
    // Генерируем индексы для треугольников
    int indexIndex = 0;
    for (int z = 0; z < gridSize; z++) {
        for (int x = 0; x < gridSize; x++) {
            int topLeft = z * (gridSize + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (gridSize + 1) + x;
            int bottomRight = bottomLeft + 1;
            
            // Первый треугольник
            indices[indexIndex++] = topLeft;
            indices[indexIndex++] = bottomLeft;
            indices[indexIndex++] = topRight;
            
            // Второй треугольник
            indices[indexIndex++] = topRight;
            indices[indexIndex++] = bottomLeft;
            indices[indexIndex++] = bottomRight;
        }
    }
    
    glGenVertexArrays(1, &pComponent->VAO);
    glGenBuffers(1, &pComponent->VBO);
    glGenBuffers(1, &pComponent->EBO);
    
    glBindVertexArray(pComponent->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, pComponent->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pComponent->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    
    // Освобождаем память
    free(vertices);
    free(indices);
    
    // Сохраняем количество индексов для отрисовки
    pComponent->indexCount = indexCount;
}

MentalResult mentalCreateGroundComponent(MentalComponent* pComponent) {
    if (!pComponent) {
        MENTAL_DEBUG("Ground component pointer is null.");
        return MENTAL_POINTER_IS_NULL;
    }

    // Инициализируем компонент
    pComponent->eType = MENTAL_COMPONENT_TYPE_GROUND;
    pComponent->position[0] = 0.0f;
    pComponent->position[1] = -1.0f; // Земля ниже уровня камеры
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

    // Создаем геометрию земли
    __mental_create_ground(pComponent);

    MENTAL_DEBUG("Ground component created successfully.");
    return MENTAL_OK;
}

MentalResult mentalLoadGroundTexture(MentalComponent* pComponent, const char* texture_path) {
    (void)texture_path; // Suppress unused parameter warning
    if (!pComponent) {
        MENTAL_DEBUG("Ground component pointer is null.");
        return MENTAL_POINTER_IS_NULL;
    }

    // Здесь можно добавить загрузку текстуры, если нужно
    // Пока используем цвет из шейдера
    MENTAL_DEBUG("Ground texture loading not implemented yet.");
    return MENTAL_OK;
}

MentalResult mentalDrawGroundComponent(MentalComponent* pComponent, MentalWindowManager *pManager) {
    if (!pComponent || !pManager) {
        MENTAL_DEBUG("Ground component or WindowManager pointer is null.");
        return MENTAL_POINTER_IS_NULL;
    }

    if (pComponent->shaderProgram == 0) {
        MENTAL_DEBUG("Ground component shader not attached.");
        return MENTAL_ERROR;
    }

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

    // Отрисовываем землю
    glBindVertexArray(pComponent->VAO);
    glDrawElements(GL_TRIANGLES, pComponent->indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    return MENTAL_OK;
}

