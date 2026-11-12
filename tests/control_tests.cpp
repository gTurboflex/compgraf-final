#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>

using namespace std;

namespace {
void require(bool condition, const char* message) {
    if (!condition) {
        cerr << "FAIL: " << message << '\n';
        exit(1);
    }
}
bool near(glm::vec3 a, glm::vec3 b, float tolerance = 0.0001f) {
    return glm::length(a - b) < tolerance;
}
}

int main() {
    Camera slow, fast;
    for (int i = 0; i < 60; ++i) slow.move(1.f, 0.f, 1.f / 30.f);
    for (int i = 0; i < 288; ++i) fast.move(1.f, 0.f, 1.f / 144.f);
    require(near(slow.position(), fast.position()), "movement must not depend on frame rate");
    require(near(slow.position(), {0.f, 1.65f, -2.f}), "two seconds walks five units");

    Camera straight, diagonal;
    const glm::vec3 start = straight.position();
    straight.move(1.f, 0.f, 0.1f);
    diagonal.move(1.f, 1.f, 0.1f);
    require(abs(glm::length(straight.position() - start) -
                     glm::length(diagonal.position() - start)) < 0.0001f,
            "diagonal walking must not be faster");

    Camera camera;
    camera.look(900.f, -100000.f);
    require(camera.forward().y < 1.f && camera.forward().y > 0.99f, "pitch clamp prevents flipping");
    camera.move(1.f, 0.f, 0.1f);
    require(abs(camera.position().y - 1.65f) < 0.0001f, "looking up must not change walking height");
    require(camera.position().x > 0.24f, "walking follows horizontal yaw");

    camera.reset();
    camera.move(1.f, 0.f, 20.f);
    require(glm::length(camera.position() - start) < 0.251f, "long stalls cannot teleport camera");
    const glm::vec3 before = camera.position();
    camera.move(1.f, 0.f, -1.f);
    camera.move(1.f, 0.f, numeric_limits<float>::quiet_NaN());
    require(near(before, camera.position()), "invalid frame times cannot move camera");

    const glm::mat4 wide = camera.projection(1280, 720);
    const glm::mat4 square = camera.projection(720, 720);
    require(abs(wide[1][1] / wide[0][0] - 1280.f / 720.f) < 0.0001f,
            "projection uses framebuffer aspect ratio");
    require(abs(wide[1][1] - square[1][1]) < 0.0001f, "resize keeps vertical field of view");
    require(isfinite(camera.projection(0, 0)[0][0]), "zero viewport must remain finite");
    camera.reset();
    require(near(camera.position(), start), "reset restores position");
    require(near(camera.forward(), {0.f, 0.f, -1.f}), "reset restores view direction");
    const glm::vec4 eyeInView = camera.view() * glm::vec4(camera.position(), 1.f);
    require(glm::length(glm::vec3(eyeInView)) < 0.0001f, "view matrix maps eye to origin");
    cout << "PASS: frame-independent walking, diagonal speed, yaw, pitch, reset, stall clamp and aspect ratio\n";
}
