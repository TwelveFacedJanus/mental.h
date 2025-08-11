#!/usr/bin/env python3
from PIL import Image
import os

# Создаем папку для текстур, если её нет
os.makedirs('textures/skybox', exist_ok=True)

# Размер текстур
size = 512

# Цвета для каждой грани куба
colors = {
    'right': (255, 100, 100),   # Красный
    'left': (100, 255, 100),    # Зеленый
    'top': (100, 100, 255),     # Синий
    'bottom': (255, 255, 100),  # Желтый
    'front': (255, 100, 255),   # Пурпурный
    'back': (100, 255, 255)     # Голубой
}

# Создаем текстуры для каждой грани
for face, color in colors.items():
    img = Image.new('RGB', (size, size), color)
    
    # Добавляем простой градиент для более интересного вида
    pixels = img.load()
    for x in range(size):
        for y in range(size):
            # Создаем градиент от центра
            center_x, center_y = size // 2, size // 2
            distance = ((x - center_x) ** 2 + (y - center_y) ** 2) ** 0.5
            factor = min(1.0, distance / (size * 0.7))
            
            r = int(color[0] * (1 - factor * 0.3))
            g = int(color[1] * (1 - factor * 0.3))
            b = int(color[2] * (1 - factor * 0.3))
            
            pixels[x, y] = (r, g, b)
    
    filename = f'textures/skybox/{face}.jpg'
    img.save(filename, 'JPEG', quality=90)
    print(f'Создана текстура: {filename}')

print('Все текстуры скайбокса созданы!')
