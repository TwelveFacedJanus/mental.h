#include "mental.h"
#include "component.h"
#include "wm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/glew.h>
#include <cglm/cglm.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>

// Включаем stb_image для загрузки текстур
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

// ============================
// Расширенная отладка OpenGL
// ============================
static const char* mental_gl_error_to_string(GLenum err)
{
    switch (err) {
        case GL_NO_ERROR: return "GL_NO_ERROR";
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
#ifdef GL_STACK_OVERFLOW
        case GL_STACK_OVERFLOW: return "GL_STACK_OVERFLOW";
#endif
#ifdef GL_STACK_UNDERFLOW
        case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW";
#endif
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
#ifdef GL_INVALID_FRAMEBUFFER_OPERATION
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
#endif
        default: return "GL_UNKNOWN_ERROR";
    }
}

static void mental_gl_check(const char* where)
{
    GLenum err;
    bool had_error = false;
    while ((err = glGetError()) != GL_NO_ERROR) {
        had_error = true;
        MENTAL_DEBUG("[GL] %s -> 0x%X (%s)", where, err, mental_gl_error_to_string(err));
    }
    if (!had_error) {
        MENTAL_DEBUG("[GL] %s -> OK", where);
    }
}

static void mental_log_model_stats(const Model3DData* d)
{
    if (!d) return;
    MENTAL_DEBUG("Model stats: vertices=%u, indices=%u", d->vertexCount, d->indexCount);
    if (d->vertexCount == 0) return;

    // Печать первых значений
    for (int i = 0; i < 3 && (unsigned)i < d->vertexCount; i++) {
        const float *v = &d->vertices[i*3];
        const float *n = &d->normals[i*3];
        const float *t = &d->texCoords[i*2];
        MENTAL_DEBUG("v[%d]=(%g,%g,%g) n[%d]=(%g,%g,%g) uv[%d]=(%g,%g)",
                     i, v[0], v[1], v[2], i, n[0], n[1], n[2], i, t[0], t[1]);
    }

    // Статистика UV
    float u_min = 1e9f, u_max = -1e9f, v_min = 1e9f, v_max = -1e9f;
    unsigned out_of_01 = 0U;
    for (unsigned i = 0; i < d->vertexCount; i++) {
        float u = d->texCoords[i*2+0];
        float v = d->texCoords[i*2+1];
        if (isnan(u) || isnan(v) || isinf(u) || isinf(v)) {
            MENTAL_DEBUG("UV[%u] is NaN/Inf: (%g,%g)", i, u, v);
        }
        if (u < 0.f || u > 1.f || v < 0.f || v > 1.f) out_of_01++;
        if (u < u_min) u_min = u; if (u > u_max) u_max = u;
        if (v < v_min) v_min = v; if (v > v_max) v_max = v;
    }
    MENTAL_DEBUG("UV range: u=[%g,%g] v=[%g,%g], out_of_[0,1]=%u", u_min, u_max, v_min, v_max, out_of_01);

    if (d->indexCount % 3 != 0) {
        MENTAL_DEBUG("WARN: indexCount (%u) is not divisible by 3", d->indexCount);
    }
    for (int i = 0; i < 6 && (unsigned)i < d->indexCount; i+=3) {
        unsigned a = d->indices[i+0], b = d->indices[i+1], c = d->indices[i+2];
        MENTAL_DEBUG("tri[%d] = (%u,%u,%u)", i/3, a, b, c);
    }
}

// Максимальное количество вершин, которое можно загрузить из OBJ файла
#define MAX_VERTICES 50000
#define MAX_FACES 50000

// Структура для временного хранения данных при загрузке OBJ файла
typedef struct {
    float positions[MAX_VERTICES][3];
    float texcoords[MAX_VERTICES][2];
    float normals[MAX_VERTICES][3];
    int face_positions[MAX_FACES][3];
    int face_texcoords[MAX_FACES][3];
    int face_normals[MAX_FACES][3];
    int position_count;
    int texcoord_count;
    int normal_count;
    int face_count;
} OBJData;

