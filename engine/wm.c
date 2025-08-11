#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "wm.h"

MentalResult mentalCreateWM(MentalWindowManager* pManager) {
    if (!pManager) {
        MENTAL_DEBUG("Mental Window manager pointer is null.");
        return MENTAL_POINTER_IS_NULL;
    }

    if (glfwInit() != GLFW_TRUE) {
        MENTAL_DEBUG("Failed to initialize GLFW library.");
        return MENTAL_GLFW_INIT_FAILED;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    
#ifdef    __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_TRUE);
#endif // __APPLE__

    pManager->pNext = glfwCreateWindow(pManager->pInfo->aSizes[0], pManager->pInfo->aSizes[1], 
                                     pManager->pInfo->pTitle, NULL, NULL);
    if (!pManager->pNext) {
        MENTAL_DEBUG("Failed to create GLFW window.");
        glfwTerminate();
        return MENTAL_GLFW_WINDOW_CREATE_FAILED;
    }

    glfwMakeContextCurrent(pManager->pNext);
    
    // Print OpenGL version info
    MENTAL_DEBUG("OpenGL Version: %s", glGetString(GL_VERSION));
    MENTAL_DEBUG("OpenGL Renderer: %s", glGetString(GL_RENDERER));
    MENTAL_DEBUG("OpenGL Vendor: %s", glGetString(GL_VENDOR));
    
    if (mentalInitializeGLEW() != MENTAL_OK) {
        MENTAL_DEBUG("Failed to initialize GLEW.");
        glfwDestroyWindow(pManager->pNext);
        glfwTerminate();
        return MENTAL_FAILED_TO_INITIALIZE_GLEW;
    }

    // Set input callbacks
    glfwSetWindowUserPointer(pManager->pNext, pManager);
    glfwSetCursorPosCallback(pManager->pNext, mental_mouse_callback);
    glfwSetScrollCallback(pManager->pNext, mental_scroll_callback);
    glfwSetFramebufferSizeCallback(pManager->pNext, mental_framebuffer_size_callback);
    glfwSetInputMode(pManager->pNext, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Configure OpenGL
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // Proper depth testing
    glEnable(GL_MULTISAMPLE); // Включение сглаживания
    glEnable(GL_POLYGON_SMOOTH);
    glViewport(0, 0, pManager->pInfo->aSizes[0], pManager->pInfo->aSizes[1]);

    // Initialize camera
    mental_camera_init(&pManager->camera, 0.0f, 0.0f, 3.0f);

    // Initialize skybox
    if (mentalCreateSkybox(&pManager->skybox) != MENTAL_OK) {
        MENTAL_DEBUG("Failed to create skybox.");
        return MENTAL_ERROR;
    }
    
    // Load skybox textures (you can replace these paths with your own textures)
    if (mentalLoadSkyboxTextures(&pManager->skybox, 
                                "textures/skybox/right.ppm", "textures/skybox/left.ppm",
                                "textures/skybox/top.ppm", "textures/skybox/bottom.ppm",
                                "textures/skybox/front.ppm", "textures/skybox/back.ppm") != MENTAL_OK) {
        MENTAL_DEBUG("Failed to load skybox textures.");
        return MENTAL_ERROR;
    }
    
    // Attach skybox shaders
    if (mentalAttachSkyboxShader(&pManager->skybox, "skybox_vertex.glsl", "skybox_fragment.glsl") != MENTAL_OK) {
        MENTAL_DEBUG("Failed to attach skybox shaders.");
        return MENTAL_ERROR;
    }

    return MENTAL_OK;
}

// Input callback functions
void mental_mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    MentalWindowManager* pManager = glfwGetWindowUserPointer(window);
    static float lastX = 400.0f;  // Default value
    static float lastY = 300.0f;  // Default value
    static bool firstMouse = true;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    mental_process_mouse(&pManager->camera, xoffset, yoffset);
}

void mental_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)xoffset; // Suppress unused parameter warning
    MentalWindowManager* pManager = glfwGetWindowUserPointer(window);
    mental_camera_process_zoom(&pManager->camera, (float)yoffset);
}

void mental_framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    MentalWindowManager* pManager = glfwGetWindowUserPointer(window);
    if (pManager && pManager->pInfo) {
        pManager->pInfo->aSizes[0] = width;
        pManager->pInfo->aSizes[1] = height;
    }
    glViewport(0, 0, width, height);
    MENTAL_DEBUG("Framebuffer resized to %dx%d", width, height);
}

