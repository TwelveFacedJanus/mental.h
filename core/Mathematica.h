#ifndef MATHEMATICA_H
#define MATHEMATICA_H

/**
 * @file mathematica.h
 * @brief Generic 3D vector and 3x3 matrix library with support for user-defined types.
 *
 * This header provides macros to define 3D vectors (`Vec3`) and 3x3 matrices (`Mat3`)
 * for any data type, along with operations for initialization, filling, arithmetic operations,
 * and conversion to C arrays and C++ std::vector (in C++ environments).
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

#define LINKED_LIST(T) \
typedef struct LinkedList_##T { \
    T value; \
    struct LinkedList_##T* next; \
} LinkedList_##T

#define LINKED_LIST_INIT(T) \
MentalResult linked_list_init_##T(LinkedList_##T** list) { \
    if (!list) return MENTAL_FATAL; \
    *list = NULL; \
    return MENTAL_OK; \
}

#define LINKED_LIST_ADD(T) \
MentalResult linked_list_add_##T(LinkedList_##T** list, T value) { \
    if (!list) return MENTAL_FATAL; \
    LinkedList_##T* new_node = (LinkedList_##T*)malloc(sizeof(LinkedList_##T)); \
    if (!new_node) return MENTAL_FATAL; \
    new_node->value = value; \
    new_node->next = *list; \
    *list = new_node; \
    return MENTAL_OK; \
}

#define LINKED_LIST_REMOVE(T) \
MentalResult linked_list_remove_##T(LinkedList_##T** list, T value) { \
    if (!list || !*list) return MENTAL_FATAL; \
    LinkedList_##T* current = *list; \
    LinkedList_##T* previous = NULL; \
    while (current != NULL) { \
        if (current->value == value) { \
            if (previous == NULL) { \
                *list = current->next; \
            } else { \
                previous->next = current->next; \
            } \
            free(current); \ 
            return MENTAL_OK; \
        } \
        previous = current; \
        current = current->next; \
    } \
    return MENTAL_ERROR; \
}

#define LINKED_LIST_DESTROY(T) \
MentalResult linked_list_destroy_##T(LinkedList_##T** list) { \
    if (!list || !*list) return MENTAL_FATAL; \
    LinkedList_##T* current = *list; \
    while (current != NULL) { \
        LinkedList_##T* next = current->next; \
        free(current); \
        current = next; \
    } \
    *list = NULL; \
    return MENTAL_OK; \
}

/**
 * @def VEC3_ZERO(T)
 * @brief Defines a function to set a Vec3_T to zero.
 * @param T The data type of the vector components.
 */
#define VEC3_ZERO(T) \
MentalResult vec3_zero_##T(Vec3_##T* v) { \
    if (!v) return MENTAL_FATAL; \
    v->x = (T)0; \
    v->y = (T)0; \
    v->z = (T)0; \
    return MENTAL_OK; \
}

/**
 * @def VEC3_ONE(T)
 * @brief Defines a function to set a Vec3_T to one.
 * @param T The data type of the vector components.
 */
#define VEC3_ONE(T) \
MentalResult vec3_one_##T(Vec3_##T* v) { \
    if (!v) return MENTAL_FATAL; \
    v->x = (T)1; \
    v->y = (T)1; \
    v->z = (T)1; \
    return MENTAL_OK; \
}

/**
 * @def VEC3_FILL(T)
 * @brief Defines a function to fill a Vec3_T with a specified value.
 * @param T The data type of the vector components.
 */
#define VEC3_FILL(T) \
MentalResult vec3_fill_##T(Vec3_##T* v, T value) { \
    if (!v) return MENTAL_FATAL; \
    v->x = value; \
    v->y = value; \
    v->z = value; \
    return MENTAL_OK; \
}



/**
 * @def VEC3_TO_ARRAY(T)
 * @brief Defines a function to convert a Vec3_T to a C array.
 * @param T The data type of the vector components.
 */
#define VEC3_TO_ARRAY(T) \
MentalResult vec3_to_array_##T(const Vec3_##T* v, T arr[3]) { \
    if (!v || !arr) return MENTAL_FATAL; \
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
 */
#define VEC3_TO_VECTOR(T) \
std::vector<T> vec3_to_vector_##T(const Vec3_##T* v) { \
    if (!v) return std::vector<T>(); \
    return std::vector<T>{v->x, v->y, v->z}; \
}
#else
#define VEC3_TO_VECTOR(T)
#endif

/**
 * @def VEC3_ADD(T)
 * @brief Defines a function to add two Vec3_T vectors.
 * @param T The data type of the vector components.
 */
#define VEC3_ADD(T) \
MentalResult vec3_add_##T(const Vec3_##T* a, const Vec3_##T* b, Vec3_##T* result) { \
    if (!a || !b || !result) return MENTAL_FATAL; \
    result->x = a->x + b->x; \
    result->y = a->y + b->y; \
    result->z = a->z + b->z; \
    return MENTAL_OK; \
}