// Функция для загрузки OBJ файла
static MentalResult loadOBJFile(const char* filename, Model3DData* modelData) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        MENTAL_DEBUG("Failed to open OBJ file: %s", filename);
        return MENTAL_FILE_OPEN_FAILED;
    }
    
    OBJData objData = {0};
    
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Удаляем символ новой строки
        line[strcspn(line, "\n")] = 0;
        
        if (strncmp(line, "v ", 2) == 0) {
            // Вершина
            if (objData.position_count >= MAX_VERTICES) {
                MENTAL_DEBUG("Too many vertices in OBJ file");
                fclose(file);
                return MENTAL_ERROR;
            }
            
            sscanf(line, "v %f %f %f", 
                   &objData.positions[objData.position_count][0],
                   &objData.positions[objData.position_count][1],
                   &objData.positions[objData.position_count][2]);
            objData.position_count++;
        }
        else if (strncmp(line, "vt ", 3) == 0) {
            // Текстурная координата
            if (objData.texcoord_count >= MAX_VERTICES) {
                MENTAL_DEBUG("Too many texture coordinates in OBJ file");
                fclose(file);
                return MENTAL_ERROR;
            }
            
            sscanf(line, "vt %f %f", 
                   &objData.texcoords[objData.texcoord_count][0],
                   &objData.texcoords[objData.texcoord_count][1]);
            objData.texcoord_count++;
        }
        else if (strncmp(line, "vn ", 3) == 0) {
            // Нормаль
            if (objData.normal_count >= MAX_VERTICES) {
                MENTAL_DEBUG("Too many normals in OBJ file");
                fclose(file);
                return MENTAL_ERROR;
            }
            
            sscanf(line, "vn %f %f %f", 
                   &objData.normals[objData.normal_count][0],
                   &objData.normals[objData.normal_count][1],
                   &objData.normals[objData.normal_count][2]);
            objData.normal_count++;
        }
        else if (strncmp(line, "f ", 2) == 0) {
            // Грань (треугольник)
            if (objData.face_count >= MAX_FACES) {
                MENTAL_DEBUG("Too many faces in OBJ file");
                fclose(file);
                return MENTAL_ERROR;
            }
            
            // Поддерживаем форматы: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
            // или f v1//vn1 v2//vn2 v3//vn3
            // или f v1/vt1 v2/vt2 v3/vt3
            // или f v1 v2 v3
            int v[3], vt[3] = {0}, vn[3] = {0};
            char* formats[] = {
                "f %d/%d/%d %d/%d/%d %d/%d/%d", // v/vt/vn
                "f %d//%d %d//%d %d//%d",       // v//vn
                "f %d/%d %d/%d %d/%d",          // v/vt
                "f %d %d %d"                    // v
            };
            
            bool parsed = false;
            for (int i = 0; i < 4 && !parsed; i++) {
                switch (i) {
                    case 0: // v/vt/vn
                        if (sscanf(line, formats[i], &v[0], &vt[0], &vn[0], &v[1], &vt[1], &vn[1], &v[2], &vt[2], &vn[2]) == 9) {
                            parsed = true;
                        }
                        break;
                    case 1: // v//vn
                        if (sscanf(line, formats[i], &v[0], &vn[0], &v[1], &vn[1], &v[2], &vn[2]) == 6) {
                            parsed = true;
                            vt[0] = vt[1] = vt[2] = 0;
                        }
                        break;
                    case 2: // v/vt
                        if (sscanf(line, formats[i], &v[0], &vt[0], &v[1], &vt[1], &v[2], &vt[2]) == 6) {
                            parsed = true;
                            vn[0] = vn[1] = vn[2] = 0;
                        }
                        break;
                    case 3: // v
                        if (sscanf(line, formats[i], &v[0], &v[1], &v[2]) == 3) {
                            parsed = true;
                            vt[0] = vt[1] = vt[2] = 0;
                            vn[0] = vn[1] = vn[2] = 0;
                        }
                        break;
                }
            }
            
            if (!parsed) {
                MENTAL_DEBUG("Failed to parse face: %s", line);
                continue;
            }
            
            // OBJ индексы начинаются с 1, а не с 0
            for (int i = 0; i < 3; i++) {
                objData.face_positions[objData.face_count][i] = v[i] - 1;
                objData.face_texcoords[objData.face_count][i] = vt[i] - 1;
                objData.face_normals[objData.face_count][i] = vn[i] - 1;
            }
            
            objData.face_count++;
        }
    }
    
    fclose(file);
    
    // Теперь создаем массивы для OpenGL
    // Для каждой вершины треугольника нам нужны позиция, текстурные координаты и нормаль
    modelData->vertexCount = objData.face_count * 3;
    modelData->indexCount = objData.face_count * 3;
    
    // Выделяем память для данных
    modelData->vertices = (float*)malloc(modelData->vertexCount * 3 * sizeof(float));
    modelData->texCoords = (float*)malloc(modelData->vertexCount * 2 * sizeof(float));
    modelData->normals = (float*)malloc(modelData->vertexCount * 3 * sizeof(float));
    modelData->indices = (unsigned int*)malloc(modelData->indexCount * sizeof(unsigned int));
    
    if (!modelData->vertices || !modelData->texCoords || !modelData->normals || !modelData->indices) {
        MENTAL_DEBUG("Failed to allocate memory for model data");
        free(modelData->vertices);
        free(modelData->texCoords);
        free(modelData->normals);
        free(modelData->indices);
        return MENTAL_FAILED_TO_ALLOCATE_MEMORY;
    }
    
    // Заполняем массивы данными
    for (int i = 0; i < objData.face_count; i++) {
        for (int j = 0; j < 3; j++) {
            int vertexIndex = i * 3 + j;
            int posIndex = objData.face_positions[i][j];
            int texIndex = objData.face_texcoords[i][j];
            int normIndex = objData.face_normals[i][j];
            
            // Позиция вершины
            modelData->vertices[vertexIndex * 3 + 0] = objData.positions[posIndex][0];
            modelData->vertices[vertexIndex * 3 + 1] = objData.positions[posIndex][1];
            modelData->vertices[vertexIndex * 3 + 2] = objData.positions[posIndex][2];
            
            // Текстурные координаты (если есть)
            if (objData.texcoord_count > 0 && texIndex >= 0) {
                modelData->texCoords[vertexIndex * 2 + 0] = objData.texcoords[texIndex][0];
                modelData->texCoords[vertexIndex * 2 + 1] = objData.texcoords[texIndex][1];
            } else {
                modelData->texCoords[vertexIndex * 2 + 0] = 0.0f;
                modelData->texCoords[vertexIndex * 2 + 1] = 0.0f;
            }
            
            // Нормали (если есть)
            if (objData.normal_count > 0 && normIndex >= 0) {
                modelData->normals[vertexIndex * 3 + 0] = objData.normals[normIndex][0];
                modelData->normals[vertexIndex * 3 + 1] = objData.normals[normIndex][1];
                modelData->normals[vertexIndex * 3 + 2] = objData.normals[normIndex][2];
            } else {
                // Если нормалей нет, используем нулевой вектор (потом можно вычислить)
                modelData->normals[vertexIndex * 3 + 0] = 0.0f;
                modelData->normals[vertexIndex * 3 + 1] = 0.0f;
                modelData->normals[vertexIndex * 3 + 2] = 1.0f;
            }
            
            // Индексы (просто последовательные числа, так как мы уже развернули данные)
            modelData->indices[vertexIndex] = vertexIndex;
        }
    }
    
    MENTAL_DEBUG("Loaded OBJ model with %d vertices, %d faces", objData.position_count, objData.face_count);
    return MENTAL_OK;
}

