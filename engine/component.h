#ifndef mental_component_h
#define mental_component_h

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cglm.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>

#include "mental.h"

#include <cglm/cglm.h>
#include <cglm/vec3.h>
#include <cglm/mat4.h>
#include <cglm/affine.h>
#include <cglm/cam.h>

// Forward declarations
typedef struct MentalWindowManager MentalWindowManager;
typedef struct MentalWindowManagerInfo MentalWindowManagerInfo;

// Структура для хранения материала 3D модели
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

// Структура для хранения данных 3D модели
typedef struct Model3DData {
    float* vertices;       // Вершины модели
    float* normals;        // Нормали
    float* texCoords;      // Текстурные координаты
    unsigned int* indices; // Индексы
    unsigned int vertexCount;       // Количество вершин
    unsigned int indexCount;        // Количество индексов
    
    // Текстуры
    uint32_t texture;      // ID базовой текстуры (диффузная/альбедо)
    uint32_t normal_map;   // ID карты нормалей
    uint32_t metallic_map; // ID карты металличности
    uint32_t roughness_map;// ID карты шероховатости
    uint32_t ao_map;       // ID карты ambient occlusion
    uint32_t height_map;   // ID карты высот для параллакс-маппинга
    
    bool hasTexture;       // Флаг наличия базовой текстуры
    bool hasNormalMap;     // Флаг наличия карты нормалей
    bool hasMetallicMap;   // Флаг наличия карты металличности
    bool hasRoughnessMap;  // Флаг наличия карты шероховатости
    bool hasAOMap;         // Флаг наличия карты ambient occlusion
    bool hasHeightMap;     // Флаг наличия карты высот
    
    Material material;     // Материал модели
} Model3DData;

typedef struct MentalComponent {
    uint32_t   VAO, VBO, EBO;
    uint32_t   shaderProgram;
    MentalComponentType eType;
    float position[3];  // x, y, z координаты
    float size;         // размер компонента
    float rotation[3];  // углы поворота по осям X, Y, Z (в градусах)
    int indexCount;     // количество индексов для отрисовки (для земли)
    
    // Данные для 3D модели
    Model3DData* modelData; // Указатель на данные модели (NULL для других типов компонентов)
} MentalComponent;

typedef struct MentalSkybox {
    uint32_t   VAO, VBO;
    uint32_t   cubemapTexture;
    uint32_t   shaderProgram;
    bool       isInitialized;
} MentalSkybox;

typedef struct MentalCamera {
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 world_up;
    
    float yaw;
    float pitch;
    
    float movement_speed;
    float mouse_sensitivity;
    float zoom;
} MentalCamera;

void mental_camera_init(MentalCamera* cam, float pos_x, float pos_y, float pos_z);
void mental_camera_update_vectors(MentalCamera* cam);
void mental_camera_get_view_matrix(MentalCamera* cam, mat4 dest);
void mental_camera_get_projection_matrix(MentalCamera* cam, mat4 dest, float aspect_ratio);
void mental_camera_process_zoom(MentalCamera* cam, float yoffset);

typedef struct MentalComponentManager {
    MentalComponent*             components;
    uint32_t                     count;
} MentalComponentManager;

MentalResult mentalCreateCM(MentalComponentManager* pCM);
MentalResult mentalAddComponent(MentalComponentManager *pCM);
MentalResult mentalDestroyCM(MentalComponentManager *pCM);

void __mental_create_rectangle(MentalComponent *pComponent);
void __mental_draw_rectangle(MentalComponent *pComponent);
void __mental_create_ground(MentalComponent *pComponent);

MentalResult mentalAttachShader(MentalComponent* pComponent, const char* vertex_path, const char* fragment_path);
MentalResult mentalCreateComponent(MentalComponent* pComponent);
MentalResult mentalDrawComponent(MentalComponent* pComponent, MentalWindowManager *pManager);
MentalResult mentalDestroyComponent(MentalComponent *pComponent);

