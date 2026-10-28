cmake_minimum_required(VERSION 3.16)
project(OpenGLApp LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Для корректной сборки под Apple Silicon (arm64) / Intel
if(APPLE)
    set(CMAKE_MACOSX_RPATH 1)
endif()

# Подключаем GLAD из локальной папки
include_directories(${CMAKE_SOURCE_DIR}/external/glad/include)

# Ищем системный GLFW (установленный через Homebrew)
find_package(glfw3 REQUIRED)

# Исходные файлы
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "external/glad/src/glad.c"
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Линковка GLFW и системных фреймворков macOS
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    "-framework OpenGL"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
)