// Функция для загрузки текстуры модели
static MentalResult loadModelTexture(const char* filename, uint32_t* textureID) {
    int width, height, nrChannels;
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
    
    if (!data) {
        MENTAL_DEBUG("Failed to load texture: %s", filename);
        return MENTAL_FILE_OPEN_FAILED;
    }
    
    // Создаем текстуру OpenGL
    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, *textureID);
    
    // Устанавливаем параметры текстуры
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Определяем формат текстуры в зависимости от количества каналов
    GLenum format;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    else {
        MENTAL_DEBUG("Unsupported image format: %d channels", nrChannels);
        stbi_image_free(data);
        return MENTAL_ERROR;
    }
    
    // Загружаем данные изображения в текстуру
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Освобождаем память
    stbi_image_free(data);
    
    MENTAL_DEBUG("Texture loaded successfully: %s", filename);
    return MENTAL_OK;
}

// Функция для загрузки PBR шейдеров
MentalResult mentalAttachPBRShader(MentalComponent* pComponent) {
    if (!pComponent) {
        MENTAL_DEBUG("Component pointer is null");
        return MENTAL_POINTER_IS_NULL;
    }
    
    // Путь к PBR шейдерам
    const char* vertex_path = "/Users/twofaced/Documents/Projects/mental.h/pbr_vertex.glsl";
    const char* fragment_path = "/Users/twofaced/Documents/Projects/mental.h/pbr_fragment.glsl";
    
    // Используем существующую функцию для прикрепления шейдеров
    return mentalAttachShader(pComponent, vertex_path, fragment_path);
}

// Функция для установки PBR материала
MentalResult mentalSetModelPBRMaterial(MentalComponent* pComponent, vec3 albedo, float metallic, float roughness, float ao) {
    if (!pComponent) {
        MENTAL_DEBUG("Component pointer is null");
        return MENTAL_POINTER_IS_NULL;
    }
    
    if (pComponent->eType != MENTAL_COMPONENT_TYPE_MODEL3D || !pComponent->modelData) {
        MENTAL_DEBUG("Component is not a 3D model or modelData is NULL");
        return MENTAL_UNKNOWN_COMPONENT_TYPE;
    }
    
    // Устанавливаем PBR параметры материала
    glm_vec3_copy(albedo, pComponent->modelData->material.albedo);
    pComponent->modelData->material.metallic = metallic;
    pComponent->modelData->material.roughness = roughness;
    pComponent->modelData->material.ao = ao;
    
    // Включаем режим PBR
    pComponent->modelData->material.use_pbr = true;
    
    MENTAL_DEBUG("PBR material set successfully");
    return MENTAL_OK;
}

// Создание компонента 3D модели
MentalResult mentalCreateModel3DComponent(MentalComponent* pComponent) {
    if (!pComponent) {
        return MENTAL_POINTER_IS_NULL;
    }
    
    pComponent->eType = MENTAL_COMPONENT_TYPE_MODEL3D;
    
    // Инициализируем позицию, размер и поворот
    pComponent->position[0] = 0.0f;
    pComponent->position[1] = 0.0f;
    pComponent->position[2] = 0.0f;
    pComponent->size = 1.0f;
    pComponent->rotation[0] = 0.0f;
    pComponent->rotation[1] = 0.0f;
    pComponent->rotation[2] = 0.0f;
    
    // Выделяем память для данных модели
    pComponent->modelData = (Model3DData*)malloc(sizeof(Model3DData));
    if (!pComponent->modelData) {
        MENTAL_DEBUG("Failed to allocate memory for model data");
        return MENTAL_FAILED_TO_ALLOCATE_MEMORY;
    }
    
    // Инициализируем данные модели
    memset(pComponent->modelData, 0, sizeof(Model3DData));
    
    // Инициализируем материал по умолчанию (традиционный)
    glm_vec3_copy((vec3){0.1f, 0.1f, 0.1f}, pComponent->modelData->material.ambient);
    glm_vec3_copy((vec3){0.5f, 0.5f, 0.5f}, pComponent->modelData->material.diffuse);
    glm_vec3_copy((vec3){1.0f, 1.0f, 1.0f}, pComponent->modelData->material.specular);
    pComponent->modelData->material.shininess = 32.0f;
    
    // Инициализируем PBR материал по умолчанию
    glm_vec3_copy((vec3){0.5f, 0.5f, 0.5f}, pComponent->modelData->material.albedo);
    pComponent->modelData->material.metallic = 0.0f;
    pComponent->modelData->material.roughness = 0.5f;
    pComponent->modelData->material.ao = 1.0f;
    pComponent->modelData->material.use_pbr = false; // По умолчанию используем традиционный материал
    
    // Создаем VAO, VBO и EBO
    glGenVertexArrays(1, &pComponent->VAO);
    glGenBuffers(1, &pComponent->VBO);
    glGenBuffers(1, &pComponent->EBO);
    
    MENTAL_DEBUG("Model3D component created successfully");
    return MENTAL_OK;
}

