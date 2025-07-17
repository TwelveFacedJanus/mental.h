#ifndef MATHEMATICA_H
#define MATHEMATICA_H

#ifdef __cplusplus
#include <vector>
#endif

/**
 * @file mathematica.h
 * @brief Generic 3D vector and 3x3 matrix library with support for user-defined types.
 *
 * This header provides macros to define 3D vectors (`Vec3`) and 3x3 matrices (`Mat3`)
 * for any data type, along with operations for initialization, filling, and conversion
 * to C arrays and C++ std::vector (in C++ environments).
 */

/**
 * @def DEFINE_VEC3(T)
 * @brief Defines a 3D vector structure for the specified type T.
 *
 * Creates a struct `Vec3_T` with three components: x, y, and z of type T.
 * @param T The data type for the vector components (e.g., float, int, double).
 */
#define DEFINE_VEC3(T) \
typedef struct Vec3_##T { \
    T x, y, z; \
} Vec3_##T

/**
 * @def VEC3_ZERO(T)
 * @brief Defines a function to set a Vec3_T to zero.
 * @param T The data type of the vector components.
 *
 * The function `vec3_zero_T` sets all components (x, y, z) of the vector to 0.
 */
#define VEC3_ZERO(T) \
MentalResult vec3_zero_##T(Vec3_##T* v) { \
    v->x = (T)0; \
    v->y = (T)0; \
    v->z = (T)0; \
    return MENTAL_OK; \
}

/**
 * @def VEC3_ONE(T)
 * @brief Defines a function to set a Vec3_T to one.
 * @param T The data type of the vector components.
 *
 * The function `vec3_one_T` sets all components (x, y, z) of the vector to 1.
 */
#define VEC3_ONE(T) \
MentalResult vec3_one_##T(Vec3_##T* v) { \
    v->x = (T)1; \
    v->y = (T)1; \
    v->z = (T)1; \
    return MENTAL_OK; \
}

/**
 * @def VEC3_FILL(T)
 * @brief Defines a function to fill a Vec3_T with a specified value.
 * @param T The data type of the vector components.
 *
 * The function `vec3_fill_T` sets all components (x, y, z) to the given value.
 */
#define VEC3_FILL(T) \
MentalResult vec3_fill_##T(Vec3_##T* v, T value) { \
    v->x = value; \
    v->y = value; \
    v->z = value; \
    return MENTAL_OK; \
}

/**
 * @def VEC3_TO_ARRAY(T)
 * @brief Defines a function to convert a Vec3_T to a C array.
 * @param T The data type of the vector components.
 *
 * The function `vec3_to_array_T` copies the vector components to a C array of size 3.
 */
#define VEC3_TO_ARRAY(T) \
MentalResult vec3_to_array_##T(const Vec3_##T* v, T arr[3]) { \
    arr[0] = v->x; \
    arr[1] = v->y; \
    arr[2] = v->z; \
    return MENTAL_OK; \
}

#ifdef __cplusplus
/**
 * @def VEC3_TO_VECTOR(T)
 * @brief Defines a function to convert a Vec3_T to a C++ std::vector (C++ only).
 * @param T The data type of the vector components.
 *
 * The function `vec3_to_vector_T` returns a std::vector<T> containing the vector components.
 * @note Only available when compiled with a C++ compiler.
 */
#define VEC3_TO_VECTOR(T) \
std::vector<T> vec3_to_vector_##T(const Vec3_##T* v) { \
    return std::vector<T>{v->x, v->y, v->z}; \
}
#else
#define VEC3_TO_VECTOR(T)
#endif

/**
 * @def DEFINE_MAT3(T)
 * @brief Defines a 3x3 matrix structure using Vec3_T for rows.
 * @param T The data type of the matrix components.
 *
 * Creates a struct `Mat3_T` with three Vec3_T rows (row0, row1, row2).
 */
#define DEFINE_MAT3(T) \
typedef struct Mat3_##T { \
    Vec3_##T row0; \
    Vec3_##T row1; \
    Vec3_##T row2; \
} Mat3_##T

/**
 * @def MAT3_ZERO(T)
 * @brief Defines a function to set a Mat3_T to zero.
 * @param T The data type of the matrix components.
 *
 * The function `mat3_zero_T` sets all elements of the matrix to 0.
 */
