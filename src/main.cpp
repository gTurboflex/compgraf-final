#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Mesh.h"
#include "Lighting.h"
#include "Texture.h"
#include "Rack.h"
#include "PerformanceStats.h"
#include "UI.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string>

using namespace std;
using namespace glm;

struct RunOptions {
    bool smokeTest = false;
    bool benchmark = false;
    string mode = "normal";
    string output;
    string capture;
};

RunOptions readOptions(int argc, char** argv) {
    RunOptions options;
    for (int i = 1; i < argc; ++i) {
        const string arg = argv[i];
        if (arg == "--smoke-test") options.smokeTest = true;
        else if (arg == "--benchmark" && i + 1 < argc) {
            options.benchmark = true;
            options.mode = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) options.output = argv[++i];
        else if (arg == "--capture" && i + 1 < argc) options.capture = argv[++i];
        else throw runtime_error("Usage: OpenGLApp [--smoke-test] [--benchmark normal|alarm|ceiling-off] [--output result.json] [--capture frame.ppm]");
    }
    if (options.mode != "normal" && options.mode != "alarm" && options.mode != "ceiling-off")
        throw runtime_error("Unknown benchmark mode");
    if (options.smokeTest && options.benchmark)
        throw runtime_error("Choose smoke test or benchmark");
    if (!options.output.empty() && !options.benchmark)
        throw runtime_error("--output requires --benchmark");
    return options;
}

void captureFrame(const string& path, int width, int height) {
    vector<unsigned char> pixels(static_cast<size_t>(width) * height * 3);
    GLint alignment;
    glGetIntegerv(GL_PACK_ALIGNMENT, &alignment);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    glPixelStorei(GL_PACK_ALIGNMENT, alignment);
    ofstream file(path, ios::binary);
    file << "P6\n" << width << ' ' << height << "\n255\n";
    for (int y = height - 1; y >= 0; --y)
        file.write(reinterpret_cast<const char*>(pixels.data() + static_cast<size_t>(y) * width * 3), width * 3);
    if (!file) throw runtime_error("Cannot write frame capture");
}

void writeBenchmark(const RunOptions& options, const PerformanceStats& stats, int width, int height) {
    const auto data = stats.benchmark();
    ofstream file;
    if (!options.output.empty()) {
        file.open(options.output);
        if (!file) throw runtime_error("Cannot write benchmark result");
    }
    ostream& out = options.output.empty() ? cout : file;
    out << fixed << setprecision(3) << "{\n  \"mode\": " << quoted(options.mode)
        << ",\n  \"width\": " << width << ", \"height\": " << height
        << ",\n  \"renderer\": " << quoted(reinterpret_cast<const char*>(glGetString(GL_RENDERER)))
        << ",\n  \"opengl\": " << quoted(reinterpret_cast<const char*>(glGetString(GL_VERSION)))
        << ",\n  \"vsync\": false, \"warmup_seconds\": 2,\n  \"frames\": " << data.frames
        << ", \"seconds\": " << data.seconds << ",\n  \"mean_fps\": " << data.meanFps
        << ",\n  \"p95_frame_ms\": " << data.p95FrameMs
        << ",\n  \"max_frame_ms\": " << data.maxFrameMs
        << ",\n  \"complete\": " << (stats.benchmarkDone() ? "true" : "false") << "\n}\n";
    if (!out) throw runtime_error("Cannot write benchmark result");
}

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
        1280, 720, "Server Room | A: alarm | L: ceiling | F: fog | H: help | Esc: exit",
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

void renderScene(Shaders& shaders, Materials& materials, Lighting& lighting, bool fog,
                 const Mesh& mesh, vector<Rack>& racks, int width, int height) {
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

    for (Rack& rack : racks) {
        rack.draw(mesh, materials.metal, materials.panel,
                  shaders.lit, shaders.led, lighting, time);
    }

    if (glGetError() != GL_NO_ERROR) {
        throw runtime_error("OpenGL rendering error");
    }
}

void updateFPS(GLFWwindow* window, PerformanceStats& stats, double frameTime) {
    if (stats.addFrame(frameTime)) {
        ostringstream title;
        title << "Server Room | FPS " << fixed << setprecision(1) << stats.fps() << " | " << setprecision(2) << stats.frameMs() << " ms" << " | A: alarm | L: ceiling | F: fog | H: help | Esc: exit";
        glfwSetWindowTitle(window, title.str().c_str());
    }
}

void helpKeyCallback(GLFWwindow* window, int key, int, int action, int) {
    if (key == GLFW_KEY_H && action == GLFW_PRESS) {
        auto& visible = *static_cast<bool*>(glfwGetWindowUserPointer(window));
        visible = !visible;
    }
}