/**
 * @def VEC3_SUB(T)
 * @brief Defines a function to subtract two Vec3_T vectors.
 * @param T The data type of the vector components.
 */
#define VEC3_SUB(T) \
MentalResult vec3_sub_##T(const Vec3_##T* a, const Vec3_##T* b, Vec3_##T* result) { \
    if (!a || !b || !result) return MENTAL_FATAL; \
    result->x = a->x - b->x; \
    result->y = a->y - b->y; \
    result->z = a->z - b->z; \
    return MENTAL_OK; \
}

/**
 * @def VEC3_SCALE(T)
 * @brief Defines a function to multiply a Vec3_T by a scalar.
 * @param T The data type of the vector components.
 */
#define VEC3_SCALE(T) \
MentalResult vec3_scale_##T(const Vec3_##T* v, T scalar, Vec3_##T* result) { \
    if (!v || !result) return MENTAL_FATAL; \
    result->x = v->x * scalar; \
    result->y = v->y * scalar; \
    result->z = v->z * scalar; \
    return MENTAL_OK; \
}

/**
 * @def VEC3_DIV(T)
 * @brief Defines a function to divide a Vec3_T by a scalar.
 * @param T The data type of the vector components.
 */
#define VEC3_DIV(T) \
MentalResult vec3_div_##T(const Vec3_##T* v, T scalar, Vec3_##T* result) { \
    if (!v || !result || scalar == (T)0) return MENTAL_FATAL; \
    result->x = v->x / scalar; \
    result->y = v->y / scalar; \
    result->z = v->z / scalar; \
    return MENTAL_OK; \
}

/**
 * @def DEFINE_MAT3(T)
 * @brief Defines a 3x3 matrix structure using Vec3_T for rows.
 * @param T The data type of the matrix components.
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
 */
#define MAT3_ZERO(T) \
MentalResult mat3_zero_##T(Mat3_##T* m) { \
    if (!m) return MENTAL_FATAL; \
    vec3_zero_##T(&m->row0); \
    vec3_zero_##T(&m->row1); \
    vec3_zero_##T(&m->row2); \
    return MENTAL_OK; \
}

/**
 * @def MAT3_ONE(T)
 * @brief Defines a function to set a Mat3_T to one.
 * @param T The data type of the matrix components.
 */
#define MAT3_ONE(T) \
MentalResult mat3_one_##T(Mat3_##T* m) { \
    if (!m) return MENTAL_FATAL; \
    vec3_fill_##T(&m->row0, (T)1); \
    vec3_fill_##T(&m->row1, (T)1); \
    vec3_fill_##T(&m->row2, (T)1); \
    return MENTAL_OK; \
}

/**
 * @def MAT3_IDENTITY(T)
 * @brief Defines a function to set a Mat3_T to the identity matrix.
 * @param T The data type of the matrix components.
 */
#define MAT3_IDENTITY(T) \
MentalResult mat3_identity_##T(Mat3_##T* m) { \
    if (!m) return MENTAL_FATAL; \
    vec3_zero_##T(&m->row0); m->row0.x = (T)1; \
    vec3_zero_##T(&m->row1); m->row1.y = (T)1; \
    vec3_zero_##T(&m->row2); m->row2.z = (T)1; \
    return MENTAL_OK; \
}

/**
 * @def MAT3_FILL(T)
 * @brief Defines a function to fill a Mat3_T with a specified value.
 * @param T The data type of the matrix components.
 */
#define MAT3_FILL(T) \
MentalResult mat3_fill_##T(Mat3_##T* m, T value) { \
    if (!m) return MENTAL_FATAL; \
    vec3_fill_##T(&m->row0, value); \
    vec3_fill_##T(&m->row1, value); \
    vec3_fill_##T(&m->row2, value); \
    return MENTAL_OK; \
}

/**
 * @def MAT3_FILL_CUSTOM(T)
 * @brief Defines a function to fill a Mat3_T with custom values.
 * @param T The data type of the matrix components.
 */
#define MAT3_FILL_CUSTOM(T) \
MentalResult mat3_fill_custom_##T(Mat3_##T* m, \
    T m00, T m01, T m02, \
    T m10, T m11, T m12, \
    T m20, T m21, T m22) { \
    if (!m) return MENTAL_FATAL; \
    m->row0.x = m00; m->row0.y = m01; m->row0.z = m02; \
    m->row1.x = m10; m->row1.y = m11; m->row1.z = m12; \
    m->row2.x = m20; m->row2.y = m21; m->row2.z = m22; \
    return MENTAL_OK; \
}

/**
 * @def MAT3_ADD(T)
 * @brief Defines a function to add two Mat3_T matrices.
 * @param T The data type of the matrix components.
 */
#define MAT3_ADD(T) \
MentalResult mat3_add_##T(const Mat3_##T* a, const Mat3_##T* b, Mat3_##T* result) { \
    if (!a || !b || !result) return MENTAL_FATAL; \
    vec3_add_##T(&a->row0, &b->row0, &result->row0); \
    vec3_add_##T(&a->row1, &b->row1, &result->row1); \
    vec3_add_##T(&a->row2, &b->row2, &result->row2); \
    return MENTAL_OK; \
}

