include_guard(GLOBAL)
include(FetchContent)

if(POLICY CMP0168)
    cmake_policy(SET CMP0168 NEW)
endif()

# Always use the same sources on every machine, including GLAD from GLFW's
# archive. FetchContent keeps downloads and dependency builds in build/_deps.
set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE NEVER)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
set(GLFW_LIBRARY_TYPE STATIC)
FetchContent_Declare(glfw
    URL https://github.com/glfw/glfw/archive/3.5.1.tar.gz
    URL_HASH SHA256=5234f4f29473e9a06bc7847d8371858dd135d38466eeeaa652fdc9f8f9ff0c20
)

set(GLM_BUILD_LIBRARY OFF)
set(GLM_BUILD_TESTS OFF)
set(GLM_BUILD_INSTALL OFF)
FetchContent_Declare(glm
    URL https://github.com/g-truc/glm/archive/1.0.3.tar.gz
    URL_HASH SHA256=6775e47231a446fd086d660ecc18bcd076531cfedd912fbd66e576b118607001
)
FetchContent_MakeAvailable(glfw glm)

# GLFW ships a generated GLAD 2 OpenGL 3.3 loader for its examples. Its
# single header embeds the Khronos platform definitions, so neither Python
# nor separately installed GLAD/KHR headers are needed. Compile it once.
add_library(glad STATIC "${CMAKE_CURRENT_LIST_DIR}/glad.c")
target_include_directories(glad SYSTEM PUBLIC "${glfw_SOURCE_DIR}/deps")
