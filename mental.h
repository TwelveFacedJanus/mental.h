#ifndef MENTAL_H
#define MENTAL_H

#if defined(__cplusplus)
    extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFINE_VECTOR3(T) \
    typedef struct Vector3_##T { \
        T x, y, z; \
        void (*destroy)(void* vec); \
    } Vector3_##T; \
    Vector3_##T* vector3##T(T x, T y, T z); \

void m_vector_destroy(void* vec);

#define IMPLEMENT_VECTOR3(T) \
    Vector3_##T* vector3##T(T x, T y, T z) { \
        Vector3_##T* __v = (Vector3_##T*)malloc(sizeof(Vector3_##T)); \
        __v->x = x; __v->y = y; __v->z = z; \
        __v->destroy = m_vector_destroy; \
        return __v; \
    }

DEFINE_VECTOR3(int)
DEFINE_VECTOR3(float)
DEFINE_VECTOR3(char)
DEFINE_VECTOR3(double)
DEFINE_VECTOR3(size_t)

#if defined(IMPLEMENT_MENTAL)
    
    void m_vector_destroy(void* vec) {
        free(vec);
    }

    IMPLEMENT_VECTOR3(int)
    IMPLEMENT_VECTOR3(float)
    IMPLEMENT_VECTOR3(char)
    IMPLEMENT_VECTOR3(double)
    IMPLEMENT_VECTOR3(size_t)
#endif // defined IMPLEMENT_MENTAL


typedef uint_fast8_t ufast8;
typedef uint_fast16_t ufast16;
typedef uint_fast32_t ufast32;
typedef uint_fast64_t ufast64;
typedef int_fast8_t ifast8;
typedef int_fast16_t ifast16;
typedef int_fast32_t ifast32;
typedef int_fast64_t ifast64;

typedef int isize;
typedef size_t usize;

#define null ((void*)0)


#if defined(__cplusplus)
    }
#endif

#endif // #define MENTAL_H