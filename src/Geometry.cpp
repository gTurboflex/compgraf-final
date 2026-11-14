#include "Geometry.h"

using namespace glm;

Geometry makeCubeGeometry() {
    Geometry cube;
    const GLuint faceIndices[] = {0, 1, 2, 0, 2, 3};
    const float corners[][2] = {{-.5f, -.5f}, {.5f, -.5f}, {.5f, .5f}, {-.5f, .5f}};

    // Four vertices per face keep sharp normals and separate texture edges.
    // Face order stays -X, +X, -Y, +Y, -Z, +Z; LEDs use the last six indices.
    for (int axis = 0; axis < 3; ++axis) {
        for (int sign : {-1, 1}) {
            const GLuint base = static_cast<GLuint>(cube.vertices.size());
            for (int i = 0; i < 4; ++i) {
                vec3 position(0), normal(0);
                position[axis] = sign * .5f;
                normal[axis] = float(sign);
                position[(axis + 1) % 3] = corners[i][0];
                position[(axis + 2) % 3] = corners[i][1];
                vec2 uv = axis == 0 ? vec2(position.z, position.y)
                        : axis == 1 ? vec2(position.x, position.z)
                                    : vec2(position.x, position.y);
                cube.vertices.push_back({position, normal, uv + vec2(.5f)});
            }
            for (int triangle = 0; triangle < 6; triangle += 3) {
                cube.indices.push_back(base + faceIndices[triangle]);
                cube.indices.push_back(base + faceIndices[triangle + (sign > 0 ? 1 : 2)]);
                cube.indices.push_back(base + faceIndices[triangle + (sign > 0 ? 2 : 1)]);
            }
        }
    }
    return cube;
}
