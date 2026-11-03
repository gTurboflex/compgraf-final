cmake_minimum_required(VERSION 3.16)
project(OpenGLApp LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(APPLE)
    set(CMAKE_MACOSX_RPATH 1)
endif()


include_directories(${CMAKE_SOURCE_DIR}/external/glad/include)


find_package(glfw3 REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
    "external/glad/src/glad.c"
)

add_executable(${PROJECT_NAME} ${SOURCES})

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
