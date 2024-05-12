# Map metro Sankt Petersburg

## Описание проекта
Map metro spb — это приложение для поиска оптимальных маршрутов в метрополитене Санкт-Петербурга. Программа разработана на языке C и использует графический интерфейс GTK для удобства пользователя. Приложение позволяет пользователю выбрать начальную и конечную станции метро, после чего рассчитывает и показывает наиболее быстрый маршрут и ожидаемое время в пути.

## Основные функции
- **Выбор станции**: Пользователи могут выбрать начальную и конечную станцию из списка всех доступных станций метрополитена Санкт-Петербурга.
- **Расчет маршрута**: На основе введенных данных приложение рассчитывает наиболее оптимальный маршрут.
- **Отображение времени в пути**: Пользователь получает информацию о предполагаемом времени проезда от начальной до конечной станции.
- **Визуализация маршрута**: Маршрут отображается на схеме метро, что делает ориентацию проще и нагляднее.

## Технологии
Проект написан на языке программирования C, что обеспечивает высокую производительность приложения. Для создания графического интерфейса используется библиотека GTK, позволяющая создавать привлекательный и функциональный пользовательский интерфейс.

## Установка и запуск
Для работы с проектом необходимо иметь установленные:
- GCC (GNU Compiler Collection) для компиляции кода на C.
- GTK для работы графического интерфейса.

### Установка и запуск

Для работы с проектом необходимо иметь установленные:
- GCC (GNU Compiler Collection) для компиляции кода на C.
- GTK для работы графического интерфейса.
- SQLite3 для работы с базами данных.

### Установка необходимых компонентов:

#### Windows:
1. **Установка MinGW (включает GCC):**
   - Скачайте и установите MinGW с официального сайта.
   - Добавьте путь к папке bin MinGW в переменную среды PATH.

2. **Установка GTK:**
   - Скачайте и установите GTK с официального сайта или используйте MSYS2 для установки GTK.
   - Добавьте пути к библиотекам и заголовочным файлам GTK в переменные среды.

3. **Установка SQLite3:**
   - Скачайте precompiled binaries для Windows с официального сайта SQLite.
   - Добавьте путь к исполняемому файлу SQLite в переменную среды PATH.

#### macOS:
1. **Установка GCC:**
   - Установите Xcode через App Store, это автоматически установит GCC.
   - Или используйте Homebrew: brew install gcc.

2. **Установка GTK:**
   - Установите Homebrew, если еще не установлен: /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
   - Затем установите GTK: brew install gtk+3.

3. **Установка SQLite3:**
   - SQLite обычно предустановлен на macOS.
   - Для последней версии: brew install sqlite3.

#### Linux (Ubuntu/Debian):
1. **Установка GCC:**
   - Установите GCC через терминал: sudo apt-get install build-essential.

2. **Установка GTK:**
   - Установите GTK: sudo apt-get install libgtk-3-dev.

3. **Установка SQLite3:**
   - Установите SQLite3: sudo apt-get install sqlite3 libsqlite3-dev.

### Шаги для установки и запуска проекта:

1. **Клонирование репозитория:**
   
   git clone https://github.com/yourusername/Map-metro-spb.git
   cd Map-metro-spb
   

2. **Компиляция проекта:**
   
   gcc main.c `pkg-config --cflags --libs gtk+-3.0` -lsqlite3 -o map_metro_spb
   

3. **Запуск приложения:**
   
   ./map_metro_spb
   

После выполнения этих шагов, приложение должно успешно запуститься на вашей системе.
   
## Как использовать?
После запуска приложения:
1. Выберите начальную станцию.
2. Выберите конечную станцию.
3. Просмотрите предложенный маршрут и время в пути на интерактивной карте метро.
