#include "Scene.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

static void drawBox(const Mesh& cube, Shader& shader, vec3 position, vec3 size) {
    const mat4 model = scale(translate(mat4(1.f), position), size);
    shader.use();
    shader.setMat4("model", value_ptr(model));
    cube.draw();
}

Scene::Scene() {
    for (int row = 0; row < 2; ++row) {
        for (int column = 0; column < 3; ++column) {
            racks.emplace_back(vec3(row == 0 ? -2.4f : 2.4f, 0.f,
                                    -3.f + column * 2.f));
            Rack& rack = racks.back();
            rack.rot = row == 0 ? 90.f : -90.f;
            rack.id = static_cast<unsigned>(racks.size() - 1);
        }
    }
}

void Scene::update(float dt) {
    for (Rack& rack : racks) rack.update(dt);
}

void Scene::draw(const Mesh& cube, const Materials& materials, Shader& lit, Shader& led,
                 const Lighting& lighting, float time) {
    materials.floor.apply(lit);
    drawBox(cube, lit, {0, -.1f, -1}, {10, .2f, 8});
    materials.wall.apply(lit);
    drawBox(cube, lit, {0, 2, -5}, {10, 4, .15f});
    drawBox(cube, lit, {-5, 2, -1}, {.15f, 4, 8});

    for (Rack& rack : racks)
        rack.draw(cube, materials.metal, materials.panel, lit, led, lighting, time);
}
