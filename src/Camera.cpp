#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>

glm::vec3 Camera::forward() const {
    const float yaw = glm::radians(yaw_);
    const float pitch = glm::radians(pitch_);
    return glm::normalize(glm::vec3(std::cos(yaw) * std::cos(pitch),
                                    std::sin(pitch),
                                    std::sin(yaw) * std::cos(pitch)));
}

glm::mat4 Camera::view() const {
    return glm::lookAt(position_, position_ + forward(), glm::vec3(0.f, 1.f, 0.f));
}

glm::mat4 Camera::projection(int width, int height) const {
    const float aspect = float(std::max(width, 1)) / float(std::max(height, 1));
    return glm::perspective(glm::radians(60.f), aspect, 0.1f, 100.f);
}

void Camera::move(float forwardAxis, float rightAxis, float deltaSeconds) {
    if (!std::isfinite(deltaSeconds) || deltaSeconds <= 0.f) return;
    const float dt = std::min(deltaSeconds, 0.1f);
    const float yaw = glm::radians(yaw_);
    const glm::vec3 ahead(std::cos(yaw), 0.f, std::sin(yaw));
    const glm::vec3 right = glm::cross(ahead, glm::vec3(0.f, 1.f, 0.f));
    glm::vec3 direction = ahead * std::clamp(forwardAxis, -1.f, 1.f)
                        + right * std::clamp(rightAxis, -1.f, 1.f);
    const float distance = glm::length(direction);
    if (distance > 1.f) direction /= distance;
    position_ += direction * walkingSpeed * dt;
}

void Camera::look(float deltaX, float deltaY) {
    if (!std::isfinite(deltaX) || !std::isfinite(deltaY)) return;
    yaw_ = std::remainder(yaw_ + deltaX * mouseSensitivity, 360.f);
    pitch_ = std::clamp(pitch_ - deltaY * mouseSensitivity, -89.f, 89.f);
}

void Camera::reset() {
    position_ = {0.f, 1.65f, 3.f};
    yaw_ = -90.f;
    pitch_ = 0.f;
}