#define MAT3_ZERO(T) \
MentalResult mat3_zero_##T(Mat3_##T* m) { \
    vec3_zero_##T(&m->row0); \
    vec3_zero_##T(&m->row1); \
    vec3_zero_##T(&m->row2); \
    return MENTAL_OK; \
}

/**
 * @def MAT3_ONE(T)
 * @brief Defines a function to set a Mat3_T to one.
 * @param T The data type of the matrix components.
 *
 * The function `mat3_one_T` sets all elements of the matrix to 1.
 */
#define MAT3_ONE(T) \
MentalResult mat3_one_##T(Mat3_##T* m) { \
    vec3_fill_##T(&m->row0, (T)1); \
    vec3_fill_##T(&m->row1, (T)1); \
    vec3_fill_##T(&m->row2, (T)1); \
    return MENTAL_OK; \
}

/**
 * @def MAT3_IDENTITY(T)
 * @brief Defines a function to set a Mat3_T to the identity matrix.
 * @param T The data type of the matrix components.
 *
 * The function `mat3_identity_T` sets the matrix to the identity matrix (1s on diagonal, 0s elsewhere).
 */
#define MAT3_IDENTITY(T) \
MentalResult mat3_identity_##T(Mat3_##T* m) { \
    vec3_zero_##T(&m->row0); m->row0.x = (T)1; \
    vec3_zero_##T(&m->row1); m->row1.y = (T)1; \
    vec3_zero_##T(&m->row2); m->row2.z = (T)1; \
    return MENTAL_OK; \
}

/**
 * @def MAT3_FILL(T)
 * @brief Defines a function to fill a Mat3_T with a specified value.
 * @param T The data type of the matrix components.
 *
 * The function `mat3_fill_T` sets all elements of the matrix to the given value.
 */
#define MAT3_FILL(T) \
MentalResult mat3_fill_##T(Mat3_##T* m, T value) { \
    vec3_fill_##T(&m->row0, value); \
    vec3_fill_##T(&m->row1, value); \
    vec3_fill_##T(&m->row2, value); \
    return MENTAL_OK; \
}

/**
 * @def MAT3_FILL_CUSTOM(T)
 * @brief Defines a function to fill a Mat3_T with custom values.
 * @param T The data type of the matrix components.
 *
 * The function `mat3_fill_custom_T` sets each element of the matrix to the specified values.
 */
#define MAT3_FILL_CUSTOM(T) \
MentalResult mat3_fill_custom_##T(Mat3_##T* m, \
    T m00, T m01, T m02, \
    T m10, T m11, T m12, \
    T m20, T m21, T m22) { \
    m->row0.x = m00; m->row0.y = m01; m->row0.z = m02; \
    m->row1.x = m10; m->row1.y = m11; m->row1.z = m12; \
    m->row2.x = m20; m->row2.y = m21; m->row2.z = m22; \
    return MENTAL_OK; \
}

/**
 * @def DEFINE_MATH_STRUCTS(T)
 * @brief Defines Vec3 and Mat3 with all associated operations for a given type.
 * @param T The data type for the vector and matrix components.
 *
 * This macro defines the Vec3_T and Mat3_T structures and all their operations,
 * including zeroing, filling, identity matrix creation, and conversions to arrays/vectors.
 */
#define DEFINE_MATH_STRUCTS(T) \
    DEFINE_VEC3(T); \
    VEC3_ZERO(T); \
    VEC3_ONE(T); \
    VEC3_FILL(T); \
    VEC3_TO_ARRAY(T); \
    VEC3_TO_VECTOR(T); \
    DEFINE_MAT3(T); \
    MAT3_ZERO(T); \
    MAT3_ONE(T); \
    MAT3_IDENTITY(T); \
    MAT3_FILL(T); \
    MAT3_FILL_CUSTOM(T)

// Predefined types for float, int, and double
DEFINE_MATH_STRUCTS(float);  ///< Defines Vec3_float, Mat3_float, and their operations
DEFINE_MATH_STRUCTS(int);    ///< Defines Vec3_int, Mat3_int, and their operations
DEFINE_MATH_STRUCTS(double); ///< Defines Vec3_double, Mat3_double, and their operations

// Users can add their own types by invoking the macro, e.g.:
// DEFINE_MATH_STRUCTS(long);  ///< Example: Defines Vec3_long, Mat3_long, and operations
// DEFINE_MATH_STRUCTS(short); ///< Example: Defines Vec3_short, Mat3_short, and operations

#endif // MATHEMATICA_H