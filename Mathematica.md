# Usage example

```c

/**
 * @brief Example usage of the mathematica.h library for Vec3 and Mat3 operations.
 *
 * This program demonstrates how to use the Vec3 and Mat3 types with float,
 * including initialization, filling, and conversion to C arrays and C++ std::vector.
 */

#include "Mental.h"

#ifdef __cplusplus
#include <vector>
#include <iostream>
#endif

DEFINE_MATH_STRUCTS(MentalResult);

int main() {
    Vec3_float v1;
    vec3_zero_float(&v1);
    printf("Zero vector: (%f, %f, %f)\n", v1.x, v1.y, v1.z);

    vec3_one_float(&v1);
    printf("One vector: (%f, %f, %f)\n", v1.x, v1.y, v1.z);

    vec3_fill_float(&v1, 5.0f);
    printf("Filled vector: (%f, %f, %f)\n", v1.x, v1.y, v1.z);

    Vec3_MentalResult vm = {MENTAL_OK, MENTAL_FATAL, MENTAL_ERROR};
    printf("Filled with MentalResult: (%d, %d, %d)\n", vm.x, vm.y, vm.z);
    MentalResult arrvm[3];
    vec3_to_array_MentalResult(&vm, arrvm);
    printf("C array with MentalResult: (%d, %d, %d)\n", arrvm[0], arrvm[1], arrvm[2]);

    float arr[3];
    vec3_to_array_float(&v1, arr);
    printf("C array: [%f, %f, %f]\n", arr[0], arr[1], arr[2]);

#ifdef __cplusplus
    std::vector<float> vec = vec3_to_vector_float(&v1);
    std::cout << "C++ vector: [" << vec[0] << ", " << vec[1] << ", " << vec[2] << "]\n";
#endif

    Mat3_float m1;
    mat3_identity_float(&m1);
    printf("Identity matrix:\n");
    printf("[%f, %f, %f]\n", m1.row0.x, m1.row0.y, m1.row0.z);
    printf("[%f, %f, %f]\n", m1.row1.x, m1.row1.y, m1.row1.z);
    printf("[%f, %f, %f]\n", m1.row2.x, m1.row2.y, m1.row2.z);

    mat3_fill_custom_float(&m1, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
    printf("Custom matrix:\n");
    printf("[%f, %f, %f]\n", m1.row0.x, m1.row0.y, m1.row0.z);
    printf("[%f, %f, %f]\n", m1.row1.x, m1.row1.y, m1.row1.z);
    printf("[%f, %f, %f]\n", m1.row2.x, m1.row2.y, m1.row2.z);


    Vec3_double v2;
    vec3_fill_double(&v2, 10.0);
    double arr2[3];
    vec3_to_array_double(&v2, arr2);
    printf("Double vector: (%f, %f, %f)\n", v2.x, v2.y, v2.z);
    printf("Double C array: [%f, %f, %f]\n", arr2[0], arr2[1], arr2[2]);

    return 0;
}

```