// Загрузка 3D модели из файла
MentalResult mentalLoadModel3D(MentalComponent* pComponent, const char* model_path) {
    if (!pComponent || !model_path) {
        return MENTAL_POINTER_IS_NULL;
    }
    
    if (pComponent->eType != MENTAL_COMPONENT_TYPE_MODEL3D || !pComponent->modelData) {
        MENTAL_DEBUG("Component is not a 3D model or modelData is NULL");
        return MENTAL_UNKNOWN_COMPONENT_TYPE;
    }
    
    // Загружаем модель из OBJ файла
    MentalResult result = loadOBJFile(model_path, pComponent->modelData);
    if (result != MENTAL_OK) {
        MENTAL_DEBUG("Failed to load OBJ file: %s", model_path);
        return result;
    }
    
    // Инициализируем флаги наличия текстур
    pComponent->modelData->hasTexture = false;
    pComponent->modelData->hasNormalMap = false;
    pComponent->modelData->hasMetallicMap = false;
    pComponent->modelData->hasRoughnessMap = false;
    pComponent->modelData->hasAOMap = false;
    pComponent->modelData->hasHeightMap = false;
    
    // Базовый путь для текстур
    char base_path[256];
    strcpy(base_path, model_path);
    char* ext = strrchr(base_path, '.');
    if (ext) {
        *ext = '\0'; // Обрезаем расширение
    }
    
    // Проверяем наличие базовой текстуры (альбедо/диффузной)
    char texture_path[256];
    
    // Проверяем альбедо текстуру (для PBR)
    sprintf(texture_path, "%s_albedo.png", base_path);
    FILE* test = fopen(texture_path, "r");
    if (test) {
        fclose(test);
        MentalResult texResult = loadModelTexture(texture_path, &pComponent->modelData->texture);
        if (texResult == MENTAL_OK) {
            pComponent->modelData->hasTexture = true;
            pComponent->modelData->material.use_pbr = true; // Если есть PBR текстуры, включаем PBR режим
        }
    } else {
        sprintf(texture_path, "%s_albedo.jpg", base_path);
        test = fopen(texture_path, "r");
        if (test) {
            fclose(test);
            MentalResult texResult = loadModelTexture(texture_path, &pComponent->modelData->texture);
            if (texResult == MENTAL_OK) {
                pComponent->modelData->hasTexture = true;
                pComponent->modelData->material.use_pbr = true;
            }
        } else {
            // Проверяем обычную диффузную текстуру (для традиционного рендеринга)
            sprintf(texture_path, "%s.png", base_path);
            test = fopen(texture_path, "r");
            if (test) {
                fclose(test);
                MentalResult texResult = loadModelTexture(texture_path, &pComponent->modelData->texture);
                if (texResult == MENTAL_OK) {
                    pComponent->modelData->hasTexture = true;
                }
            } else {
                sprintf(texture_path, "%s.jpg", base_path);
                test = fopen(texture_path, "r");
                if (test) {
                    fclose(test);
                    MentalResult texResult = loadModelTexture(texture_path, &pComponent->modelData->texture);
                    if (texResult == MENTAL_OK) {
                        pComponent->modelData->hasTexture = true;
                    }
                }
            }
        }
    }
    
    // Если текстура не загружена, создаем пустую текстуру для предотвращения ошибок
    if (!pComponent->modelData->hasTexture) {
        // Создаем пустую текстуру 1x1 пиксель белого цвета
        glGenTextures(1, &pComponent->modelData->texture);
        glBindTexture(GL_TEXTURE_2D, pComponent->modelData->texture);
        
        // Создаем белый пиксель
        unsigned char whitePixel[] = {255, 255, 255, 255};
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // Устанавливаем флаг, что текстура есть (пустая, но есть)
        pComponent->modelData->hasTexture = true;
        MENTAL_DEBUG("Created default white texture for model without texture");
    }
    
    // Проверяем карту нормалей
    sprintf(texture_path, "%s_normal.png", base_path);
    test = fopen(texture_path, "r");
    if (test) {
        fclose(test);
        MentalResult texResult = loadModelTexture(texture_path, &pComponent->modelData->normal_map);
        if (texResult == MENTAL_OK) {
            pComponent->modelData->hasNormalMap = true;
            pComponent->modelData->material.use_pbr = true;
        }
    } else {
        sprintf(texture_path, "%s_normal.jpg", base_path);
        test = fopen(texture_path, "r");
        if (test) {
            fclose(test);
            MentalResult texResult = loadModelTexture(texture_path, &pComponent->modelData->normal_map);
            if (texResult == MENTAL_OK) {
                pComponent->modelData->hasNormalMap = true;
                pComponent->modelData->material.use_pbr = true;
            }
        }
    }
    
    // Проверяем карту металличности
    sprintf(texture_path, "%s_metallic.png", base_path);
    test = fopen(texture_path, "r");
    if (test) {
        fclose(test);
        MentalResult texResult = loadModelTexture(texture_path, &pComponent->modelData->metallic_map);
        if (texResult == MENTAL_OK) {
            pComponent->modelData->hasMetallicMap = true;
            pComponent->modelData->material.use_pbr = true;
        }
    } else {
        sprintf(texture_path, "%s_metallic.jpg", base_path);
        test = fopen(texture_path, "r");
        if (test) {
            fclose(test);
            MentalResult texResult = loadModelTexture(texture_path, &pComponent->modelData->metallic_map);
            if (texResult == MENTAL_OK) {
                pComponent->modelData->hasMetallicMap = true;
                pComponent->modelData->material.use_pbr = true;
            }
        }
    }
    
    // Проверяем карту шероховатости
    sprintf(texture_path, "%s_roughness.png", base_path);
    test = fopen(texture_path, "r");
    if (test) {
        fclose(test);
        MentalResult texResult = loadModelTexture(texture_path, &pComponent->modelData->roughness_map);
        if (texResult == MENTAL_OK) {
            pComponent->modelData->hasRoughnessMap = true;
            pComponent->modelData->material.use_pbr = true;
        }
    } else {
        sprintf(texture_path, "%s_roughness.jpg", base_path);
        test = fopen(texture_path, "r");
        if (test) {
            fclose(test);
            MentalResult texResult = loadModelTexture(texture_path, &pComponent->modelData->roughness_map);
            if (texResult == MENTAL_OK) {
                pComponent->modelData->hasRoughnessMap = true;
                pComponent->modelData->material.use_pbr = true;
            }
        }
    }
    
    // Проверяем карту ambient occlusion
    sprintf(texture_path, "%s_ao.png", base_path);
    test = fopen(texture_path, "r");
    if (test) {
        fclose(test);
        MentalResult texResult = loadModelTexture(texture_path, &pComponent->modelData->ao_map);
        if (texResult == MENTAL_OK) {
            pComponent->modelData->hasAOMap = true;
            pComponent->modelData->material.use_pbr = true;
        }
    } else {
        sprintf(texture_path, "%s_ao.jpg", base_path);
        test = fopen(texture_path, "r");
        if (test) {
            fclose(test);
            MentalResult texResult = loadModelTexture(texture_path, &pComponent->modelData->ao_map);
            if (texResult == MENTAL_OK) {
                pComponent->modelData->hasAOMap = true;
                pComponent->modelData->material.use_pbr = true;
            }
        }
    }
    
    // Загружаем данные в буферы OpenGL
    glBindVertexArray(pComponent->VAO);
    
    // Позиции вершин
    glBindBuffer(GL_ARRAY_BUFFER, pComponent->VBO);
    glBufferData(GL_ARRAY_BUFFER, 
                 pComponent->modelData->vertexCount * 3 * sizeof(float) + // Позиции
                 pComponent->modelData->vertexCount * 2 * sizeof(float) + // Текстурные координаты
                 pComponent->modelData->vertexCount * 3 * sizeof(float),  // Нормали
                 NULL, GL_STATIC_DRAW);
    
    // Загружаем позиции вершин
    glBufferSubData(GL_ARRAY_BUFFER, 0, 
                    pComponent->modelData->vertexCount * 3 * sizeof(float), 
                    pComponent->modelData->vertices);
    
    // Загружаем текстурные координаты
    glBufferSubData(GL_ARRAY_BUFFER, 
                    pComponent->modelData->vertexCount * 3 * sizeof(float),
                    pComponent->modelData->vertexCount * 2 * sizeof(float),
                    pComponent->modelData->texCoords);
    
    // Загружаем нормали
    glBufferSubData(GL_ARRAY_BUFFER, 
                    pComponent->modelData->vertexCount * 3 * sizeof(float) + 
                    pComponent->modelData->vertexCount * 2 * sizeof(float),
                    pComponent->modelData->vertexCount * 3 * sizeof(float),
                    pComponent->modelData->normals);
    
    // Настраиваем атрибуты вершин
    // Позиции (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Текстурные координаты (location = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 
                          (void*)(pComponent->modelData->vertexCount * 3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Нормали (location = 2)
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 
                          (void*)(pComponent->modelData->vertexCount * 3 * sizeof(float) + 
                                 pComponent->modelData->vertexCount * 2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // Загружаем индексы
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pComponent->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 pComponent->modelData->indexCount * sizeof(unsigned int),
                 pComponent->modelData->indices, GL_STATIC_DRAW);
    
    // Сохраняем количество индексов для отрисовки
    pComponent->indexCount = pComponent->modelData->indexCount;
    
    // Отвязываем VAO
    glBindVertexArray(0);
    
    MENTAL_DEBUG("Model3D loaded successfully: %s", model_path);
    return MENTAL_OK;
}

// Отрисовка 3D модели
MentalResult mentalDrawModel3DComponent(MentalComponent* pComponent, MentalWindowManager *pManager) {
    if (!pComponent || !pManager) {
        return MENTAL_POINTER_IS_NULL;
    }
    
    if (pComponent->eType != MENTAL_COMPONENT_TYPE_MODEL3D || !pComponent->modelData) {
        MENTAL_DEBUG("Component is not a 3D model or modelData is NULL");
        return MENTAL_UNKNOWN_COMPONENT_TYPE;
    }
    
    // Используем шейдерную программу
    glUseProgram(pComponent->shaderProgram);
    
    // Получаем матрицы вида и проекции из менеджера окон
    mat4 view, projection;
    mental_camera_get_view_matrix(&pManager->camera, view);
    // Вычисляем соотношение сторон из размеров окна
    float aspect_ratio = (float)pManager->pInfo->aSizes[0] / (float)pManager->pInfo->aSizes[1];
    mental_camera_get_projection_matrix(&pManager->camera, projection, aspect_ratio);
    
    // Создаем матрицу модели
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    
    // Применяем трансформации: перемещение, масштабирование и поворот
    glm_translate(model, (vec3){pComponent->position[0], pComponent->position[1], pComponent->position[2]});
    
    // Поворот по X, Y, Z
    glm_rotate(model, glm_rad(pComponent->rotation[0]), (vec3){1.0f, 0.0f, 0.0f});
    glm_rotate(model, glm_rad(pComponent->rotation[1]), (vec3){0.0f, 1.0f, 0.0f});
    glm_rotate(model, glm_rad(pComponent->rotation[2]), (vec3){0.0f, 0.0f, 1.0f});
    
    // Масштабирование
    glm_scale_uni(model, pComponent->size);
    
    // Передаем матрицы в шейдер
    glUniformMatrix4fv(glGetUniformLocation(pComponent->shaderProgram, "model"), 1, GL_FALSE, (float*)model);
    glUniformMatrix4fv(glGetUniformLocation(pComponent->shaderProgram, "view"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(pComponent->shaderProgram, "projection"), 1, GL_FALSE, (float*)projection);
    
    // Устанавливаем параметры освещения
    glUniform3f(glGetUniformLocation(pComponent->shaderProgram, "lightPos"), 1.2f, 1.0f, 2.0f);
    glUniform3f(glGetUniformLocation(pComponent->shaderProgram, "viewPos"), 
                pManager->camera.position[0], 
                pManager->camera.position[1], 
                pManager->camera.position[2]);
    glUniform3f(glGetUniformLocation(pComponent->shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
    
    // Передаем флаг использования PBR материала
    glUniform1i(glGetUniformLocation(pComponent->shaderProgram, "usePBR"), pComponent->modelData->material.use_pbr);
    
    if (pComponent->modelData->material.use_pbr) {
        // Устанавливаем параметры PBR материала
        glUniform3fv(glGetUniformLocation(pComponent->shaderProgram, "material.albedo"), 1, pComponent->modelData->material.albedo);
        glUniform1f(glGetUniformLocation(pComponent->shaderProgram, "material.metallic"), pComponent->modelData->material.metallic);
        glUniform1f(glGetUniformLocation(pComponent->shaderProgram, "material.roughness"), pComponent->modelData->material.roughness);
        glUniform1f(glGetUniformLocation(pComponent->shaderProgram, "material.ao"), pComponent->modelData->material.ao);
        
        // Передаем флаги наличия PBR текстур
        glUniform1i(glGetUniformLocation(pComponent->shaderProgram, "hasAlbedoMap"), pComponent->modelData->hasTexture);
        glUniform1i(glGetUniformLocation(pComponent->shaderProgram, "hasNormalMap"), pComponent->modelData->hasNormalMap);
        glUniform1i(glGetUniformLocation(pComponent->shaderProgram, "hasMetallicMap"), pComponent->modelData->hasMetallicMap);
        glUniform1i(glGetUniformLocation(pComponent->shaderProgram, "hasRoughnessMap"), pComponent->modelData->hasRoughnessMap);
        glUniform1i(glGetUniformLocation(pComponent->shaderProgram, "hasAOMap"), pComponent->modelData->hasAOMap);
        glUniform1i(glGetUniformLocation(pComponent->shaderProgram, "hasHeightMap"), pComponent->modelData->hasHeightMap);
        
        // Активируем текстуры для PBR
        int textureUnit = 0;
        
        // Альбедо карта (базовая текстура)
        if (pComponent->modelData->hasTexture) {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, pComponent->modelData->texture);
            glUniform1i(glGetUniformLocation(pComponent->shaderProgram, "albedoMap"), textureUnit);
            textureUnit++;
        }
        
        // Карта нормалей
        if (pComponent->modelData->hasNormalMap) {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, pComponent->modelData->normal_map);
            glUniform1i(glGetUniformLocation(pComponent->shaderProgram, "normalMap"), textureUnit);
            textureUnit++;
        }
        
        // Карта металличности
        if (pComponent->modelData->hasMetallicMap) {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, pComponent->modelData->metallic_map);
            glUniform1i(glGetUniformLocation(pComponent->shaderProgram, "metallicMap"), textureUnit);
            textureUnit++;
        }
        
        // Карта шероховатости
        if (pComponent->modelData->hasRoughnessMap) {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, pComponent->modelData->roughness_map);
            glUniform1i(glGetUniformLocation(pComponent->shaderProgram, "roughnessMap"), textureUnit);
            textureUnit++;
        }
        
        // Карта ambient occlusion
        if (pComponent->modelData->hasAOMap) {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, pComponent->modelData->ao_map);
            glUniform1i(glGetUniformLocation(pComponent->shaderProgram, "aoMap"), textureUnit);
            textureUnit++;
        }
        
        // Карта высот
        if (pComponent->modelData->hasHeightMap) {
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, pComponent->modelData->height_map);
            glUniform1i(glGetUniformLocation(pComponent->shaderProgram, "heightMap"), textureUnit);
        }
    } else {
        // Устанавливаем параметры традиционного материала
        glUniform3fv(glGetUniformLocation(pComponent->shaderProgram, "material.ambient"), 1, pComponent->modelData->material.ambient);
        glUniform3fv(glGetUniformLocation(pComponent->shaderProgram, "material.diffuse"), 1, pComponent->modelData->material.diffuse);
        glUniform3fv(glGetUniformLocation(pComponent->shaderProgram, "material.specular"), 1, pComponent->modelData->material.specular);
        glUniform1f(glGetUniformLocation(pComponent->shaderProgram, "material.shininess"), pComponent->modelData->material.shininess);
        
        glUniform3f(glGetUniformLocation(pComponent->shaderProgram, "objectColor"), 0.5f, 0.5f, 1.0f);
        glUniform1i(glGetUniformLocation(pComponent->shaderProgram, "hasTexture"), pComponent->modelData->hasTexture);
        
        // Если есть текстура, активируем её
        if (pComponent->modelData->hasTexture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, pComponent->modelData->texture);
            glUniform1i(glGetUniformLocation(pComponent->shaderProgram, "texture1"), 0);
        }
    }
    
    // Отрисовываем модель
    glBindVertexArray(pComponent->VAO);
    glDrawElements(GL_TRIANGLES, pComponent->indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    return MENTAL_OK;
}

