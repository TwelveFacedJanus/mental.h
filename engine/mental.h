#ifndef mental_h
#define mental_h

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "historical.h"

#define null NULL


typedef enum MentalResult {
    MENTAL_FATAL_ERROR               =-1,
    MENTAL_OK                        = 0,
    MENTAL_SUCCESS                   = 0,
    MENTAL_ERROR                     = 1,
    MENTAL_POINTER_IS_NULL           = 2,
    MENTAL_FAILED_TO_ALLOCATE_MEMORY = 3,
    MENTAL_GLFW_INIT_FAILED          = 4,
    MENTAL_GLFW_WINDOW_CREATE_FAILED = 5,
    MENTAL_FAILED_TO_INITIALIZE_GLEW = 6,
    MENTAL_SHADER_COMPILE_FAILED     = 7,
    MENTAL_SHADER_LINK_FAILED        = 8,
    MENTAL_FILE_OPEN_FAILED          = 9,
    MENTAL_UNKNOWN_COMPONENT_TYPE    = 10,
    MENTAL_ERROR_FILE_NOT_FOUND      = 11,
    MENTAL_ERROR_OUT_OF_MEMORY       = 12,
    MENTAL_ERROR_SHADER_COMPILATION  = 13,
    MENTAL_ERROR_SHADER_LINKING      = 14,
    MENTAL_ERROR_INVALID_COMPONENT   = 15,
    MENTAL_ERROR_TEXTURE_UNSUPPORTED_FORMAT = 16,
    MENTAL_ERROR_INVALID_PARAMETER = 17,
    MENTAL_ERROR_NO_MODEL_DATA = 18,
    MENTAL_ERROR_TEXTURE_LOAD_FAILED = 19,


} MentalResult;

typedef enum MentalStructureType {
    MENTAL_STRUCTURE_TYPE_PROJECT_INFO         = 0,
    MENTAL_STRUCTURE_TYPE_PROJECT              = 1,
    
    MENTAL_STRUCTURE_TYPE_WINDOW_MANAGER_INFO  = 2,
    MENTAL_STRUCTURE_TYPE_WINDOW_MANAGER       = 3,
} MentalStructureType;


typedef enum MentalComponentType {
    MENTAL_COMPONENT_TYPE_RECTANGLE,
    MENTAL_COMPONENT_TYPE_SHADER,
    MENTAL_COMPONENT_TYPE_TRIANGLE,
    MENTAL_COMPONENT_TYPE_CUBE,
    MENTAL_COMPONENT_TYPE_SKYBOX,
    MENTAL_COMPONENT_TYPE_GROUND,
    MENTAL_COMPONENT_TYPE_CLOUDS,
    MENTAL_COMPONENT_TYPE_MODEL3D, // Новый тип для 3D моделей
} MentalComponentType;

#endif // mental_h