/*! \file mental.h
 * \brief Заголовочный файл с определениями стандартных типов и макросов для C и C++.
 *
 * Файл предоставляет определения стандартных типов и макросов для обеспечения переносимости
 * и удобства работы с целочисленными типами, указателями и размерами в программах на C и C++.
 * Поддерживает как C, так и C++ с использованием условной компиляции.
 */

#ifndef MENTAL_H
#define MENTAL_H

#ifdef __cplusplus
    #include <cstdint>
    #include <cstddef>
    #include <vector>
#else
    #include <stdint.h>
    #include <stddef.h>
    #include <stdbool.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <time.h>
    #include <string.h>
    #include <assert.h>
#endif

#include "Types.h"

#define MENTAL_USE_GLEW

#ifdef MENTAL_USE_GLEW
    #include <GL/glew.h>
#endif
#ifdef MENTAL_INCLUDE_GLFW
    #include <GLFW/glfw3.h>
#endif

/*! \def mental.h versions.
 * \brief Макрос, определяющий версии mental.h
 * \note Внимание, подобные макросы выстанавливаются автоматочески при сборке проекта. И эти будут удалены в будущем.
 */
#define MENTAL_H_VERSION_MAJOR_A 1
#define MENTAL_H_VERSION_MINOR_A 3
#define MENTAL_H_VERSION_PATCH_A 0

#define MENTAL_OPENGL_VERSION_MAJOR 3
#define MENTAL_OPENGL_VERSION_MINOR 3

#define MENTAL_USE_MATHEMATICA



// Макрос для создания лямбда-функций (GCC/Clang)
#if defined(__GNUC__) || defined(__clang__)
#define LAMBDA(ret_type, body) ({ ret_type __fn__ body __fn__; })
#else
#error "Lambda functions require GCC or Clang compiler"
#endif



#if !defined(__cplusplus)
    /*! \def nullptr
     * \brief Макрос, определяющий `nullptr` как `NULL` в C.
     */
    #define nullptr NULL
#endif
/*! @} */

/*! \enum MentalResult
 * \brief Перечисление для представления кодов возврата функций.
 */
typedef enum MentalResult
{
    MENTAL_FATAL = -1,  /*!< Фатальная ошибка. */
    MENTAL_OK = 0,      /*!< Успешное выполнение. */
    MENTAL_ERROR = 1,   /*!< Ошибка выполнения. */
    MENTAL_POINTER_IS_NULL = 2, /*!< Указатель на NULL. */
#ifdef MENTAL_INCLUDE_GLFW
    MENTAL_FAILED_TO_INIT_GLFW = 3,
    MENTAL_FAILED_TO_CREATE_GLFW_WINDOW = 4,
#endif
    MENTAL_NO = 5,
    MENTAL_YES = 6,
}
MentalResult;


typedef enum MentalGraphicsBackendType {
    MENTAL_OPENGL  = 0,
    MENTAL_VULKAN  = 1,
    MENTAL_MOLTEN  = 2,
    MENTAL_NOTHING = 3,
} MentalGraphicsBackendType;

typedef enum MentalStructureType {
    MENTAL_STRUCTURE_TYPE_GRAPHICS_INFO = 0,
    MENTAL_STRUCTURE_TYPE_GRAPHICS = 1,
    MENTAL_STRUCTURE_TYPE_APPLICATION = 2,
    MENTAL_STRUCTURE_TYPE_FUNCTION_PIPELINE = 3,
    MENTAL_STRUCTURE_TYPE_FUNCTION_PIPELINE_INFO = 4,
    MENTAL_STRUCTURE_TYPE_SHADER_MATERIAL_INFO   = 5,
    MENTAL_STRUCTURE_TYPE_SHADER_MATERIAL        = 6,
    MENTAL_STRUCTURE_TYPE_COMPONENT_INFO,
    MENTAL_STRUCTURE_TYPE_COMPONENT,
} MentalStructureType;


typedef enum MentalBackendType {
#ifdef MENTAL_INCLUDE_GLFW
    MENTAL_BACKEND_TYPE_GLFW,
#endif
    MENTAL_BACKEND_TYPE_CUSTOM,
} MentalBackendType;

typedef enum MentalRenderAPIType {
    MENTAL_RENDER_API_TYPE_OPENGL,
    MENTAL_RENDER_API_TYPE_VULKAN,
    MENTAL_RENDER_API_TYPE_MOLTENVK,
    MENTAL_RENDER_API_TYPE_METAL,
    MENTAL_RENDER_API_TYPE_DIRECTX,
} MentalRenderAPIType;


typedef enum MentalComponentType {
    MENTAL_COMPONENT_TYPE_RECTANGLE,
    MENTAL_COMPONENT_TYPE_CUBE,
    MENTAL_COMPONENT_TYPE_OBJ,
    MENTAL_COMPONENT_TYPE_FBX,
    MENTAL_COMPONENT_TYPE_MATERIAL,
} MentalComponentType;

typedef enum MentalMaterialType {
    MENTAL_MATERIAL_TYPE_SHADER,
    MENTAL_MATERIAL_TYPE_TEXTURE,
} MentalMaterialType;


#ifdef MENTAL_USE_MATHEMATICA
#include "Mathematica.h"
#endif

#include "Historical.h"
#include "Pipes.h"
#include "../graphics/render/render.h"
#include "../graphics/graphics.h"

#endif // MENTAL_H
