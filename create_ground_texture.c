#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "engine/perlin.h"

int main() {
    // Создаем папку для текстур
    system("mkdir -p textures");
    
    // Генерируем текстуру земли с шумом Перлина
    printf("Генерация текстуры земли с шумом Перлина...\n");
    
    if (mental_save_ground_texture("textures/ground.ppm", 512, 512, 8.0f) == MENTAL_OK) {
        printf("Текстура земли создана: textures/ground.ppm\n");
    } else {
        printf("Ошибка при создании текстуры земли\n");
        return 1;
    }
    
    printf("Все текстуры созданы успешно!\n");
    return 0;
}