// Загрузка текстуры для 3D модели
MentalResult mentalLoadModelTexture(MentalComponent* pComponent, const char* texture_path) {
    if (!pComponent || !texture_path) {
        return MENTAL_POINTER_IS_NULL;
    }
    
    if (pComponent->eType != MENTAL_COMPONENT_TYPE_MODEL3D || !pComponent->modelData) {
        MENTAL_DEBUG("Component is not a 3D model or modelData is NULL");
        return MENTAL_UNKNOWN_COMPONENT_TYPE;
    }
    
    // Если уже есть текстура, удаляем её
    if (pComponent->modelData->hasTexture) {
        glDeleteTextures(1, &pComponent->modelData->texture);
    }
    
    // Загружаем новую текстуру
    MentalResult result = loadModelTexture(texture_path, &pComponent->modelData->texture);
    if (result == MENTAL_OK) {
        pComponent->modelData->hasTexture = true;
        MENTAL_DEBUG("Model texture loaded successfully: %s", texture_path);
    } else {
        pComponent->modelData->hasTexture = false;
        MENTAL_DEBUG("Failed to load model texture: %s", texture_path);
    }
    
    return result;
}

// Загрузка карты нормалей для 3D модели
MentalResult mentalLoadModelNormalMap(MentalComponent* pComponent, const char* texture_path) {
    if (!pComponent || !texture_path) {
        return MENTAL_POINTER_IS_NULL;
    }
    
    if (pComponent->eType != MENTAL_COMPONENT_TYPE_MODEL3D || !pComponent->modelData) {
        MENTAL_DEBUG("Component is not a 3D model or modelData is NULL");
        return MENTAL_UNKNOWN_COMPONENT_TYPE;
    }
    
    // Если уже есть карта нормалей, удаляем её
    if (pComponent->modelData->hasNormalMap) {
        glDeleteTextures(1, &pComponent->modelData->normal_map);
    }
    
    // Загружаем новую карту нормалей
    MentalResult result = loadModelTexture(texture_path, &pComponent->modelData->normal_map);
    if (result == MENTAL_OK) {
        pComponent->modelData->hasNormalMap = true;
        pComponent->modelData->material.use_pbr = true;
        MENTAL_DEBUG("Model normal map loaded successfully: %s", texture_path);
    } else {
        pComponent->modelData->hasNormalMap = false;
        MENTAL_DEBUG("Failed to load model normal map: %s", texture_path);
    }
    
    return result;
}

