#pragma once

#include "Input.h"
#include <cmath>
#include <stdexcept>

using namespace std;

inline void runInputChecks(GLFWwindow* window, Input& input, Camera& camera) {
    auto require = [](bool condition, const char* message) {
        if (!condition) throw runtime_error(message);
    };
    const auto key = glfwSetKeyCallback(window, nullptr);
    const auto mouse = glfwSetCursorPosCallback(window, nullptr);
    const auto focus = glfwSetWindowFocusCallback(window, nullptr);
    const auto button = glfwSetMouseButtonCallback(window, nullptr);
    glfwSetKeyCallback(window, key);
    glfwSetCursorPosCallback(window, mouse);
    glfwSetWindowFocusCallback(window, focus);
    glfwSetMouseButtonCallback(window, button);
    require(key && mouse && focus && button, "Input smoke: callbacks are not installed");
    require(glfwGetWindowAttrib(window, GLFW_FOCUSED) == GLFW_TRUE,
            "Input smoke requires a focused GLFW window");

    struct ActionCase { int keyCode; bool InputActions::*flag; };
    const ActionCase cases[] = {
        {GLFW_KEY_B, &InputActions::alarm}, {GLFW_KEY_E, &InputActions::door},
        {GLFW_KEY_L, &InputActions::ceiling}, {GLFW_KEY_F, &InputActions::fog},
        {GLFW_KEY_H, &InputActions::toggleHelp}
    };
    for (const auto& test : cases) {
        key(window, test.keyCode, 0, GLFW_PRESS, 0);
        require(input.update(0.f).*test.flag, "Input smoke: press did not produce action");
        key(window, test.keyCode, 0, GLFW_REPEAT, 0);
        require(!(input.update(0.f).*test.flag), "Input smoke: repeat retriggered action");
        key(window, test.keyCode, 0, GLFW_RELEASE, 0);
        require(!(input.update(0.f).*test.flag), "Input smoke: release retriggered action");
    }

    camera.reset();
    input.setCaptured(true);
    const auto start = camera.position();
    key(window, GLFW_KEY_W, 0, GLFW_PRESS, 0);
    input.update(0.1f);
    require(glm::length(camera.position() - start) > 0.24f,
            "Input smoke: held W did not walk");
    focus(window, GLFW_FALSE);
    const auto stopped = camera.position();
    require(!input.captured(), "Input smoke: focus loss did not release cursor");
    input.setCaptured(true);
    input.update(0.1f);
    require(glm::length(camera.position() - stopped) < 0.0001f,
            "Input smoke: focus loss left a movement key held");

    camera.reset();
    input.setCaptured(false);
    input.setCaptured(true);
    const auto initialDirection = camera.forward();
    mouse(window, 100.0, 100.0);
    require(glm::length(camera.forward() - initialDirection) < 0.0001f,
            "Input smoke: first mouse sample caused a jump");
    button(window, GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
    mouse(window, 150.0, 100.0);
    require(glm::length(camera.forward() - initialDirection) > 0.01f,
            "Input smoke: mouse did not rotate after a click while captured");
    key(window, GLFW_KEY_R, 0, GLFW_PRESS, 0);
    require(input.update(0.f).reset, "Input smoke: reset flag missing");
    require(glm::length(camera.position() - start) < 0.0001f &&
            glm::length(camera.forward() - initialDirection) < 0.0001f,
            "Input smoke: camera reset failed");
    key(window, GLFW_KEY_R, 0, GLFW_RELEASE, 0);
    focus(window, GLFW_FALSE);
    input.setCaptured(true);
}