/**
 * @def MAT3_SUB(T)
 * @brief Defines a function to subtract two Mat3_T matrices.
 * @param T The data type of the matrix components.
 */
#define MAT3_SUB(T) \
MentalResult mat3_sub_##T(const Mat3_##T* a, const Mat3_##T* b, Mat3_##T* result) { \
    if (!a || !b || !result) return MENTAL_FATAL; \
    vec3_sub_##T(&a->row0, &b->row0, &result->row0); \
    vec3_sub_##T(&a->row1, &b->row1, &result->row1); \
    vec3_sub_##T(&a->row2, &b->row2, &result->row2); \
    return MENTAL_OK; \
}

/**
 * @def MAT3_SCALE(T)
 * @brief Defines a function to multiply a Mat3_T by a scalar.
 * @param T The data type of the matrix components.
 */
#define MAT3_SCALE(T) \
MentalResult mat3_scale_##T(const Mat3_##T* m, T scalar, Mat3_##T* result) { \
    if (!m || !result) return MENTAL_FATAL; \
    vec3_scale_##T(&m->row0, scalar, &result->row0); \
    vec3_scale_##T(&m->row1, scalar, &result->row1); \
    vec3_scale_##T(&m->row2, scalar, &result->row2); \
    return MENTAL_OK; \
}

/**
 * @def MAT3_DIV(T)
 * @brief Defines a function to divide a Mat3_T by a scalar.
 * @param T The data type of the matrix components.
 */
#define MAT3_DIV(T) \
MentalResult mat3_div_##T(const Mat3_##T* m, T scalar, Mat3_##T* result) { \
    if (!m || !result || scalar == (T)0) return MENTAL_FATAL; \
    vec3_div_##T(&m->row0, scalar, &result->row0); \
    vec3_div_##T(&m->row1, scalar, &result->row1); \
    vec3_div_##T(&m->row2, scalar, &result->row2); \
    return MENTAL_OK; \
}

/**
 * @def MAT3_MUL(T)
 * @brief Defines a function to multiply two Mat3_T matrices.
 * @param T The data type of the matrix components.
 */
#define MAT3_MUL(T) \
MentalResult mat3_mul_##T(const Mat3_##T* a, const Mat3_##T* b, Mat3_##T* result) { \
    if (!a || !b || !result) return MENTAL_FATAL; \
    result->row0.x = a->row0.x * b->row0.x + a->row0.y * b->row1.x + a->row0.z * b->row2.x; \
    result->row0.y = a->row0.x * b->row0.y + a->row0.y * b->row1.y + a->row0.z * b->row2.y; \
    result->row0.z = a->row0.x * b->row0.z + a->row0.y * b->row1.z + a->row0.z * b->row2.z; \
    result->row1.x = a->row1.x * b->row0.x + a->row1.y * b->row1.x + a->row1.z * b->row2.x; \
    result->row1.y = a->row1.x * b->row0.y + a->row1.y * b->row1.y + a->row1.z * b->row2.y; \
    result->row1.z = a->row1.x * b->row0.z + a->row1.y * b->row1.z + a->row1.z * b->row2.z; \
    result->row2.x = a->row2.x * b->row0.x + a->row2.y * b->row1.x + a->row2.z * b->row2.x; \
    result->row2.y = a->row2.x * b->row0.y + a->row2.y * b->row1.y + a->row2.z * b->row2.y; \
    result->row2.z = a->row2.x * b->row0.z + a->row2.y * b->row1.z + a->row2.z * b->row2.z; \
    return MENTAL_OK; \
}

/**
 * @def DEFINE_MATH_STRUCTS(T)
 * @brief Defines Vec3 and Mat3 with all associated operations for a given type.
 * @param T The data type for the vector and matrix components.
 */
#define DEFINE_MATH_STRUCTS(T) \
    DEFINE_VEC3(T); \
    VEC3_ZERO(T); \
    VEC3_ONE(T); \
    VEC3_FILL(T); \
    VEC3_TO_ARRAY(T); \
    VEC3_TO_VECTOR(T); \
    VEC3_ADD(T); \
    VEC3_SUB(T); \
    VEC3_SCALE(T); \
    VEC3_DIV(T); \
    DEFINE_MAT3(T); \
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

// Predefined types for float, int, and double
DEFINE_MATH_STRUCTS(float);  ///< Defines Vec3_float, Mat3_float, and their operations
DEFINE_MATH_STRUCTS(int);    ///< Defines Vec3_int, Mat3_int, and their operations
DEFINE_MATH_STRUCTS(double); ///< Defines Vec3_double, Mat3_double, and their operations

// Users can add their own types by invoking the macro, e.g.:
// DEFINE_MATH_STRUCTS(long);  ///< Example: Defines Vec3_long, Mat3_long, and operations
// DEFINE_MATH_STRUCTS(short); ///< Example: Defines Vec3_short, Mat3_short, and operations

#endif // MATHEMATICA_H