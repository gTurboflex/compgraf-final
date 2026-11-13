#include "Geometry.h"
#include <cmath>
#include <cstdlib>
#include <iostream>

void require(bool ok, const char* message) {
    if (!ok) { std::cerr << message << '\n'; std::exit(1); }
}

int main() {
    const auto cube = makeCubeGeometry();
    require(cube.vertices.size() == 24 && cube.indices.size() == 36,
            "Cube does not have six separate indexed faces");
    for (const auto& vertex : cube.vertices) {
        require(std::abs(glm::length(vertex.normal) - 1.f) < .001f, "Normal is not unit length");
        require(glm::dot(vertex.position, vertex.normal) > 0, "Normal points into the cube");
        require(vertex.uv.x >= 0 && vertex.uv.x <= 1 && vertex.uv.y >= 0 && vertex.uv.y <= 1,
                "Face UV is outside the unit square");
    }
    for (GLuint index : cube.indices)
        require(index < cube.vertices.size(), "Index references a missing vertex");
    for (size_t i = 0; i < cube.indices.size(); i += 3) {
        const auto& a = cube.vertices[cube.indices[i]];
        const auto& b = cube.vertices[cube.indices[i + 1]];
        const auto& c = cube.vertices[cube.indices[i + 2]];
        const auto faceNormal = glm::cross(b.position - a.position, c.position - a.position);
        require(glm::dot(faceNormal, a.normal) > .99f,
                "Triangle winding disagrees with its outward normal");
        const auto ab = b.uv - a.uv, ac = c.uv - a.uv;
        require(std::abs(ab.x * ac.y - ab.y * ac.x) > .99f, "Texture triangle collapsed");
    }
    for (size_t i = 30; i < 36; ++i) {
        const auto& vertex = cube.vertices[cube.indices[i]];
        require(vertex.position.z == .5f && vertex.normal.z == 1.f,
                "LED draw range is no longer the front face");
    }
}