// Загрузка карты металличности для 3D модели
MentalResult mentalLoadModelMetallicMap(MentalComponent* pComponent, const char* texture_path) {
    if (!pComponent || !texture_path) {
        return MENTAL_POINTER_IS_NULL;
    }
    
    if (pComponent->eType != MENTAL_COMPONENT_TYPE_MODEL3D || !pComponent->modelData) {
        MENTAL_DEBUG("Component is not a 3D model or modelData is NULL");
        return MENTAL_UNKNOWN_COMPONENT_TYPE;
    }
    
    // Если уже есть карта металличности, удаляем её
    if (pComponent->modelData->hasMetallicMap) {
        glDeleteTextures(1, &pComponent->modelData->metallic_map);
    }
    
    // Загружаем новую карту металличности
    MentalResult result = loadModelTexture(texture_path, &pComponent->modelData->metallic_map);
    if (result == MENTAL_OK) {
        pComponent->modelData->hasMetallicMap = true;
        pComponent->modelData->material.use_pbr = true;
        MENTAL_DEBUG("Model metallic map loaded successfully: %s", texture_path);
    } else {
        pComponent->modelData->hasMetallicMap = false;
        MENTAL_DEBUG("Failed to load model metallic map: %s", texture_path);
    }
    
    return result;
}

