cmake_minimum_required(VERSION 3.24)
project(OpenGLApp LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/Dependencies.cmake)

add_executable(OpenGLApp
    src/main.cpp
    src/Mesh.cpp
    src/Rack.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/PerformanceStats.cpp
    src/UI.cpp
)
target_link_libraries(OpenGLApp PRIVATE glfw glm::glm glad)
target_compile_definitions(OpenGLApp PRIVATE
    GLFW_INCLUDE_NONE
    TEXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/textures"
    SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders"
)

include(CTest)
if(BUILD_TESTING)
    add_executable(performance_tests tests/performance_tests.cpp src/PerformanceStats.cpp)
    target_include_directories(performance_tests PRIVATE src)
    add_test(NAME performance_tests COMMAND performance_tests)
endif()
