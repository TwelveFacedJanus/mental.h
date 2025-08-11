#include "engine.h"

MentalResult mentalInitializeGLEW()
{   
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        MENTAL_DEBUG("Mental failed to initialize GLEW: %s", glewGetErrorString(err));
        return MENTAL_FAILED_TO_INITIALIZE_GLEW;
    }
    
    // Check for OpenGL version
    if (!GLEW_VERSION_3_3) {
        MENTAL_DEBUG("OpenGL 3.3 is not supported. Available: %s", glewGetString(GLEW_VERSION));
        return MENTAL_FAILED_TO_INITIALIZE_GLEW;
    }
    
    return MENTAL_OK;
}

MentalResult mentalDrawCall() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.3f, 0.2f, 0.5f, 1.0f); 
    return MENTAL_OK;
}

