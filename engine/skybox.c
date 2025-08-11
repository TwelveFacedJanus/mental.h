#include "component.h"
#include "mental.h"
#include "wm.h"
#include <string.h>

// Вершины для куба скайбокса (все грани направлены внутрь)
static float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

MentalResult mentalCreateSkybox(MentalSkybox* pSkybox) {
    if (!pSkybox) {
        MENTAL_DEBUG("Skybox pointer is null.");
        return MENTAL_POINTER_IS_NULL;
    }

    // Инициализация структуры
    pSkybox->VAO = 0;
    pSkybox->VBO = 0;
    pSkybox->cubemapTexture = 0;
    pSkybox->shaderProgram = 0;
    pSkybox->isInitialized = false;

    // Создание VAO и VBO
    glGenVertexArrays(1, &pSkybox->VAO);
    glGenBuffers(1, &pSkybox->VBO);
    
    glBindVertexArray(pSkybox->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, pSkybox->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    pSkybox->isInitialized = true;
    MENTAL_DEBUG("Skybox created successfully.");
    return MENTAL_OK;
}

// Функция для загрузки PPM файла
unsigned char* loadPPM(const char* filename, int* width, int* height) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        MENTAL_DEBUG("Failed to open PPM file: %s", filename);
        return NULL;
    }
    
    char magic[3];
    int maxval;
    
    // Читаем заголовок PPM
    if (fscanf(file, "%2s", magic) != 1 || strcmp(magic, "P6") != 0) {
        MENTAL_DEBUG("Invalid PPM format in file: %s", filename);
        fclose(file);
        return NULL;
    }
    
    if (fscanf(file, "%d %d", width, height) != 2) {
        MENTAL_DEBUG("Failed to read dimensions from PPM file: %s", filename);
        fclose(file);
        return NULL;
    }
    
    if (fscanf(file, "%d", &maxval) != 1) {
        MENTAL_DEBUG("Failed to read maxval from PPM file: %s", filename);
        fclose(file);
        return NULL;
    }
    
    // Пропускаем один символ новой строки
    fgetc(file);
    
    // Выделяем память для данных
    unsigned char* data = malloc(*width * *height * 3);
    if (!data) {
        MENTAL_DEBUG("Failed to allocate memory for PPM data");
        fclose(file);
        return NULL;
    }
    
    // Читаем данные
    if (fread(data, 1, *width * *height * 3, file) != (size_t)(*width * *height * 3)) {
        MENTAL_DEBUG("Failed to read PPM data from file: %s", filename);
        free(data);
        fclose(file);
        return NULL;
    }
    
    fclose(file);
    return data;
}

// Функция для загрузки изображения в различных форматах
unsigned char* loadImage(const char* filename, int* width, int* height, int* channels) {
    // Проверяем расширение файла
    const char* ext = strrchr(filename, '.');
    if (!ext) {
        MENTAL_DEBUG("No file extension found: %s", filename);
        return NULL;
    }
    
    if (strcmp(ext, ".ppm") == 0) {
        // Загружаем PPM файл
        unsigned char* data = loadPPM(filename, width, height);
        if (data) {
            *channels = 3; // PPM всегда RGB
        }
        return data;
    } else {
        // Для других форматов можно добавить поддержку stb_image или других библиотек
        MENTAL_DEBUG("Unsupported image format: %s", ext);
        return NULL;
    }
}

