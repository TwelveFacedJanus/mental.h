#ifndef MATHEMATICA_H
#define MATHEMATICA_H

#include "Mental.h"

// Linked List Macros (declarations only)
#define LINKED_LIST(T) \
typedef struct LinkedList_##T { \
    T value; \
    struct LinkedList_##T* next; \
} LinkedList_##T

#define LINKED_LIST_INIT(T) \
MentalResult linked_list_init_##T(LinkedList_##T** list);

#define LINKED_LIST_ADD(T) \
MentalResult linked_list_add_##T(LinkedList_##T** list, T value);

#define LINKED_LIST_REMOVE(T) \
MentalResult linked_list_remove_##T(LinkedList_##T** list, T value);

#define LINKED_LIST_DESTROY(T) \
MentalResult linked_list_destroy_##T(LinkedList_##T** list);

// Vec3 Macros (declarations only)
#define DEFINE_VEC3(T) \
typedef struct Vec3_##T { \
    T x, y, z; \
} Vec3_##T

#define VEC3_ZERO(T) \
MentalResult vec3_zero_##T(Vec3_##T* v);

#define VEC3_ONE(T) \
MentalResult vec3_one_##T(Vec3_##T* v);

#define VEC3_FILL(T) \
MentalResult vec3_fill_##T(Vec3_##T* v, T value);

#define VEC3_TO_ARRAY(T) \
MentalResult vec3_to_array_##T(const Vec3_##T* v, T arr[3]);

#define VEC3_ADD(T) \
MentalResult vec3_add_##T(const Vec3_##T* a, const Vec3_##T* b, Vec3_##T* result);

#define VEC3_SUB(T) \
MentalResult vec3_sub_##T(const Vec3_##T* a, const Vec3_##T* b, Vec3_##T* result);

#define VEC3_SCALE(T) \
MentalResult vec3_scale_##T(const Vec3_##T* v, T scalar, Vec3_##T* result);

#define VEC3_DIV(T) \
MentalResult vec3_div_##T(const Vec3_##T* v, T scalar, Vec3_##T* result);

// Mat3 Macros (declarations only)
#define DEFINE_MAT3(T) \
typedef struct Mat3_##T { \
    Vec3_##T row0; \
    Vec3_##T row1; \
    Vec3_##T row2; \
} Mat3_##T

#define MAT3_ZERO(T) \
MentalResult mat3_zero_##T(Mat3_##T* m);

#define MAT3_ONE(T) \
MentalResult mat3_one_##T(Mat3_##T* m);

#define MAT3_IDENTITY(T) \
MentalResult mat3_identity_##T(Mat3_##T* m);

#define MAT3_FILL(T) \
MentalResult mat3_fill_##T(Mat3_##T* m, T value);

#define MAT3_FILL_CUSTOM(T) \
MentalResult mat3_fill_custom_##T(Mat3_##T* m, \
    T m00, T m01, T m02, \
    T m10, T m11, T m12, \
    T m20, T m21, T m22);

#define MAT3_ADD(T) \
MentalResult mat3_add_##T(const Mat3_##T* a, const Mat3_##T* b, Mat3_##T* result);

#define MAT3_SUB(T) \
MentalResult mat3_sub_##T(const Mat3_##T* a, const Mat3_##T* b, Mat3_##T* result);

#define MAT3_SCALE(T) \
MentalResult mat3_scale_##T(const Mat3_##T* m, T scalar, Mat3_##T* result);

#define MAT3_DIV(T) \
MentalResult mat3_div_##T(const Mat3_##T* m, T scalar, Mat3_##T* result);

#define MAT3_MUL(T) \
MentalResult mat3_mul_##T(const Mat3_##T* a, const Mat3_##T* b, Mat3_##T* result);

// Macro to declare all types and functions
#define DECLARE_MATH_STRUCTS(T) \
    LINKED_LIST(T); \
    DEFINE_VEC3(T); \
    DEFINE_MAT3(T); \
    LINKED_LIST_INIT(T); \
    LINKED_LIST_ADD(T); \
    LINKED_LIST_REMOVE(T); \
    LINKED_LIST_DESTROY(T); \
    VEC3_ZERO(T); \
    VEC3_ONE(T); \
    VEC3_FILL(T); \
    VEC3_TO_ARRAY(T); \
    VEC3_ADD(T); \
    VEC3_SUB(T); \
    VEC3_SCALE(T); \
    VEC3_DIV(T); \
    MAT3_ZERO(T); \
    MAT3_ONE(T); \
    MAT3_IDENTITY(T); \
    MAT3_FILL(T); \
    MAT3_FILL_CUSTOM(T); \
    MAT3_ADD(T); \
    MAT3_SUB(T); \
    MAT3_SCALE(T); \
    MAT3_DIV(T); \
    MAT3_MUL(T)

// Declare structures for common types
DECLARE_MATH_STRUCTS(float);
DECLARE_MATH_STRUCTS(int);
DECLARE_MATH_STRUCTS(double);

#endif // MATHEMATICA_H