```c
/**
 * @brief Example usage of the mathematica.h library for Vec3 and Mat3 operations.
 *
 * This program demonstrates how to use the Vec3 and Mat3 types with float,
 * including initialization, filling, arithmetic operations, and conversion to
 * C arrays and C++ std::vector.
 */

#include "mathematica.h"

#ifdef __cplusplus
#include <vector>
#include <iostream>
#endif

// Helper function to print MentalResult status
void print_result_status(MentalResult result) {
    switch (result) {
        case MENTAL_OK:
            printf("Operation successful\n");
            break;
        case MENTAL_FATAL:
            printf("Operation failed: Invalid input\n");
            break;
        case MENTAL_ERROR:
            printf("Operation failed.\n");
            break;
        default:
            printf("Unknown result status\n");
    }
}

int main() {
    // Vector operations with float
    Vec3_float v1, v2, v_result;

    // Initialize and print zero vector
    vec3_zero_float(&v1);
    printf("Zero vector: (%f, %f, %f)\n", v1.x, v1.y, v1.z);

    // Initialize and print one vector
    vec3_one_float(&v1);
    printf("One vector: (%f, %f, %f)\n", v1.x, v1.y, v1.z);

    // Fill vector with a value
    vec3_fill_float(&v1, 5.0f);
    printf("Filled vector: (%f, %f, %f)\n", v1.x, v1.y, v1.z);

    // Vector arithmetic operations
    vec3_fill_float(&v1, 2.0f);
    vec3_fill_float(&v2, 3.0f);

    // Vector addition
    MentalResult result = vec3_add_float(&v1, &v2, &v_result);
    printf("Vector addition (v1 + v2): (%f, %f, %f)\n", v_result.x, v_result.y, v_result.z);
    print_result_status(result);

    // Vector subtraction
    result = vec3_sub_float(&v1, &v2, &v_result);
    printf("Vector subtraction (v1 - v2): (%f, %f, %f)\n", v_result.x, v_result.y, v_result.z);
    print_result_status(result);

    // Vector scalar multiplication
    result = vec3_scale_float(&v1, 2.0f, &v_result);
    printf("Vector scalar multiplication (v1 * 2): (%f, %f, %f)\n", v_result.x, v_result.y, v_result.z);
    print_result_status(result);

    // Vector scalar division
    result = vec3_div_float(&v1, 2.0f, &v_result);
    printf("Vector scalar division (v1 / 2): (%f, %f, %f)\n", v_result.x, v_result.y, v_result.z);
    print_result_status(result);

    // Test division by zero
    result = vec3_div_float(&v1, 0.0f, &v_result);
    printf("Vector division by zero test: ");
    print_result_status(result);

    // Convert vector to C array
    float arr[3];
    result = vec3_to_array_float(&v1, arr);
    printf("C array: [%f, %f, %f]\n", arr[0], arr[1], arr[2]);
    print_result_status(result);

#ifdef __cplusplus
    // Convert vector to C++ std::vector
    std::vector<float> vec = vec3_to_vector_float(&v1);
    std::cout << "C++ vector: [" << vec[0] << ", " << vec[1] << ", " << vec[2] << "]\n";
#endif

    // Matrix operations with float
    Mat3_float m1, m2, m_result;

    // Initialize and print identity matrix
    mat3_identity_float(&m1);
    printf("Identity matrix:\n");
    printf("[%f, %f, %f]\n", m1.row0.x, m1.row0.y, m1.row0.z);
    printf("[%f, %f, %f]\n", m1.row1.x, m1.row1.y, m1.row1.z);
    printf("[%f, %f, %f]\n", m1.row2.x, m1.row2.y, m1.row2.z);

    // Fill matrix with custom values
    mat3_fill_custom_float(&m1, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
    printf("Custom matrix m1:\n");
    printf("[%f, %f, %f]\n", m1.row0.x, m1.row0.y, m1.row0.z);
    printf("[%f, %f, %f]\n", m1.row1.x, m1.row1.y, m1.row1.z);
    printf("[%f, %f, %f]\n", m1.row2.x, m1.row2.y, m1.row2.z);

    // Initialize second matrix
    mat3_fill_float(&m2, 2.0f);
    printf("Matrix m2 (filled with 2):\n");
    printf("[%f, %f, %f]\n", m2.row0.x, m2.row0.y, m2.row0.z);
    printf("[%f, %f, %f]\n", m2.row1.x, m2.row1.y, m2.row1.z);
    printf("[%f, %f, %f]\n", m2.row2.x, m2.row2.y, m2.row2.z);

    // Matrix addition
    result = mat3_add_float(&m1, &m2, &m_result);
    printf("Matrix addition (m1 + m2):\n");
    printf("[%f, %f, %f]\n", m_result.row0.x, m_result.row0.y, m_result.row0.z);
    printf("[%f, %f, %f]\n", m_result.row1.x, m_result.row1.y, m_result.row1.z);
    printf("[%f, %f, %f]\n", m_result.row2.x, m_result.row2.y, m_result.row2.z);
    print_result_status(result);

    // Matrix subtraction
    result = mat3_sub_float(&m1, &m2, &m_result);
    printf("Matrix subtraction (m1 - m2):\n");
    printf("[%f, %f, %f]\n", m_result.row0.x, m_result.row0.y, m_result.row0.z);
    printf("[%f, %f, %f]\n", m_result.row1.x, m_result.row1.y, m_result.row1.z);
    printf("[%f, %f, %f]\n", m_result.row2.x, m_result.row2.y, m_result.row2.z);
    print_result_status(result);

    // Matrix scalar multiplication
    result = mat3_scale_float(&m1, 2.0f, &m_result);
    printf("Matrix scalar multiplication (m1 * 2):\n");
    printf("[%f, %f, %f]\n", m_result.row0.x, m_result.row0.y, m_result.row0.z);
    printf("[%f, %f, %f]\n", m_result.row1.x, m_result.row1.y, m_result.row1.z);
    printf("[%f, %f, %f]\n", m_result.row2.x, m_result.row2.y, m_result.row2.z);
    print_result_status(result);

    // Matrix scalar division
    result = mat3_div_float(&m1, 2.0f, &m_result);
    printf("Matrix scalar division (m1 / 2):\n");
    printf("[%f, %f, %f]\n", m_result.row0.x, m_result.row0.y, m_result.row0.z);
    printf("[%f, %f, %f]\n", m_result.row1.x, m_result.row1.y, m_result.row1.z);
    printf("[%f, %f, %f]\n", m_result.row2.x, m_result.row2.y, m_result.row2.z);
    print_result_status(result);

    // Matrix multiplication
    result = mat3_mul_float(&m1, &m2, &m_result);
    printf("Matrix multiplication (m1 * m2):\n");
    printf("[%f, %f, %f]\n", m_result.row0.x, m_result.row0.y, m_result.row0.z);
    printf("[%f, %f, %f]\n", m_result.row1.x, m_result.row1.y, m_result.row1.z);
    printf("[%f, %f, %f]\n", m_result.row2.x, m_result.row2.y, m_result.row2.z);
    print_result_status(result);

    // Test matrix division by zero
    result = mat3_div_float(&m1, 0.0f, &m_result);
    printf("Matrix division by zero test: ");
    print_result_status(result);

    // Double vector example
    Vec3_double v3;
    vec3_fill_double(&v3, 10.0);
    double arr2[3];
    vec3_to_array_double(&v3, arr2);
    printf("Double vector: (%f, %f, %f)\n", v3.x, v3.y, v3.z);
    printf("Double C array: [%f, %f, %f]\n", arr2[0], arr2[1], arr2[2]);

    return 0;
}
```