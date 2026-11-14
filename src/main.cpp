#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Mesh.h"
#include "Geometry.h"
#include "Lighting.h"
#include "Texture.h"
#include "Rack.h"
#include "Scene.h"
#include "PerformanceStats.h"
#include "UI.h"
#include "Audio.h"
#include "Camera.h"
#include "Input.h"
#include "Interaction.h"
#include "../tests/input_smoke.h"
#include "../tests/scene_smoke.h"
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
    bool sceneSmoke = false;
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
        else if (arg == "--scene-smoke") { options.smokeTest = true; options.sceneSmoke = true; }
        else if (arg == "--benchmark" && i + 1 < argc) {
            options.benchmark = true;
            options.mode = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) options.output = argv[++i];
        else if (arg == "--capture" && i + 1 < argc) options.capture = argv[++i];
        else throw runtime_error("Usage: OpenGLApp [--smoke-test|--scene-smoke] [--benchmark normal|alarm|ceiling-off] [--output result.json] [--capture frame.ppm]");
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

GLFWwindow* setupOpenGL(const RunOptions& options) {
    if (!glfwInit()) {
        throw runtime_error("Could not initialize GLFW.");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Benchmark resolution is in framebuffer pixels, including on Retina.
    if (options.benchmark || options.smokeTest)
        glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(
        1280, 720, "Server Room | WASD: walk | B: alarm | E: door | H: help | Esc: exit",
        nullptr, nullptr);
    if (!window) {
        throw runtime_error("Could not create the window.");
    }

    glfwMakeContextCurrent(window);
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
    const auto cube = makeCubeGeometry();
    return Mesh(cube.vertices, cube.indices);
}

void processActions(const InputActions& actions, Lighting& lighting, bool& fog,
                    bool& help, Audio& audio, vector<Rack>& racks, const Camera& camera) {
    if (actions.alarm) lighting.toggleAlarm();
    if (actions.ceiling) lighting.toggleCeiling();
    if (actions.fog) fog = !fog;
    if (actions.toggleHelp) help = !help;
    const int nearby = nearbyRack(racks, camera.position());
    if (actions.door && nearby >= 0) racks[nearby].toggle();
    if (actions.alarm || actions.ceiling || actions.fog || actions.toggleHelp ||
        actions.reset || (actions.door && nearby >= 0)) audio.playClick();
    audio.setAlarm(lighting.alarm);
}

void renderScene(Shaders& shaders, Materials& materials, Lighting& lighting, bool fog,
                 const Mesh& mesh, Scene& scene, const Camera& camera, int width, int height) {
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float time = float(glfwGetTime());
    const auto eye = camera.position();
    const auto view = camera.view();
    const auto projection = camera.projection(width, height);

    for (Shader* shader : {&shaders.lit, &shaders.led}) {
        shader->use();
        shader->setMat4("view", value_ptr(view));
        shader->setMat4("projection", value_ptr(projection));
        shader->setVec3("eye", eye.x, eye.y, eye.z);
        shader->setInt("fogEnabled", fog);
    }
    lighting.apply(shaders.lit, time);

    scene.draw(mesh, materials, shaders.lit, shaders.led, lighting, time);

    if (glGetError() != GL_NO_ERROR) {
        throw runtime_error("OpenGL rendering error");
    }
}

void updateFPS(GLFWwindow* window, PerformanceStats& stats, double frameTime) {
    if (stats.addFrame(frameTime)) {
        ostringstream title;
        title << "Server Room | FPS " << fixed << setprecision(1) << stats.fps() << " | " << setprecision(2) << stats.frameMs() << " ms" << " | WASD: walk | B: alarm | E: door | H: help | Esc: exit";
        glfwSetWindowTitle(window, title.str().c_str());
    }
}

void run(GLFWwindow* window, Shaders& shaders, const Mesh& mesh, const RunOptions& options) {
    Materials materials;
    Scene scene;
    auto& racks = scene.racks;
    Lighting lighting;
    PerformanceStats stats;
    UI ui(SHADER_DIR "/ui.vert", SHADER_DIR "/ui.frag");
    glfwSwapInterval(options.benchmark || options.smokeTest ? 0 : 1);
    if (options.benchmark) stats.beginBenchmark(2, 30);
    if (options.mode == "alarm") lighting.toggleAlarm();
    if (options.mode == "ceiling-off") lighting.toggleCeiling();
    Audio audio;
    audio.setAlarm(lighting.alarm);
    Camera camera;
    Input input(window, camera);
    bool help = true;
    bool captured = false;
    if (options.smokeTest) {
        glfwFocusWindow(window);
        glfwPollEvents();
        for (int attempt = 0; attempt < 20 && !glfwGetWindowAttrib(window, GLFW_FOCUSED); ++attempt)
            glfwWaitEventsTimeout(.05);
        runInputChecks(window, input, camera);
    }
    SceneSmoke sceneSmoke(window);
    const auto key = glfwSetKeyCallback(window, nullptr);
    glfwSetKeyCallback(window, key);
    bool fog = true;
    int frames = 0;
    int smokeStage = 0;
    double previous = glfwGetTime();
    double started = previous;
    double movementPrevious = previous;
    int lastWidth = 1280, lastHeight = 720;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        const double frameStart = glfwGetTime();
        const float dt = static_cast<float>(frameStart - movementPrevious);
        movementPrevious = frameStart;
        if (options.sceneSmoke) sceneSmoke.advance(frameStart - started, camera, racks);
        processActions(input.update(dt), lighting, fog, help, audio, racks, camera);
        scene.update(dt);
        if (options.sceneSmoke) sceneSmoke.verify(racks, lighting, help, fog, stats.fps());
        if (glfwWindowShouldClose(window)) break;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        if (width <= 0 || height <= 0) {
            glfwWaitEventsTimeout(.1);
            previous = glfwGetTime();
            movementPrevious = previous;
            continue;
        }

        renderScene(shaders, materials, lighting, fog, mesh, scene, camera, width, height);
        lastWidth = width;
        lastHeight = height;
        UIState hud;
        hud.fps = stats.fps();
        hud.frameMs = stats.frameMs();
        hud.helpVisible = help;
        hud.mouseCaptured = input.captured();
        const int nearby = nearbyRack(racks, camera.position());
        if (nearby >= 0)
            hud.doorPrompt = "E: " + string(racks[nearby].isOpen() ? "CLOSE" : "OPEN") +
                             " RACK " + to_string(nearby + 1) + " DOOR";
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
        if (!options.capture.empty() && !captured &&
            (options.sceneSmoke ? sceneSmoke.done() : glfwGetTime() - started >= 1.0)) {
            captureFrame(options.capture, width, height);
            captured = true;
        }
        if (glGetError() != GL_NO_ERROR) throw runtime_error("OpenGL HUD rendering error");
        glfwSwapBuffers(window);

        double now = glfwGetTime();
        updateFPS(window, stats, now - previous);
        previous = now;
        ++frames;

        if (options.sceneSmoke && sceneSmoke.done()) break;
        if (options.sceneSmoke && now - started > 12.0) throw runtime_error("Scene smoke timed out");
        if (options.smokeTest && !options.sceneSmoke && now - started >= 2.6) {
            if (stats.fps() <= 0) throw runtime_error("FPS counter did not update");
            if (smokeStage != 2 || !help || width != 1280 || height != 720)
                throw runtime_error("HUD resize/help smoke check did not finish");
            break;
        }
        if (options.smokeTest && !options.sceneSmoke && smokeStage == 0 && now - started >= 0.5) {
            key(window, GLFW_KEY_H, 0, GLFW_PRESS, 0);
            key(window, GLFW_KEY_H, 0, GLFW_REPEAT, 0);
            key(window, GLFW_KEY_H, 0, GLFW_RELEASE, 0);
            processActions(input.update(0.f), lighting, fog, help, audio, racks, camera);
            if (help) throw runtime_error("Help callback did not ignore key repeat/release");
            glfwSetWindowSize(window, 960, 540);
            smokeStage = 1;
        } else if (options.smokeTest && !options.sceneSmoke && smokeStage == 1 && now - started >= 1.5) {
            if (width != 960 || height != 540) throw runtime_error("Window resize failed");
            key(window, GLFW_KEY_H, 0, GLFW_PRESS, 0);
            key(window, GLFW_KEY_H, 0, GLFW_RELEASE, 0);
            processActions(input.update(0.f), lighting, fog, help, audio, racks, camera);
            glfwSetWindowSize(window, 1280, 720);
            smokeStage = 2;
        }
        if (options.smokeTest && !options.sceneSmoke && frames <= 4) {
            fog = !fog;
            lighting.toggleAlarm();
            lighting.toggleCeiling();
        }
        if (options.benchmark && stats.benchmarkDone()) break;
    }
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
        window = setupOpenGL(options);
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
