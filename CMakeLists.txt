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
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
if(APPLE)
target_link_libraries(${PROJECT_NAME} PRIVATE
    "-framework OpenGL"
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
)
endif()
find_package(glm CONFIG REQUIRED)
target_link_libraries(OpenGLApp PRIVATE glm::glm)
target_compile_definitions(OpenGLApp PRIVATE TEXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/textures" SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders")

include(CTest)
if(BUILD_TESTING)
    add_executable(performance_tests tests/performance_tests.cpp src/PerformanceStats.cpp)
    target_include_directories(performance_tests PRIVATE src)
    add_test(NAME performance_tests COMMAND performance_tests)
endif()
