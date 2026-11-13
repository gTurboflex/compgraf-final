cmake_minimum_required(VERSION 3.24)
project(OpenGLApp LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/Dependencies.cmake)

find_package(Threads REQUIRED)
add_library(server_audio STATIC src/Audio.cpp src/AudioSynth.cpp external/miniaudio/miniaudio.c)
target_include_directories(server_audio PUBLIC src)
target_include_directories(server_audio SYSTEM PRIVATE external/miniaudio)
target_compile_definitions(server_audio PRIVATE MA_NO_DECODING MA_NO_ENCODING MA_NO_ENGINE MA_NO_RESOURCE_MANAGER MA_NO_NODE_GRAPH MA_NO_GENERATION)
target_link_libraries(server_audio PRIVATE Threads::Threads ${CMAKE_DL_LIBS})
if(UNIX AND NOT APPLE)
    target_link_libraries(server_audio PRIVATE m)
endif()

add_executable(OpenGLApp
    src/main.cpp
    src/Mesh.cpp
    src/Geometry.cpp
    src/Rack.cpp
    src/Scene.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/PerformanceStats.cpp
    src/UI.cpp
    src/Camera.cpp
    src/Input.cpp
)
target_link_libraries(OpenGLApp PRIVATE glfw glm::glm glad server_audio)
target_compile_definitions(OpenGLApp PRIVATE
    GLFW_INCLUDE_NONE
    TEXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/textures"
    SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders"
)

include(CTest)
if(BUILD_TESTING)
    add_executable(geometry_tests tests/geometry_tests.cpp src/Geometry.cpp)
    target_include_directories(geometry_tests PRIVATE src)
    target_link_libraries(geometry_tests PRIVATE glm::glm glad)
    add_test(NAME geometry_tests COMMAND geometry_tests)
    add_executable(control_tests tests/control_tests.cpp src/Camera.cpp)
    target_include_directories(control_tests PRIVATE src)
    target_link_libraries(control_tests PRIVATE glm::glm)
    add_test(NAME control_tests COMMAND control_tests)
    add_executable(interaction_tests tests/interaction_tests.cpp src/Rack.cpp src/Scene.cpp src/Mesh.cpp src/Shader.cpp src/Texture.cpp)
    target_include_directories(interaction_tests PRIVATE src)
    target_compile_definitions(interaction_tests PRIVATE TEXTURE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/assets/textures")
    target_link_libraries(interaction_tests PRIVATE glm::glm glad)
    add_test(NAME interaction_tests COMMAND interaction_tests)
    add_executable(performance_tests tests/performance_tests.cpp src/PerformanceStats.cpp)
    target_include_directories(performance_tests PRIVATE src)
    add_test(NAME performance_tests COMMAND performance_tests)
    add_executable(audio_tests tests/audio_tests.cpp src/AudioSynth.cpp)
    target_include_directories(audio_tests PRIVATE src)
    add_test(NAME audio_tests COMMAND audio_tests)
    add_executable(audio_device_check tests/audio_device_check.cpp)
    target_link_libraries(audio_device_check PRIVATE server_audio)
endif()
