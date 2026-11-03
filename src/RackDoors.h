#pragma once
#include <array>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class RackDoors {
public:
    static constexpr int count = 4;
    static float rackX(int rack) { return -3.f + rack * 2.f; }
    int nearby(glm::vec3 eye) const {
        int nearest = -1;
        float distance = 2.3f;
        for (int i = 0; i < count; ++i) {
            float candidate = glm::length(eye - glm::vec3(rackX(i), 1.5f, -1.12f));
            if (candidate < distance) { nearest = i; distance = candidate; }
        }
        return nearest;
    }
    void toggle(int rack) { if (rack >= 0 && rack < count) open_[rack] = !open_[rack]; }
    bool isOpen(int rack) const { return rack >= 0 && rack < count && open_[rack]; }
    void update(float dt) {
        const float step = glm::clamp(dt, 0.f, .1f) * 140.f;
        for (int i = 0; i < count; ++i) {
            const float target = open_[i] ? -105.f : 0.f;
            angles_[i] += glm::clamp(target - angles_[i], -step, step);
        }
    }
    glm::mat4 transform(int rack) const {
        auto hinge = glm::translate(glm::mat4(1), glm::vec3(rackX(rack)-.70f, 1.5f, -1.10f));
        return glm::rotate(hinge, glm::radians(angles_[rack]), glm::vec3(0,1,0));
    }
private:
    std::array<bool, count> open_{};
    std::array<float, count> angles_{};
};