void mentalSetSize(MentalComponent* pComponent, float size);
void mentalSetPosition(MentalComponent* pComponent, float x, float y);
void mentalSetRotation(MentalComponent* pComponent, float angleX, float angleY, float angleZ);
void mental_process_keyboard(MentalWindowManager* wm, float delta_time);
void mental_process_mouse(MentalCamera* cam, float xoffset, float yoffset);
void mental_mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mental_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mental_framebuffer_size_callback(GLFWwindow* window, int width, int height);

// Skybox functions
MentalResult mentalCreateSkybox(MentalSkybox* pSkybox);
MentalResult mentalLoadSkyboxTextures(MentalSkybox* pSkybox, const char* right, const char* left, 
                                     const char* top, const char* bottom, 
                                     const char* front, const char* back);

// 3D Model functions
MentalResult mentalCreateModel3DComponent(MentalComponent* pComponent);
MentalResult mentalLoadModel3D(MentalComponent* pComponent, const char* model_path);
MentalResult mentalLoadModelTexture(MentalComponent* pComponent, const char* texture_path);
MentalResult mentalDrawModel3DComponent(MentalComponent* pComponent, MentalWindowManager *pManager);
MentalResult mentalDestroyModel3DComponent(MentalComponent* pComponent);
MentalResult mentalSetModelMaterial(MentalComponent* pComponent, vec3 ambient, vec3 diffuse, vec3 specular, float shininess);
MentalResult mentalSetModelPBRMaterial(MentalComponent* pComponent, vec3 albedo, float metallic, float roughness, float ao);
MentalResult mentalAttachPBRShader(MentalComponent* pComponent);
MentalResult mentalAttachSkyboxShader(MentalSkybox* pSkybox, const char* vertex_path, const char* fragment_path);
MentalResult mentalDrawSkybox(MentalSkybox* pSkybox, MentalWindowManager *pManager);
MentalResult mentalDestroySkybox(MentalSkybox *pSkybox);
uint32_t mentalLoadCubemap(const char* faces[6]);

// Ground functions
MentalResult mentalCreateGroundComponent(MentalComponent* pComponent);
MentalResult mentalLoadGroundTexture(MentalComponent* pComponent, const char* texture_path);
MentalResult mentalDrawGroundComponent(MentalComponent* pComponent, MentalWindowManager *pManager);

// Cloud functions
MentalResult mentalCreateCloudComponent(MentalComponent* pComponent);
MentalResult mentalDrawCloudComponent(MentalComponent* pComponent, MentalWindowManager *pManager);

// 3D Model functions
MentalResult mentalCreateModel3DComponent(MentalComponent* pComponent);
MentalResult mentalLoadModel3D(MentalComponent* pComponent, const char* model_path);
MentalResult mentalDrawModel3DComponent(MentalComponent* pComponent, MentalWindowManager *pManager);
MentalResult mentalDestroyModel3DComponent(MentalComponent* pComponent);

// 3D Model texture functions
MentalResult mentalLoadModelTexture(MentalComponent* pComponent, const char* texture_path);
MentalResult mentalLoadModelNormalMap(MentalComponent* pComponent, const char* texture_path);
MentalResult mentalLoadModelMetallicMap(MentalComponent* pComponent, const char* texture_path);
MentalResult mentalLoadModelRoughnessMap(MentalComponent* pComponent, const char* texture_path);
MentalResult mentalLoadModelAOMap(MentalComponent* pComponent, const char* texture_path);
MentalResult mentalLoadModelHeightMap(MentalComponent* pComponent, const char* texture_path);

// 3D Model material functions
MentalResult mentalSetModelMaterial(MentalComponent* pComponent, vec3 ambient, vec3 diffuse, vec3 specular, float shininess);
MentalResult mentalSetModelPBRMaterial(MentalComponent* pComponent, vec3 albedo, float metallic, float roughness, float ao);
MentalResult mentalAttachPBRShader(MentalComponent* pComponent);

#endif // mental_component_h