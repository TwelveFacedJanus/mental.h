#!/usr/bin/env python3
"""
Скрипт для создания простой текстуры для сферы
"""

from PIL import Image, ImageDraw
import math

def create_sphere_texture(size=512):
    """Создает простую текстуру для сферы с градиентом"""
    
    # Создаем изображение
    img = Image.new('RGB', (size, size), color='white')
    draw = ImageDraw.Draw(img)
    
    # Создаем градиент от центра к краям
    center_x, center_y = size // 2, size // 2
    max_distance = math.sqrt(center_x**2 + center_y**2)
    
    for x in range(size):
        for y in range(size):
            # Вычисляем расстояние от центра
            distance = math.sqrt((x - center_x)**2 + (y - center_y)**2)
            
            # Нормализуем расстояние
            normalized_distance = distance / max_distance
            
            # Создаем градиент от синего в центре к белому на краях
            blue = int(255 * (1 - normalized_distance * 0.7))
            green = int(200 * (1 - normalized_distance * 0.5))
            red = int(150 * (1 - normalized_distance * 0.3))
            
            # Ограничиваем значения
            red = max(0, min(255, red))
            green = max(0, min(255, green))
            blue = max(0, min(255, blue))
            
            # Устанавливаем пиксель
            img.putpixel((x, y), (red, green, blue))
    
    return img

def create_checker_texture(size=512, squares=8):
    """Создает шахматную текстуру"""
    
    img = Image.new('RGB', (size, size), color='white')
    draw = ImageDraw.Draw(img)
    
    square_size = size // squares
    
    for i in range(squares):
        for j in range(squares):
            x1 = i * square_size
            y1 = j * square_size
            x2 = x1 + square_size
            y2 = y1 + square_size
            
            # Чередуем цвета
            if (i + j) % 2 == 0:
                color = (100, 150, 255)  # Светло-синий
            else:
                color = (50, 100, 200)   # Темно-синий
            
            draw.rectangle([x1, y1, x2, y2], fill=color)
    
    return img

if __name__ == "__main__":
    # Создаем градиентную текстуру
    gradient_texture = create_sphere_texture(512)
    gradient_texture.save("sphere_texture.png")
    print("Создана градиентная текстура: sphere_texture.png")
    
    # Создаем шахматную текстуру
    checker_texture = create_checker_texture(512, 8)
    checker_texture.save("sphere_checker.png")
    print("Создана шахматная текстура: sphere_checker.png")
    
    print("Текстуры для сферы созданы успешно!")
