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

void Scene::drawEquipment(const Mesh& cube, const Materials& materials, Shader& lit) {
    // These fixtures sit above the two light positions in lit.frag.
    for (float x : {-2.f, 2.f}) {
        materials.metal.apply(lit);
        drawBox(cube, lit, {x, 3.9f, -1}, {1.4f, .12f, .65f});
        drawBox(cube, lit, {x, 3.73f, -1}, {.12f, .24f, .12f});
        drawBox(cube, lit, {x, 3.64f, -1}, {1.3f, .12f, .55f});
        materials.diffuser.apply(lit);
        drawBox(cube, lit, {x, 3.565f, -1}, {1.12f, .03f, .40f});
    }

    // Floor-standing cooling unit, with a recessed dark vent and louvers.
    materials.equipment.apply(lit);
    drawBox(cube, lit, {-4.1f, 1.15f, -4.15f}, {1.2f, 2.3f, 1.f});
    materials.metal.apply(lit);
    drawBox(cube, lit, {-4.1f, 1.05f, -3.64f}, {.98f, 1.65f, .04f});
    materials.equipment.apply(lit);
    for (int i = 0; i < 9; ++i)
        drawBox(cube, lit, {-4.1f, .32f + .18f * i, -3.59f}, {.92f, .06f, .08f});
    materials.screen.apply(lit);
    drawBox(cube, lit, {-4.1f, 2.08f, -3.63f}, {.42f, .14f, .05f});

    // Static monitor and its stand.
    materials.metal.apply(lit);
    drawBox(cube, lit, {1.05f, .06f, -4.35f}, {.7f, .12f, .55f});
    drawBox(cube, lit, {1.05f, .77f, -4.35f}, {.12f, 1.42f, .12f});
    drawBox(cube, lit, {1.05f, 1.7f, -4.35f}, {1.1f, .75f, .16f});
    materials.screen.apply(lit);
    drawBox(cube, lit, {1.05f, 1.7f, -4.255f}, {.94f, .59f, .03f});
    materials.diffuser.apply(lit);
    for (int i = 0; i < 3; ++i)
        drawBox(cube, lit, {.92f, 1.88f - .16f * i, -4.232f}, {.53f, .035f, .01f});

    // Separate wall-mounted control panel, with display and three buttons.
    materials.equipment.apply(lit);
    drawBox(cube, lit, {-.75f, 1.55f, -4.79f}, {.65f, .9f, .25f});
    materials.screen.apply(lit);
    drawBox(cube, lit, {-.75f, 1.73f, -4.65f}, {.47f, .32f, .04f});
    materials.metal.apply(lit);
    for (int i = 0; i < 3; ++i)
        drawBox(cube, lit, {-.93f + .18f * i, 1.32f, -4.63f}, {.10f, .10f, .08f});
}

void Scene::drawCableTrays(const Mesh& cube, const Materials& materials, Shader& lit) {
    for (float x : {-2.4f, 2.4f}) {
        materials.metal.apply(lit);
        drawBox(cube, lit, {x, 3.23f, -1}, {.5f, .04f, 6});
        for (float side : {-.25f, .25f}) {
            drawBox(cube, lit, {x + side, 3.31f, -1}, {.04f, .16f, 6});
            for (float z : {-3.5f, -1.f, 1.5f})
                drawBox(cube, lit, {x + side, 3.68f, z}, {.035f, .64f, .035f});
        }
        for (int i = 0; i < 12; ++i)
            drawBox(cube, lit, {x, 3.27f, -3.75f + .5f * i}, {.5f, .04f, .06f});

        // Three simple cable runs rest on the crosspieces.
        for (int cable = 0; cable < 3; ++cable) {
            materials.metal.apply(lit);
            lit.setVec3("albedo", .18f + .14f * cable, .32f, .43f - .10f * cable);
            drawBox(cube, lit, {x - .13f + .13f * cable, 3.31f, -1.06f - .065f * cable},
                    {.045f, .045f, 6.12f + .13f * cable});
        }
    }

    // Connect the two rows along the back wall.
    materials.metal.apply(lit);
    drawBox(cube, lit, {0, 3.23f, -4.25f}, {5.3f, .04f, .5f});
    drawBox(cube, lit, {0, 3.31f, -4.5f}, {5.3f, .16f, .04f});
    drawBox(cube, lit, {0, 3.31f, -4.f}, {4.25f, .16f, .04f});
    for (float x : {-1.5f, 1.5f})
        drawBox(cube, lit, {x, 3.64f, -4.25f}, {.035f, .72f, .035f});
    for (int cable = 0; cable < 3; ++cable) {
        lit.setVec3("albedo", .18f + .14f * cable, .32f, .43f - .10f * cable);
        drawBox(cube, lit, {-.13f + .13f * cable, 3.31f, -4.12f - .13f * cable},
                {4.8f, .045f, .045f});
    }
}

void Scene::draw(const Mesh& cube, const Materials& materials, Shader& lit, Shader& led,
                 const Lighting& lighting, float time) {
    materials.floor.apply(lit);
    drawBox(cube, lit, {0, -.1f, -1}, {10, .2f, 8});
    materials.wall.apply(lit);
    drawBox(cube, lit, {0, 2, -5}, {10, 4, .15f});
    drawBox(cube, lit, {-5, 2, -1}, {.15f, 4, 8});
    drawBox(cube, lit, {5, 2, -1}, {.15f, 4, 8});
    drawBox(cube, lit, {0, 4.05f, -1}, {10, .1f, 8});

    drawEquipment(cube, materials, lit);
    drawCableTrays(cube, materials, lit);

    for (Rack& rack : racks)
        rack.draw(cube, materials.metal, materials.panel, lit, led, lighting, time);
}
