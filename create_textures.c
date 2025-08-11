#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Простая структура для RGB пикселя
typedef struct {
    unsigned char r, g, b;
} RGB;

// Функция для создания градиента неба
void create_sky_gradient(RGB* pixels, int width, int height, RGB top_color, RGB bottom_color) {
    for (int y = 0; y < height; y++) {
        float t = (float)y / (height - 1);
        RGB color;
        color.r = (unsigned char)(top_color.r * (1 - t) + bottom_color.r * t);
        color.g = (unsigned char)(top_color.g * (1 - t) + bottom_color.g * t);
        color.b = (unsigned char)(top_color.b * (1 - t) + bottom_color.b * t);
        
        for (int x = 0; x < width; x++) {
            pixels[y * width + x] = color;
        }
    }
}

// Функция для создания текстуры с облаками
void create_cloud_texture(RGB* pixels, int width, int height, RGB sky_color) {
    // Создаем базовый градиент неба
    for (int y = 0; y < height; y++) {
        float t = (float)y / (height - 1);
        RGB color;
        color.r = (unsigned char)(sky_color.r * (1 - t * 0.3f));
        color.g = (unsigned char)(sky_color.g * (1 - t * 0.2f));
        color.b = (unsigned char)(sky_color.b * (1 - t * 0.1f));
        
        for (int x = 0; x < width; x++) {
            pixels[y * width + x] = color;
        }
    }
    
    // Добавляем облака разных размеров
    for (int cloud = 0; cloud < 8; cloud++) {
        int center_x = rand() % width;
        int center_y = rand() % (height * 2 / 3) + height / 6;
        int radius = 20 + rand() % 80;
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                float distance = sqrt((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y));
                if (distance < radius) {
                    float alpha = 1.0f - (distance / radius);
                    alpha = alpha * alpha * alpha; // Более мягкие края
                    
                    // Разные оттенки облаков
                    RGB cloud_color = {240 + rand() % 15, 240 + rand() % 15, 240 + rand() % 15};
                    RGB current = pixels[y * width + x];
                    
                    pixels[y * width + x].r = (unsigned char)(current.r * (1 - alpha) + cloud_color.r * alpha);
                    pixels[y * width + x].g = (unsigned char)(current.g * (1 - alpha) + cloud_color.g * alpha);
                    pixels[y * width + x].b = (unsigned char)(current.b * (1 - alpha) + cloud_color.b * alpha);
                }
            }
        }
    }
}

// Функция для создания простой текстуры
void create_texture(const char* filename, RGB base_color, int type) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Не удалось создать файл: %s\n", filename);
        return;
    }
    
    int width = 512;
    int height = 512;
    
    // Записываем заголовок PPM
    fprintf(file, "P6\n%d %d\n255\n", width, height);
    
    // Выделяем память для пикселей
    RGB* pixels = malloc(width * height * sizeof(RGB));
    if (!pixels) {
        printf("Не удалось выделить память\n");
        fclose(file);
        return;
    }
    
    if (type == 0) {
        // Обычная текстура с градиентом
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                RGB pixel = base_color;
                
                // Создаем градиент от центра
                int center_x = width / 2;
                int center_y = height / 2;
                float distance = sqrt((x - center_x) * (x - center_x) + (y - center_y) * (y - center_y));
                float factor = distance / (width * 0.7f);
                if (factor > 1.0f) factor = 1.0f;
                
                pixel.r = (unsigned char)(base_color.r * (1.0f - factor * 0.3f));
                pixel.g = (unsigned char)(base_color.g * (1.0f - factor * 0.3f));
                pixel.b = (unsigned char)(base_color.b * (1.0f - factor * 0.3f));
                
                pixels[y * width + x] = pixel;
            }
        }
    } else if (type == 1) {
        // Градиент неба для верхней грани
        RGB top_color = {135, 206, 235}; // Светло-голубой
        RGB bottom_color = {70, 130, 180}; // Стальной синий
        create_sky_gradient(pixels, width, height, top_color, bottom_color);
    } else if (type == 2) {
        // Градиент неба с облаками для боковых граней
        RGB sky_color = {135, 206, 235}; // Светло-голубой
        create_cloud_texture(pixels, width, height, sky_color);
    }
    
    // Записываем пиксели в файл
    fwrite(pixels, sizeof(RGB), width * height, file);
    free(pixels);
    fclose(file);
    printf("Создана текстура: %s\n", filename);
}

int main() {
    // Создаем папку для текстур
    system("mkdir -p textures/skybox");
    
    // Создаем текстуры для каждой грани
    create_texture("textures/skybox/right.ppm", (RGB){135, 206, 235}, 2);  // Небо с облаками
    create_texture("textures/skybox/left.ppm", (RGB){135, 206, 235}, 2);   // Небо с облаками
    create_texture("textures/skybox/top.ppm", (RGB){135, 206, 235}, 1);    // Градиент неба
    create_texture("textures/skybox/bottom.ppm", (RGB){34, 139, 34}, 0);   // Зеленая земля
    create_texture("textures/skybox/front.ppm", (RGB){135, 206, 235}, 2);  // Небо с облаками
    create_texture("textures/skybox/back.ppm", (RGB){135, 206, 235}, 2);   // Небо с облаками
    
    printf("Все текстуры созданы!\n");
    return 0;
}