// Загрузка карты шероховатости для 3D модели
MentalResult mentalLoadModelRoughnessMap(MentalComponent* pComponent, const char* texture_path) {
    if (!pComponent || !texture_path) {
        return MENTAL_POINTER_IS_NULL;
    }
    
    if (pComponent->eType != MENTAL_COMPONENT_TYPE_MODEL3D || !pComponent->modelData) {
        MENTAL_DEBUG("Component is not a 3D model or modelData is NULL");
        return MENTAL_UNKNOWN_COMPONENT_TYPE;
    }
    
    // Если уже есть карта шероховатости, удаляем её
    if (pComponent->modelData->hasRoughnessMap) {
        glDeleteTextures(1, &pComponent->modelData->roughness_map);
    }
    
    // Загружаем новую карту шероховатости
    MentalResult result = loadModelTexture(texture_path, &pComponent->modelData->roughness_map);
    if (result == MENTAL_OK) {
        pComponent->modelData->hasRoughnessMap = true;
        pComponent->modelData->material.use_pbr = true;
        MENTAL_DEBUG("Model roughness map loaded successfully: %s", texture_path);
    } else {
        pComponent->modelData->hasRoughnessMap = false;
        MENTAL_DEBUG("Failed to load model roughness map: %s", texture_path);
    }
    
    return result;
}

