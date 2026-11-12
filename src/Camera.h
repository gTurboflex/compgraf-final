#pragma once

#include <glm/glm.hpp>

class Camera {
public:
    static constexpr float walkingSpeed = 2.5f;
    static constexpr float mouseSensitivity = 0.10f;

    const glm::vec3& position() const { return position_; }
    glm::vec3 forward() const;
    glm::mat4 view() const;
    glm::mat4 projection(int framebufferWidth, int framebufferHeight) const;
    void move(float forwardAxis, float rightAxis, float deltaSeconds);
    void look(float deltaX, float deltaY);
    void reset();

private:
    glm::vec3 position_{0.f, 1.65f, 3.f};
    float yaw_ = -90.f;
    float pitch_ = 0.f;
};
