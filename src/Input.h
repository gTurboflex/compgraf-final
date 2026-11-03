#pragma once

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>
#include <array>
#include "Camera.h"

struct InputActions {
    bool alarm = false;
    bool door = false;
    bool ceiling = false;
    bool fog = false;
    bool toggleHelp = false;
    bool reset = false;
};

class Input {
public:
    Input(GLFWwindow* window, Camera& camera);
    ~Input();
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    InputActions update(float deltaSeconds);
    bool captured() const { return captured_; }
    void setCaptured(bool captured);

private:
    GLFWwindow* window_;
    Camera& camera_;
    std::array<bool, GLFW_KEY_LAST + 1> keys_{};
    InputActions pending_;
    bool captured_ = false;
    bool firstMouse_ = true;
    double lastX_ = 0.0, lastY_ = 0.0;

    static Input& instance(GLFWwindow* window);
    static void keyCallback(GLFWwindow*, int key, int scancode, int action, int mods);
    static void mouseCallback(GLFWwindow*, double x, double y);
    static void buttonCallback(GLFWwindow*, int button, int action, int mods);
    static void focusCallback(GLFWwindow*, int focused);
};
