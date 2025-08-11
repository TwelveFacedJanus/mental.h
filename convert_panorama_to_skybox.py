#!/usr/bin/env python3
"""
Скрипт для конвертации панорамного изображения в 6 граней скайбокса
Использование: python3 convert_panorama_to_skybox.py sky_41_2k.png
"""

import sys
import os
from PIL import Image
import math

def create_face_from_panorama(panorama, face_size, face_name):
    """Создает одну грань куба из панорамного изображения"""
    face = Image.new('RGB', (face_size, face_size))
    
    # Координаты для каждой грани куба
    # Эти координаты определяют, какую часть панорамы использовать для каждой грани
    face_coords = {
        'right': (0, 0.25, 0.25, 0.75),      # Правая грань
        'left': (0.75, 0.25, 1.0, 0.75),     # Левая грань  
        'top': (0.25, 0, 0.75, 0.25),        # Верхняя грань
        'bottom': (0.25, 0.75, 0.75, 1.0),   # Нижняя грань
        'front': (0.25, 0.25, 0.75, 0.75),   # Передняя грань
        'back': (0.5, 0.25, 1.0, 0.75)       # Задняя грань
    }
    
    if face_name not in face_coords:
        print(f"Неизвестная грань: {face_name}")
        return None
    
    x1, y1, x2, y2 = face_coords[face_name]
    
    # Вычисляем координаты в пикселях
    pano_width, pano_height = panorama.size
    start_x = int(x1 * pano_width)
    start_y = int(y1 * pano_height)
    end_x = int(x2 * pano_width)
    end_y = int(y2 * pano_height)
    
    # Вырезаем область из панорамы
    region = panorama.crop((start_x, start_y, end_x, end_y))
    
    # Изменяем размер до нужного размера грани
    face = region.resize((face_size, face_size), Image.Resampling.LANCZOS)
    
    return face

def convert_panorama_to_skybox(input_path, output_dir="textures/skybox", face_size=512):
    """Конвертирует панорамное изображение в 6 граней скайбокса"""
    
    try:
        # Открываем панорамное изображение
        panorama = Image.open(input_path)
        print(f"Загружено панорамное изображение: {input_path}")
        print(f"Размер: {panorama.size}")
        
        # Создаем папку для выходных файлов
        os.makedirs(output_dir, exist_ok=True)
        
        # Создаем каждую грань
        faces = ['right', 'left', 'top', 'bottom', 'front', 'back']
        
        for face_name in faces:
            face = create_face_from_panorama(panorama, face_size, face_name)
            if face:
                output_path = os.path.join(output_dir, f"{face_name}.ppm")
                face.save(output_path, 'PPM')
                print(f"Создана грань: {output_path}")
        
        print(f"\nВсе грани созданы в папке: {output_dir}")
        print("Теперь можно запустить программу с новым скайбоксом!")
        
    except Exception as e:
        print(f"Ошибка при конвертации: {e}")
        return False
    
    return True

def main():
    if len(sys.argv) != 2:
        print("Использование: python3 convert_panorama_to_skybox.py <panorama_image>")
        print("Пример: python3 convert_panorama_to_skybox.py sky_41_2k.png")
        return
    
    input_path = sys.argv[1]
    
    if not os.path.exists(input_path):
        print(f"Файл не найден: {input_path}")
        return
    
    if convert_panorama_to_skybox(input_path):
        print("Конвертация завершена успешно!")
    else:
        print("Конвертация не удалась!")

if __name__ == "__main__":
    main()
