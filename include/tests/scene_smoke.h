#pragma once

#include "Camera.h"
#include "Lighting.h"
#include "Interaction.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <stdexcept>

class SceneSmoke {
public:
    explicit SceneSmoke(GLFWwindow* window) : window_(window) {
        key_ = glfwSetKeyCallback(window_, nullptr);
        glfwSetKeyCallback(window_, key_);
        require(key_ != nullptr, "Scene smoke: key callback is missing");
    }

    void advance(double elapsed, const Camera& camera, const vector<Rack>& racks) {
        require(glfwGetWindowAttrib(window_, GLFW_FOCUSED) == GLFW_TRUE,
                "Scene smoke: keep the application window focused");
        if (phase_ < 0) {
            require(nearbyRack(racks, camera.position()) < 0,
                    "Scene smoke: initial camera should be outside door range");
            press(GLFW_KEY_W);
            tap(GLFW_KEY_E);
            tap(GLFW_KEY_B);
            tap(GLFW_KEY_L);
            tap(GLFW_KEY_H);
            tap(GLFW_KEY_F);
            phase_ = 0;
        } else if (phase_ == 0 && elapsed >= 1.1) {
            release(GLFW_KEY_W);
            rack_ = nearbyRack(racks, camera.position());
            require(rack_ >= 0, "Scene smoke: held W did not reach a rack door");
            require(camera.position().z < 1.f,
                    "Scene smoke: production movement did not change the camera");
            tap(GLFW_KEY_E);
            tap(GLFW_KEY_B);
            tap(GLFW_KEY_L);
            tap(GLFW_KEY_H);
            tap(GLFW_KEY_F);
            phase_ = 1;
            changedAt_ = elapsed;
        } else if (phase_ == 1 && elapsed - changedAt_ >= 1.3) {
            checkAngle(racks, 100.f, "Scene smoke: door did not finish opening");
            tap(GLFW_KEY_E);
            phase_ = 2;
            changedAt_ = elapsed;
        } else if (phase_ == 2 && elapsed - changedAt_ >= 1.3) {
            checkAngle(racks, 0.f, "Scene smoke: door did not finish closing");
            tap(GLFW_KEY_E);
            tap(GLFW_KEY_B);
            phase_ = 3;
            changedAt_ = elapsed;
        } else if (phase_ == 3 && elapsed - changedAt_ >= 1.3) {
            checkAngle(racks, 100.f, "Scene smoke: final door did not finish opening");
            tap(GLFW_KEY_R);
            tap(GLFW_KEY_B);
            phase_ = 4;
        }
    }

    void verify(const vector<Rack>& racks, const Lighting& lighting,
                bool help, bool fog, double fps) {
        if (phase_ == 0) {
            require(lighting.alarm && !lighting.ceilingEnabled && !help && !fog,
                    "Scene smoke: B/L/H/F actions did not reach scene state");
            for (int i = 0; i < static_cast<int>(racks.size()); ++i)
                require(!racks[i].isOpen(), "Scene smoke: distant E opened a door");
            return;
        }
        require(lighting.ceilingEnabled && help && fog,
                "Scene smoke: ceiling, help or fog did not switch back");
        require(lighting.alarm == (phase_ == 3),
                "Scene smoke: alarm state is wrong");
        require(racks[rack_].isOpen() == (phase_ != 2),
                "Scene smoke: nearby E did not toggle the selected door");
        for (int i = 0; i < static_cast<int>(racks.size()); ++i)
            if (i != rack_)
                require(!racks[i].isOpen(), "Scene smoke: E changed another door");
        if (phase_ == 4) {
            require(isfinite(fps) && fps > 0.0,
                    "Scene smoke: FPS counter has no measured result");
            complete_ = true;
        }
    }

    bool done() const { return complete_; }

private:
    GLFWwindow* window_;
    GLFWkeyfun key_ = nullptr;
    int phase_ = -1, rack_ = -1;
    double changedAt_ = 0;
    bool complete_ = false;

    static void require(bool condition, const char* message) {
        if (!condition) throw runtime_error(message);
    }
    void press(int key) { key_(window_, key, 0, GLFW_PRESS, 0); }
    void release(int key) { key_(window_, key, 0, GLFW_RELEASE, 0); }
    void tap(int key) { press(key); release(key); }
    void checkAngle(const vector<Rack>& racks, float degrees, const char* message) const {
        require(abs(racks[rack_].doorAngle() - degrees) < 0.002f,
                message);
    }
};
