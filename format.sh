#!/bin/bash
# script: format-code.sh
# Использование: ./format-code.sh [директории...]
# Цвета для вывода
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color
# Проверка наличия clang-format
if ! command -v clang-format &> /dev/null; then
    echo -e "${RED}❌ Ошибка: clang-format не найден${NC}"
    echo "Установите clang-format:"
    echo "  Ubuntu: sudo apt-get install clang-format"
    echo "  CentOS: sudo yum install clang-tools-extra"
    echo "  macOS:  brew install clang-format"
    exit 1
fi
# Директории по умолчанию, если не указаны аргументы
DIRECTORIES=("app" "lib/core" "lib/media" "lib/network")
# Расширения файлов для форматирования
FILE_EXTENSIONS=("*.cpp" "*.c" "*.h" "*.hpp" "*.cc" "*.cxx" "*.hxx")
total_files=0
formatted_files=0
for dir in "${DIRECTORIES[@]}"
do
    echo -e "${YELLOW}📁 Обработка директории: $dir${NC}"
    # Обработка каждого расширения файлов
    for extension in "${FILE_EXTENSIONS[@]}"
        do
        # Поиск файлов и их форматирование
        while IFS= read -r -d '' file; do
            if [ -f "$file" ]; then
                echo "Форматирование: $file"
                clang-format -i -style=file "$file"
                ((formatted_files++))
            fi
            ((total_files++))
        done < <(find "$dir" -name "$extension" -type f -print0 2>/dev/null)
    done
done
echo -e "${GREEN} Готово! Обработано файлов: $formatted_files/$total_files${NC}"
