#include "Input.h"

Input::Input(GLFWwindow* window, Camera& camera) : window_(window), camera_(camera) {
    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, keyCallback);
    glfwSetCursorPosCallback(window_, mouseCallback);
    glfwSetMouseButtonCallback(window_, buttonCallback);
    glfwSetWindowFocusCallback(window_, focusCallback);
    setCaptured(glfwGetWindowAttrib(window_, GLFW_FOCUSED) == GLFW_TRUE);
}

Input::~Input() {
    setCaptured(false);
    glfwSetKeyCallback(window_, nullptr);
    glfwSetCursorPosCallback(window_, nullptr);
    glfwSetMouseButtonCallback(window_, nullptr);
    glfwSetWindowFocusCallback(window_, nullptr);
    glfwSetWindowUserPointer(window_, nullptr);
}

Input& Input::instance(GLFWwindow* window) {
    return *static_cast<Input*>(glfwGetWindowUserPointer(window));
}

void Input::setCaptured(bool captured) {
    if (captured_ == captured) return;
    captured_ = captured;
    firstMouse_ = true;
    glfwSetInputMode(window_, GLFW_CURSOR, captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Input::keyCallback(GLFWwindow* window, int key, int, int action, int) {
    Input& input = instance(window);
    if (key >= 0 && key <= GLFW_KEY_LAST)
        input.keys_[key] = action != GLFW_RELEASE;
    if (action != GLFW_PRESS) return;
    switch (key) {
        case GLFW_KEY_B: input.pending_.alarm = true; break;
        case GLFW_KEY_E: input.pending_.door = true; break;
        case GLFW_KEY_L: input.pending_.ceiling = true; break;
        case GLFW_KEY_F: input.pending_.fog = true; break;
        case GLFW_KEY_H: input.pending_.toggleHelp = true; break;
        case GLFW_KEY_R:
            input.camera_.reset();
            input.firstMouse_ = true;
            input.pending_.reset = true;
            break;
        case GLFW_KEY_TAB: input.setCaptured(!input.captured_); break;
        case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
        default: break;
    }
}

void Input::mouseCallback(GLFWwindow* window, double x, double y) {
    Input& input = instance(window);
    if (!input.captured_) return;
    if (!input.firstMouse_)
        input.camera_.look(float(x - input.lastX_), float(y - input.lastY_));
    input.lastX_ = x;
    input.lastY_ = y;
    input.firstMouse_ = false;
}

void Input::buttonCallback(GLFWwindow* window, int button, int action, int) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        instance(window).setCaptured(true);
}

void Input::focusCallback(GLFWwindow* window, int focused) {
    if (focused == GLFW_TRUE) return;
    Input& input = instance(window);
    input.keys_.fill(false);
    input.pending_ = {};
    input.setCaptured(false);
}

InputActions Input::update(float deltaSeconds) {
    if (captured_ && glfwGetWindowAttrib(window_, GLFW_FOCUSED) == GLFW_TRUE) {
        const float forward = float(keys_[GLFW_KEY_W]) - float(keys_[GLFW_KEY_S]);
        const float right = float(keys_[GLFW_KEY_D]) - float(keys_[GLFW_KEY_A]);
        camera_.move(forward, right, deltaSeconds);
    }
    const InputActions actions = pending_;
    pending_ = {};
    return actions;
}