MentalResult mentalRunWM(MentalWindowManager *pManager) {
    if (!pManager) {
        MENTAL_DEBUG("Pointer to MentalWindowManager is null.");
        return MENTAL_POINTER_IS_NULL;
    }

    // Create ground component
    MentalComponent ground = {
        .eType = MENTAL_COMPONENT_TYPE_GROUND,
        .position = {0.0f, -1.0f, 0.0f},  // Земля ниже уровня камеры
        .size = 1.0f,
        .rotation = {0.0f, 0.0f, 0.0f},
        .VAO = 0,
        .VBO = 0,
        .EBO = 0,
        .shaderProgram = 0
    };
    if (mentalCreateGroundComponent(&ground) != MENTAL_OK) {
        MENTAL_DEBUG("Failed to create ground component.");
        return MENTAL_ERROR;
    }
    MENTAL_DEBUG("Ground component created successfully.");
    if (mentalAttachShader(&ground, "ground_vertex.glsl", "ground_fragment.glsl") != MENTAL_OK) {
        MENTAL_DEBUG("Failed to attach shader to ground.");
        mentalDestroyComponent(&ground);
        return MENTAL_ERROR;
    }
    MENTAL_DEBUG("Shader attached to ground successfully.");
    
    // Create clouds component
    /*
    MentalComponent clouds = {
        .eType = MENTAL_COMPONENT_TYPE_CLOUDS,
        .position = {0.0f, 5.0f, 0.0f},  // Облака высоко над землей
        .size = 1.0f,
        .rotation = {0.0f, 0.0f, 0.0f},
        .VAO = 0,
        .VBO = 0,
        .EBO = 0,
        .shaderProgram = 0
    };
    
    if (mentalCreateCloudComponent(&clouds) != MENTAL_OK) {
        MENTAL_DEBUG("Failed to create clouds component.");
        mentalDestroyComponent(&ground);
        return MENTAL_ERROR;
    }
    
    MENTAL_DEBUG("Clouds component created successfully.");
    if (mentalAttachShader(&clouds, "cloud_vertex.glsl", "cloud_fragment.glsl") != MENTAL_OK) {
        MENTAL_DEBUG("Failed to attach shader to clouds.");
        mentalDestroyComponent(&ground);
        mentalDestroyComponent(&clouds);
        return MENTAL_ERROR;
    }
    MENTAL_DEBUG("Shader attached to clouds successfully.");
    */
    // Create 3D model component for cube
    MentalComponent cube = {
        .eType = MENTAL_COMPONENT_TYPE_MODEL3D,
        .position = {0.0f, 0.0f, -3.0f},  // Position in 3D space
        .size = 0.5f,                     // Scale factor
        .rotation = {0.0f, 0.0f, 0.0f},  // Rotation angles in degrees
        .VAO = 0,
        .VBO = 0,
        .EBO = 0,
        .shaderProgram = 0,
    };
    
    // Create components
    MentalComponent rectangle = {
        .eType = MENTAL_COMPONENT_TYPE_RECTANGLE,
        .position = {2.0f, 0.0f, 0.0f},  // Using 3D coordinates now
        .size = 1.0f,                     // Scale factor
        .rotation = {0.0f, 0.0f, 0.0f},  // Rotation angles in degrees
        .VAO = 0,
        .VBO = 0,
        .EBO = 0,
        .shaderProgram = 0
    };
    // Create 3D model component
    if (mentalCreateModel3DComponent(&cube) != MENTAL_OK) {
        MENTAL_DEBUG("Failed to create cube model component.");
        mentalDestroyComponent(&ground);
        return MENTAL_ERROR;
    }
    MENTAL_DEBUG("Cube model component created successfully.");
    
    // Load the 3D model from OBJ file
    if (mentalLoadModel3D(&cube, "sphere.obj") != MENTAL_OK) {
        MENTAL_DEBUG("Failed to load cube model.");
        mentalDestroyComponent(&ground);
        mentalDestroyModel3DComponent(&cube);
        return MENTAL_ERROR;
    }
    MENTAL_DEBUG("Cube model loaded successfully.");
    MENTAL_DEBUG("Model loaded: %d vertices, %d indices", 
             cube.modelData->vertexCount, 
             cube.modelData->indexCount);
    // После загрузки модели выведите информацию о UV-координатах
    MENTAL_DEBUG("Model UV coords sample: %f, %f", 
                cube.modelData->texCoords[0], 
                cube.modelData->texCoords[1]);
    // Try to load rocky terrain PBR textures from rocky-rugged-terrain-bl directory
    if (mentalLoadModelTexture(&cube, "rocky-rugged-terrain-bl/rocky-rugged-terrain_1_albedo.png") != MENTAL_OK) {
        MENTAL_DEBUG("Failed to load rocky terrain albedo texture");
    }
    MENTAL_DEBUG("Rocky terrain textures loaded. PBR textures will be auto-detected if available.");

    // Attach PBR shader to the 3D model
    if (mentalAttachPBRShader(&cube) != MENTAL_OK) {
        MENTAL_DEBUG("Failed to attach PBR shader to cube model.");
        mentalDestroyComponent(&ground);
        mentalDestroyModel3DComponent(&cube);
        return MENTAL_ERROR;
    }
    MENTAL_DEBUG("PBR shader attached to cube model successfully.");
    GLuint shaderProgram = cube.shaderProgram;
    glUseProgram(shaderProgram);
    glUniform1i(glGetUniformLocation(shaderProgram, "hasHeightMap"), 1); // Включить деформацию
    glUniform1f(glGetUniformLocation(shaderProgram, "heightScale"), 0.2f); // Сила деформации
    // Set rocky terrain PBR material
    vec3 rockyAlbedo = {0.5f, 0.5f, 0.5f}; // Neutral color for rock (will be influenced by texture)
    float rockyMetallic = 0.05f;           // Минимальная металличность для камня
    float rockyRoughness = 0.9f;           // Очень шероховатая поверхность для усиления эффекта выпуклости
    float rockyAO = 1.0f;                  // Full ambient occlusion
    
    mentalSetModelPBRMaterial(&cube, rockyAlbedo, rockyMetallic, rockyRoughness, rockyAO);
    MENTAL_DEBUG("Rocky terrain PBR material set successfully.");
    
    MENTAL_DEBUG("===== LOADING CUBE MATERIALS======");
    // Load additional PBR maps explicitly
    mentalLoadModelNormalMap(&cube, "rocky-rugged-terrain-bl/rocky-rugged-terrain_1_normal-ogl.png");
    mentalLoadModelMetallicMap(&cube, "rocky-rugged-terrain-bl/rocky-rugged-terrain_1_metallic.png");
    mentalLoadModelRoughnessMap(&cube, "rocky-rugged-terrain-bl/rocky-rugged-terrain_1_roughness.png");
    mentalLoadModelAOMap(&cube, "rocky-rugged-terrain-bl/rocky-rugged-terrain_1_ao.png");
    mentalLoadModelHeightMap(&cube, "rocky-rugged-terrain-bl/rocky-rugged-terrain_1_height.png");
    MENTAL_DEBUG("Additional PBR maps loaded.");
    glUniform1i(glGetUniformLocation(cube.shaderProgram, "hasHeightMap"), true);
    glUniform1f(glGetUniformLocation(cube.shaderProgram, "heightScale"), 0.2f);
    glPatchParameteri(GL_PATCH_VERTICES, 3);
    glUniform1f(glGetUniformLocation(shaderProgram, "TessLevel"), 16.0f);
    
    if (mentalCreateComponent(&rectangle) != MENTAL_OK) {
        MENTAL_DEBUG("Failed to create rectangle component.");
        mentalDestroyComponent(&ground);
        mentalDestroyModel3DComponent(&cube);
        return MENTAL_ERROR;
    }
    MENTAL_DEBUG("Rectangle component created successfully.");
    if (mentalAttachShader(&rectangle, "vertex.glsl", "fragment.glsl") != MENTAL_OK) {
        MENTAL_DEBUG("Failed to attach shader to rectangle.");
        mentalDestroyComponent(&ground);
        mentalDestroyModel3DComponent(&cube);
        mentalDestroyComponent(&rectangle);
        return MENTAL_ERROR;
    }
    MENTAL_DEBUG("Shader attached to rectangle successfully.");
    
    MentalComponent triangle = {
        .eType = MENTAL_COMPONENT_TYPE_TRIANGLE,
        .position = {1.5f, 0.0f, 0.0f},
        .size = 1.0f,
        .rotation = {0.0f, 0.0f, 0.0f},
        .VAO = 0,
        .VBO = 0,
        .EBO = 0,
        .shaderProgram = 0
    };
    if (mentalCreateComponent(&triangle) != MENTAL_OK) {
        MENTAL_DEBUG("Failed to create triangle component.");
        mentalDestroyComponent(&ground);
        mentalDestroyModel3DComponent(&cube);
        mentalDestroyComponent(&rectangle);
        return MENTAL_ERROR;
    }
    MENTAL_DEBUG("Triangle component created successfully.");
    if (mentalAttachShader(&triangle, "vertex.glsl", "fragment.glsl") != MENTAL_OK) {
        MENTAL_DEBUG("Failed to attach shader to triangle.");
        mentalDestroyComponent(&ground);
        mentalDestroyModel3DComponent(&cube);
        mentalDestroyComponent(&rectangle);
        mentalDestroyComponent(&triangle);
        return MENTAL_ERROR;
    }
    MENTAL_DEBUG("Shader attached to triangle successfully.");

    // Timing variables
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    // Main loop
    while (!glfwWindowShouldClose(pManager->pNext)) {
        // Calculate delta time
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        mental_process_keyboard(pManager, deltaTime);

        // Update viewport if window was resized
        int width, height;
        glfwGetFramebufferSize(pManager->pNext, &width, &height);
        if (width != pManager->pInfo->aSizes[0] || height != pManager->pInfo->aSizes[1]) {
            pManager->pInfo->aSizes[0] = width;
            pManager->pInfo->aSizes[1] = height;
            glViewport(0, 0, width, height);
        }

        // Clear screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw skybox first (background)
        if (mentalDrawSkybox(&pManager->skybox, pManager) != MENTAL_OK) {
            MENTAL_DEBUG("Failed to draw skybox.");
        }

        // Draw ground first (after skybox)
        if (mentalDrawGroundComponent(&ground, pManager) != MENTAL_OK) {
            MENTAL_DEBUG("Failed to draw ground component.");
        }
        
        /*
        // Анимируем и отрисовываем облака
        clouds.rotation[1] = currentFrame * 5.0f; // Медленное вращение облаков
        if (mentalDrawCloudComponent(&clouds, pManager) != MENTAL_OK) {
            MENTAL_DEBUG("Failed to draw clouds component.");
        }
        */
        
        // Update and render components
        //cube.rotation[1] = currentFrame * 20.0f;  // Вращение куба вокруг оси Y
        //cube.rotation[0] = currentFrame * 10.0f;  // Вращение куба вокруг оси X
        rectangle.rotation[1] = currentFrame * 50.0f;
        triangle.rotation[0] = currentFrame * 30.0f;
        
        // Отрисовка 3D модели куба
        if (mentalDrawModel3DComponent(&cube, pManager) != MENTAL_OK) {
            MENTAL_DEBUG("Failed to draw cube model component.");
        }
        
        if (mentalDrawComponent(&rectangle, pManager) != MENTAL_OK) {
            MENTAL_DEBUG("Failed to draw rectangle component.");
        }
        if (mentalDrawComponent(&triangle, pManager) != MENTAL_OK) {
            MENTAL_DEBUG("Failed to draw triangle component.");
        }

        // Swap buffers and poll events
        glfwSwapBuffers(pManager->pNext);
        glfwPollEvents();
    }

    // Cleanup
    mentalDestroyComponent(&ground);
    //mentalDestroyComponent(&clouds);
    mentalDestroyModel3DComponent(&cube);
    mentalDestroyComponent(&rectangle);
    mentalDestroyComponent(&triangle);
    mentalDestroySkybox(&pManager->skybox);
    
    MENTAL_DEBUG("Window closed successfully.");
    return MENTAL_OK;
}

MentalResult mentalDestroyWM(MentalWindowManager *pManager) {
    if (!pManager) {
        MENTAL_DEBUG("Pointer to pManager is null.");
        return MENTAL_POINTER_IS_NULL;
    }

    if (pManager->pNext) {
        glfwDestroyWindow(pManager->pNext);
        pManager->pNext = NULL;
    }
    glfwTerminate();
    return MENTAL_OK;
}