uint32_t mentalLoadCubemap(const char* faces[6]) {
    uint32_t textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, channels;
    for (unsigned int i = 0; i < 6; i++) {
        unsigned char *data = loadImage(faces[i], &width, &height, &channels);
        if (data) {
            GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                        0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            free(data);
        } else {
            MENTAL_DEBUG("Cubemap texture failed to load at path: %s", faces[i]);
        }
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

MentalResult mentalLoadSkyboxTextures(MentalSkybox* pSkybox, const char* right, const char* left, 
                                     const char* top, const char* bottom, 
                                     const char* front, const char* back) {
    if (!pSkybox) {
        MENTAL_DEBUG("Skybox pointer is null.");
        return MENTAL_POINTER_IS_NULL;
    }

    const char* faces[6] = {right, left, top, bottom, front, back};
    pSkybox->cubemapTexture = mentalLoadCubemap(faces);
    
    MENTAL_DEBUG("Skybox textures loaded successfully.");
    return MENTAL_OK;
}

MentalResult mentalAttachSkyboxShader(MentalSkybox* pSkybox, const char* vertex_path, const char* fragment_path) {
    if (!pSkybox) {
        MENTAL_DEBUG("Skybox pointer is null.");
        return MENTAL_POINTER_IS_NULL;
    }

    // Чтение и компиляция шейдеров
    char* vertexCode = NULL;
    char* fragmentCode = NULL;
    
    // Читаем вершинный шейдер
    FILE* vShaderFile = fopen(vertex_path, "r");
    if (!vShaderFile) {
        MENTAL_DEBUG("Failed to open vertex shader file: %s", vertex_path);
        return MENTAL_FILE_OPEN_FAILED;
    }
    
    fseek(vShaderFile, 0, SEEK_END);
    long vFileSize = ftell(vShaderFile);
    fseek(vShaderFile, 0, SEEK_SET);
    
    vertexCode = malloc(vFileSize + 1);
    if (!vertexCode) {
        MENTAL_DEBUG("Failed to allocate memory for vertex shader");
        fclose(vShaderFile);
        return MENTAL_FAILED_TO_ALLOCATE_MEMORY;
    }
    
    fread(vertexCode, 1, vFileSize, vShaderFile);
    vertexCode[vFileSize] = '\0';
    fclose(vShaderFile);

    // Читаем фрагментный шейдер
    FILE* fShaderFile = fopen(fragment_path, "r");
    if (!fShaderFile) {
        MENTAL_DEBUG("Failed to open fragment shader file: %s", fragment_path);
        free(vertexCode);
        return MENTAL_FILE_OPEN_FAILED;
    }
    
    fseek(fShaderFile, 0, SEEK_END);
    long fFileSize = ftell(fShaderFile);
    fseek(fShaderFile, 0, SEEK_SET);
    
    fragmentCode = malloc(fFileSize + 1);
    if (!fragmentCode) {
        MENTAL_DEBUG("Failed to allocate memory for fragment shader");
        free(vertexCode);
        fclose(fShaderFile);
        return MENTAL_FAILED_TO_ALLOCATE_MEMORY;
    }
    
    fread(fragmentCode, 1, fFileSize, fShaderFile);
    fragmentCode[fFileSize] = '\0';
    fclose(fShaderFile);

    // Компиляция шейдеров
    uint32_t vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        MENTAL_DEBUG("Vertex shader compilation failed: %s", infoLog);
        return MENTAL_SHADER_COMPILE_FAILED;
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        MENTAL_DEBUG("Fragment shader compilation failed: %s", infoLog);
        return MENTAL_SHADER_COMPILE_FAILED;
    }

    // Линковка программы
    pSkybox->shaderProgram = glCreateProgram();
    glAttachShader(pSkybox->shaderProgram, vertex);
    glAttachShader(pSkybox->shaderProgram, fragment);
    glLinkProgram(pSkybox->shaderProgram);
    glGetProgramiv(pSkybox->shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(pSkybox->shaderProgram, 512, NULL, infoLog);
        MENTAL_DEBUG("Shader program linking failed: %s", infoLog);
        return MENTAL_SHADER_LINK_FAILED;
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // Освобождаем память
    free(vertexCode);
    free(fragmentCode);

    MENTAL_DEBUG("Skybox shader attached successfully.");
    return MENTAL_OK;
}

MentalResult mentalDrawSkybox(MentalSkybox* pSkybox, MentalWindowManager *pManager) {
    if (!pSkybox || !pManager) {
        MENTAL_DEBUG("Skybox or WindowManager pointer is null.");
        return MENTAL_POINTER_IS_NULL;
    }

    if (!pSkybox->isInitialized || pSkybox->shaderProgram == 0 || pSkybox->cubemapTexture == 0) {
        MENTAL_DEBUG("Skybox not properly initialized.");
        return MENTAL_ERROR;
    }

    // Сохраняем текущее состояние глубины
    glDepthFunc(GL_LEQUAL);
    
    glUseProgram(pSkybox->shaderProgram);
    
    // Убираем трансляцию из view матрицы для скайбокса
    mat4 view;
    mental_camera_get_view_matrix(&pManager->camera, view);
    view[3][0] = 0.0f;
    view[3][1] = 0.0f;
    view[3][2] = 0.0f;
    
    mat4 projection;
    mental_camera_get_projection_matrix(&pManager->camera, projection, 
                                       (float)pManager->pInfo->aSizes[0] / pManager->pInfo->aSizes[1]);
    
    glUniformMatrix4fv(glGetUniformLocation(pSkybox->shaderProgram, "view"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(pSkybox->shaderProgram, "projection"), 1, GL_FALSE, (float*)projection);
    
    glBindVertexArray(pSkybox->VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, pSkybox->cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    
    // Восстанавливаем настройки глубины
    glDepthFunc(GL_LESS);
    
    return MENTAL_OK;
}

MentalResult mentalDestroySkybox(MentalSkybox *pSkybox) {
    if (!pSkybox) {
        MENTAL_DEBUG("Skybox pointer is null.");
        return MENTAL_POINTER_IS_NULL;
    }

    if (pSkybox->VAO != 0) {
        glDeleteVertexArrays(1, &pSkybox->VAO);
        pSkybox->VAO = 0;
    }
    
    if (pSkybox->VBO != 0) {
        glDeleteBuffers(1, &pSkybox->VBO);
        pSkybox->VBO = 0;
    }
    
    if (pSkybox->cubemapTexture != 0) {
        glDeleteTextures(1, &pSkybox->cubemapTexture);
        pSkybox->cubemapTexture = 0;
    }
    
    if (pSkybox->shaderProgram != 0) {
        glDeleteProgram(pSkybox->shaderProgram);
        pSkybox->shaderProgram = 0;
    }
    
    pSkybox->isInitialized = false;
    MENTAL_DEBUG("Skybox destroyed successfully.");
    return MENTAL_OK;
}
