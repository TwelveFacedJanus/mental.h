#!/usr/bin/env python3
"""
Скрипт для конвертации изображений в PPM формат для скайбокса
Использование: python3 convert_to_ppm.py input_image.jpg output.ppm
"""

import sys
from PIL import Image
import os

def convert_to_ppm(input_path, output_path, size=(512, 512)):
    """Конвертирует изображение в PPM формат"""
    try:
        # Открываем изображение
        img = Image.open(input_path)
        
        # Изменяем размер
        img = img.resize(size, Image.Resampling.LANCZOS)
        
        # Конвертируем в RGB если нужно
        if img.mode != 'RGB':
            img = img.convert('RGB')
        
        # Сохраняем в PPM формате
        img.save(output_path, 'PPM')
        
        print(f"Конвертировано: {input_path} -> {output_path}")
        return True
        
    except Exception as e:
        print(f"Ошибка при конвертации {input_path}: {e}")
        return False

def main():
    if len(sys.argv) != 3:
        print("Использование: python3 convert_to_ppm.py input_image.jpg output.ppm")
        print("Пример: python3 convert_to_ppm.py sky_right.jpg textures/skybox/right.ppm")
        return
    
    input_path = sys.argv[1]
    output_path = sys.argv[2]
    
    # Создаем папку если её нет
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    
    if convert_to_ppm(input_path, output_path):
        print("Конвертация завершена успешно!")
    else:
        print("Конвертация не удалась!")

if __name__ == "__main__":
    main()