// Загрузка карты ambient occlusion для 3D модели
MentalResult mentalLoadModelAOMap(MentalComponent* pComponent, const char* texture_path) {
    if (!pComponent || !texture_path) {
        return MENTAL_POINTER_IS_NULL;
    }
    
    if (pComponent->eType != MENTAL_COMPONENT_TYPE_MODEL3D || !pComponent->modelData) {
        MENTAL_DEBUG("Component is not a 3D model or modelData is NULL");
        return MENTAL_UNKNOWN_COMPONENT_TYPE;
    }
    
    // Если уже есть карта ambient occlusion, удаляем её
    if (pComponent->modelData->hasAOMap) {
        glDeleteTextures(1, &pComponent->modelData->ao_map);
    }
    
    // Загружаем новую карту ambient occlusion
    MentalResult result = loadModelTexture(texture_path, &pComponent->modelData->ao_map);
    if (result == MENTAL_OK) {
        pComponent->modelData->hasAOMap = true;
        pComponent->modelData->material.use_pbr = true;
        MENTAL_DEBUG("Model AO map loaded successfully: %s", texture_path);
    } else {
        pComponent->modelData->hasAOMap = false;
        MENTAL_DEBUG("Failed to load model AO map: %s", texture_path);
    }
    
    return result;
}


MentalResult mentalLoadModelHeightMap(MentalComponent* pComponent, const char* texture_path) {
    // Проверка входных параметров
    if (pComponent == NULL || texture_path == NULL) {
        return MENTAL_ERROR_INVALID_PARAMETER;
    }
    
    if (pComponent->modelData == NULL) {
        MENTAL_DEBUG("Component doesn't contain model data.");
        return MENTAL_ERROR_NO_MODEL_DATA;
    }
    
    uint32_t textureID;
    glGenTextures(1, &textureID);
    
    int width, height, nrComponents;
    
    // Загружаем как одноканальное изображение (высота - только красный канал)
    stbi_set_flip_vertically_on_load(true); // Важно для корректного отображения
    unsigned char* data = stbi_load(texture_path, &width, &height, &nrComponents, 1);
    if (!data) {
        MENTAL_DEBUG("Failed to load height map texture: %s", texture_path);
        glDeleteTextures(1, &textureID);
        return MENTAL_ERROR_TEXTURE_LOAD_FAILED;
    }
    
    // Для карты высот используем только один канал (GL_RED)
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // Особые параметры для карты высот
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Устанавливаем параметр для использования в шейдере как карты высот
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    
    stbi_image_free(data);
    
    pComponent->modelData->height_map = textureID;
    pComponent->modelData->hasHeightMap = true;
    
    MENTAL_DEBUG("Height map loaded successfully: %s (ID: %u)", texture_path, textureID);
    
    return MENTAL_OK;
}

// Установка параметров традиционного материала для 3D модели
MentalResult mentalSetModelMaterial(MentalComponent* pComponent, vec3 ambient, vec3 diffuse, vec3 specular, float shininess) {
    if (!pComponent) {
        return MENTAL_POINTER_IS_NULL;
    }
    
    if (pComponent->eType != MENTAL_COMPONENT_TYPE_MODEL3D || !pComponent->modelData) {
        MENTAL_DEBUG("Component is not a 3D model or modelData is NULL");
        return MENTAL_UNKNOWN_COMPONENT_TYPE;
    }
    
    // Копируем параметры материала
    glm_vec3_copy(ambient, pComponent->modelData->material.ambient);
    glm_vec3_copy(diffuse, pComponent->modelData->material.diffuse);
    glm_vec3_copy(specular, pComponent->modelData->material.specular);
    pComponent->modelData->material.shininess = shininess;
    
    // Отключаем PBR режим
    pComponent->modelData->material.use_pbr = false;
    
    MENTAL_DEBUG("Traditional model material set successfully");
    return MENTAL_OK;
}


// Уничтожение компонента 3D модели
MentalResult mentalDestroyModel3DComponent(MentalComponent* pComponent) {
    if (!pComponent) {
        return MENTAL_POINTER_IS_NULL;
    }
    
    if (pComponent->eType != MENTAL_COMPONENT_TYPE_MODEL3D || !pComponent->modelData) {
        MENTAL_DEBUG("Component is not a 3D model or modelData is NULL");
        return MENTAL_UNKNOWN_COMPONENT_TYPE;
    }
    
    // Освобождаем память для данных модели
    free(pComponent->modelData->vertices);
    free(pComponent->modelData->texCoords);
    free(pComponent->modelData->normals);
    free(pComponent->modelData->indices);
    
    // Удаляем текстуры, если они есть
    if (pComponent->modelData->hasTexture) {
        glDeleteTextures(1, &pComponent->modelData->texture);
    }
    
    if (pComponent->modelData->hasNormalMap) {
        glDeleteTextures(1, &pComponent->modelData->normal_map);
    }
    
    if (pComponent->modelData->hasMetallicMap) {
        glDeleteTextures(1, &pComponent->modelData->metallic_map);
    }
    
    if (pComponent->modelData->hasRoughnessMap) {
        glDeleteTextures(1, &pComponent->modelData->roughness_map);
    }
    
    if (pComponent->modelData->hasAOMap) {
        glDeleteTextures(1, &pComponent->modelData->ao_map);
    }
    
    // Освобождаем память для структуры данных модели
    free(pComponent->modelData);
    pComponent->modelData = NULL;
    
    // Удаляем буферы OpenGL
    glDeleteVertexArrays(1, &pComponent->VAO);
    glDeleteBuffers(1, &pComponent->VBO);
    glDeleteBuffers(1, &pComponent->EBO);
    
    MENTAL_DEBUG("Model3D component destroyed successfully");
    return MENTAL_OK;
}