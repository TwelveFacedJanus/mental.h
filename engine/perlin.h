#ifndef mental_perlin_h
#define mental_perlin_h

#include "mental.h"

// Функции шума Перлина
void mental_init_perlin();
float mental_perlin_noise(float x, float y, float z);

// Функции для генерации текстуры земли
unsigned char* mental_generate_ground_texture(int width, int height, float scale);
MentalResult mental_save_ground_texture(const char* filename, int width, int height, float scale);

#endif // mental_perlin_h
