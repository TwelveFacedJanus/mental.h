#!/bin/bash

# Настройки
OUTPUT_DIR="docs"
FORMAT="png"  # или svg, pdf 

# Создаем папку для результатов
mkdir -p "$OUTPUT_DIR"

# 1. Генерация графа вызовов функций (исправленная версия cflow)
echo "Анализ вызовов функций..."
find . -name "*.c" | xargs cflow --format=dot --depth=3 --omit-arguments > "$OUTPUT_DIR/call_graph.dot" 2>/dev/null

# 2. Генерация документации и зависимостей (doxygen)
echo "Анализ структур данных и файлов..."
cat > "$OUTPUT_DIR/Doxyfile" <<EOF
PROJECT_NAME      = "Mental.h Project Documentation"
OUTPUT_DIRECTORY  = "$OUTPUT_DIR"
INPUT             = .
RECURSIVE         = YES
EXTRACT_ALL       = YES
HAVE_DOT          = YES
CALL_GRAPH        = YES
CALLER_GRAPH      = YES
DIRECTORY_GRAPH   = YES
CLASS_GRAPH       = YES
COLLABORATION_GRAPH = YES
DOT_IMAGE_FORMAT  = $FORMAT
DOT_GRAPH_MAX_NODES = 200
EOF

doxygen "$OUTPUT_DIR/Doxyfile" 2>/dev/null

# 3. Объединение результатов
echo "Создание итоговой диаграммы..."

# Если cflow создал граф, конвертируем его в изображение
if [ -s "$OUTPUT_DIR/call_graph.dot" ]; then
    dot -T$FORMAT "$OUTPUT_DIR/call_graph.dot" -o "$OUTPUT_DIR/call_graph.$FORMAT"
fi

# Основная диаграмма будет в папке doxygen
echo "Основные диаграммы находятся в:"
echo "- $OUTPUT_DIR/html/index.html (полная документация)"
echo "- $OUTPUT_DIR/html/inherits.html (граф наследования)"
echo "- $OUTPUT_DIR/call_graph.$FORMAT (граф вызовов)"

# Открываем результат (macOS)
open "$OUTPUT_DIR/html/index.html" || echo "Откройте index.html в браузере"