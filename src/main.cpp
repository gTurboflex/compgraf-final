#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Mesh.h"
#include "Lighting.h"
#include "Texture.h"
#include "ServerEffects.h"
#include "PerformanceStats.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string>

using namespace std;
using namespace glm;

struct Shaders {
    Shader lit;
    Shader led;
};

struct Materials {
    Texture floorTexture{TEXTURE_DIR "/floor.pgm"};
    Texture metalTexture{TEXTURE_DIR "/metal.pgm"};
    Texture wallTexture{TEXTURE_DIR "/wall.pgm"};
    Texture panelTexture{TEXTURE_DIR "/panel.pgm"};

    Material floor{floorTexture, .50f, .56f, .61f, 1.f, 28.f, .18f};
    Material wall{wallTexture, .62f, .68f, .70f, .6f, 12.f, .08f};
    Material metal{metalTexture, .25f, .32f, .38f, 1.f, 80.f, .52f};
    Material panel{panelTexture, .42f, .48f, .53f, 2.f, 44.f, .32f};
};

GLFWwindow* setupOpenGL() {
    if (!glfwInit()) {
        throw runtime_error("Could not initialize GLFW.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(
        1280, 720, "Server Room | A: alarm | L: ceiling | F: fog | Esc: exit",
        nullptr, nullptr);
    if (!window) {
        throw runtime_error("Could not create the window.");
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    if (!gladLoadGL(glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        throw runtime_error("Could not initialize GLAD.");
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.008f, 0.012f, 0.025f, 1.0f);
    return window;
}

Shaders setupShaders() {
    return {
        Shader(SHADER_DIR "/lit.vert", SHADER_DIR "/lit.frag"),
        Shader(SHADER_DIR "/led.vert", SHADER_DIR "/led.frag")
    };
}

Mesh setupGeometry() {
    vector<Vertex> vertices;
    vector<GLuint> indices;
    const GLuint faceIndices[] = {0, 1, 2, 0, 2, 3};
    const float corners[][2] = {
        {-0.5f, -0.5f}, 
        {0.5f, -0.5f}, 
        {0.5f, 0.5f}, 
        {-0.5f, 0.5f}
    };

    for (int axis = 0; axis < 3; axis++) {
        for (int sign : {-1, 1}) {
            const GLuint baseVertex = static_cast<GLuint>(vertices.size());
            for (int i = 0; i < 4; i++) {
                vec3 position(0), normal(0);
                position[axis] = sign * 0.5f;
                normal[axis] = float(sign);
                position[(axis + 1) % 3] = corners[i][0];
                position[(axis + 2) % 3] = corners[i][1];
                vertices.push_back({position, normal});
            }
            for (GLuint index : faceIndices) indices.push_back(baseVertex + index);
        }
    }

    return Mesh(vertices, indices);
}

void processInput(GLFWwindow* window, Lighting& lighting, bool& fog) {
    static bool lastA = false, lastL = false, lastF = false;
    bool a = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    bool l = glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS;
    bool f = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;

    if (a && !lastA) lighting.toggleAlarm();
    if (l && !lastL) lighting.toggleCeiling();
    if (f && !lastF) fog = !fog;
    lastA = a;
    lastL = l;
    lastF = f;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void drawObject(const Mesh& mesh, Shader& shader, vec3 position, vec3 scale, bool quad = false) {
    shader.use();
    auto model = glm::scale(translate(mat4(1), position), scale);
    shader.setMat4("model", value_ptr(model));
    if (quad) mesh.draw(30, 6);
    else mesh.draw();
}

void renderScene(Shaders& shaders, Materials& materials, Lighting& lighting, bool fog, const Mesh& mesh, int width, int height) {
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time = float(glfwGetTime());
    vec3 eye(7, 5, 10);
    auto view = lookAt(eye, vec3(0, 1, -1), vec3(0, 1, 0));
    auto projection = perspective(radians(48.f), float(width) / height, 0.1f, 100.f);

    for (Shader* shader : {&shaders.lit, &shaders.led}) {
        shader->use();
        shader->setMat4("view", value_ptr(view));
        shader->setMat4("projection", value_ptr(projection));
        shader->setVec3("eye", eye.x, eye.y, eye.z);
        shader->setInt("fogEnabled", fog);
    }
    lighting.apply(shaders.lit, time);

    materials.floor.apply(shaders.lit);
    drawObject(mesh, shaders.lit, {0, -0.1f, -1}, {10, 0.2f, 8});
    materials.wall.apply(shaders.lit);
    drawObject(mesh, shaders.lit, {0, 2, -5}, {10, 4, 0.15f});
    drawObject(mesh, shaders.lit, {-5, 2, -1}, {0.15f, 4, 8});

    for (int rack = 0; rack < 4; rack++) {
        float x = -3.f + rack * 2.f;
        materials.metal.apply(shaders.lit);
        drawObject(mesh, shaders.lit, {x, 1.5f, -2}, {1.4f, 3, 1.5f});

        for (int row = 0; row < 8; row++) {
            materials.panel.apply(shaders.lit);
            drawObject(mesh, shaders.lit, {x, 0.25f + row * 0.35f, -1.22f}, {1.25f, 0.28f, 0.05f});
            
            for (int k = 0; k < 3; k++) {
                ServerEffects::apply(shaders.led, lighting, time, rack * 24 + row * 3 + k);
                drawObject(mesh, shaders.led, {x - 0.45f + k * 0.14f, 0.25f + row * 0.35f, -1.18f}, {0.065f, 0.065f, 0.01f}, true);
            }
        }
    }

    if (glGetError() != GL_NO_ERROR) {
        throw runtime_error("OpenGL rendering error");
    }
}

void updateFPS(GLFWwindow* window, PerformanceStats& stats, double frameTime) {
    if (stats.addFrame(frameTime)) {
        ostringstream title;
        title << "Server Room | FPS " << fixed << setprecision(1) << stats.fps() << " | " << setprecision(2) << stats.frameMs() << " ms" << " | A: alarm | L: ceiling | F: fog | Esc: exit";
        glfwSetWindowTitle(window, title.str().c_str());
    }
}

void run(GLFWwindow* window, Shaders& shaders, const Mesh& mesh, bool smokeTest) {
    Materials materials;
    Lighting lighting;
    PerformanceStats stats;
    bool fog = true;
    int frames = 0;
    double previous = glfwGetTime();
    double started = previous;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        processInput(window, lighting, fog);
        if (glfwWindowShouldClose(window)) break;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if (width <= 0 || height <= 0) {
            glfwWaitEventsTimeout(.1);
            previous = glfwGetTime();
            continue;
        }

        renderScene(shaders, materials, lighting, fog, mesh, width, height);
        glfwSwapBuffers(window);

        double now = glfwGetTime();
        updateFPS(window, stats, now - previous);
        previous = now;
        ++frames;

        if (smokeTest && now - started >= 1.2) {
            if (stats.fps() <= 0) throw runtime_error("FPS counter did not update");
            break;
        }
        if (smokeTest && frames <= 4) {
            fog = !fog;
            lighting.toggleAlarm();
            lighting.toggleCeiling();
        }
    }
}

void cleanup(GLFWwindow* window) {
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}

int main(int argc, char** argv) {
    GLFWwindow* window = nullptr;
    bool smokeTest = argc > 1 && string(argv[1]) == "--smoke-test";
    int result = 0;

    try {
        window = setupOpenGL();
        Shaders shaders = setupShaders();
        Mesh mesh = setupGeometry();
        run(window, shaders, mesh, smokeTest);
    } catch (const exception& e) {
        cerr << e.what() << '\n';
        result = 1;
    }

    cleanup(window);
    return result;
}