void run(GLFWwindow* window, Shaders& shaders, const Mesh& mesh, const RunOptions& options) {
    Materials materials;
    vector<Rack> racks;
    for (unsigned id = 0; id < 4; ++id) {
        racks.emplace_back(vec3(-3.f + id * 2.f, 0.f, -2.f));
        racks.back().id = id;
    }
    Lighting lighting;
    PerformanceStats stats;
    UI ui(SHADER_DIR "/ui.vert", SHADER_DIR "/ui.frag");
    glfwSwapInterval(options.benchmark || options.smokeTest ? 0 : 1);
    if (options.benchmark) stats.beginBenchmark(2, 30);
    if (options.mode == "alarm") lighting.toggleAlarm();
    if (options.mode == "ceiling-off") lighting.toggleCeiling();
    bool help = true;
    bool captured = false;
    glfwSetWindowUserPointer(window, &help);
    glfwSetKeyCallback(window, helpKeyCallback);
    bool fog = true;
    int frames = 0;
    int smokeStage = 0;
    double previous = glfwGetTime();
    double started = previous;
    int lastWidth = 1280, lastHeight = 720;

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

        renderScene(shaders, materials, lighting, fog, mesh, racks, width, height);
        lastWidth = width;
        lastHeight = height;
        UIState hud;
        hud.fps = stats.fps();
        hud.frameMs = stats.frameMs();
        hud.helpVisible = help;
        hud.alarm = lighting.alarm;
        hud.ceiling = lighting.ceilingEnabled;
        hud.fog = fog;
        GLint sceneProgram = 0, sceneVao = 0;
        if (options.smokeTest) {
            glGetIntegerv(GL_CURRENT_PROGRAM, &sceneProgram);
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &sceneVao);
        }
        ui.draw(width, height, hud);
        if (options.smokeTest) {
            GLint restoredProgram = 0, restoredVao = 0;
            GLboolean depthWrite = GL_FALSE;
            glGetIntegerv(GL_CURRENT_PROGRAM, &restoredProgram);
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &restoredVao);
            glGetBooleanv(GL_DEPTH_WRITEMASK, &depthWrite);
            if (restoredProgram != sceneProgram || restoredVao != sceneVao ||
                !glIsEnabled(GL_DEPTH_TEST) || glIsEnabled(GL_BLEND) || !depthWrite)
                throw runtime_error("HUD did not restore scene rendering state");
        }
        if (!options.capture.empty() && !captured && glfwGetTime() - started >= 1.0) {
            captureFrame(options.capture, width, height);
            captured = true;
        }
        if (glGetError() != GL_NO_ERROR) throw runtime_error("OpenGL HUD rendering error");
        glfwSwapBuffers(window);

        double now = glfwGetTime();
        updateFPS(window, stats, now - previous);
        previous = now;
        ++frames;

        if (options.smokeTest && now - started >= 2.6) {
            if (stats.fps() <= 0) throw runtime_error("FPS counter did not update");
            if (smokeStage != 2 || !help || width != 1280 || height != 720)
                throw runtime_error("HUD resize/help smoke check did not finish");
            break;
        }
        if (options.smokeTest && smokeStage == 0 && now - started >= 0.5) {
            helpKeyCallback(window, GLFW_KEY_H, 0, GLFW_PRESS, 0);
            helpKeyCallback(window, GLFW_KEY_H, 0, GLFW_REPEAT, 0);
            helpKeyCallback(window, GLFW_KEY_H, 0, GLFW_RELEASE, 0);
            if (help) throw runtime_error("Help callback did not ignore key repeat/release");
            glfwSetWindowSize(window, 960, 540);
            smokeStage = 1;
        } else if (options.smokeTest && smokeStage == 1 && now - started >= 1.5) {
            if (width != 960 || height != 540) throw runtime_error("Window resize failed");
            helpKeyCallback(window, GLFW_KEY_H, 0, GLFW_PRESS, 0);
            glfwSetWindowSize(window, 1280, 720);
            smokeStage = 2;
        }
        if (options.smokeTest && frames <= 4) {
            fog = !fog;
            lighting.toggleAlarm();
            lighting.toggleCeiling();
        }
        if (options.benchmark && stats.benchmarkDone()) break;
    }
    glfwSetKeyCallback(window, nullptr);
    glfwSetWindowUserPointer(window, nullptr);
    if (options.benchmark) writeBenchmark(options, stats, lastWidth, lastHeight);
}

void cleanup(GLFWwindow* window) {
    if (window) glfwDestroyWindow(window);
    glfwTerminate();
}

int main(int argc, char** argv) {
    GLFWwindow* window = nullptr;
    int result = 0;

    try {
        const auto options = readOptions(argc, argv);
        window = setupOpenGL();
        Shaders shaders = setupShaders();
        Mesh mesh = setupGeometry();
        run(window, shaders, mesh, options);
    } catch (const exception& e) {
        cerr << e.what() << '\n';
        result = 1;
    }

    cleanup(window);
    return result;
}
