#include "Rack.h"
#include "ServerEffects.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>

using namespace std;

Rack::Rack(glm::vec3 p) {
    pos = p;
}

void Rack::toggle() {
    opened = !opened;
}

void Rack::update(float dt) {
    float speed = 90.0f;

    if (opened) {
        angle = min(angle + speed * dt, 100.0f);
    } else {
        angle = max(angle - speed * dt, 0.0f);
    }
}

void Rack::part(const Mesh& mesh, Shader& sh, const glm::mat4& base,
                glm::vec3 p, glm::vec3 size, bool quad) {
    glm::mat4 model = glm::translate(base, p);
    model = glm::scale(model, size);

    sh.use();
    sh.setMat4("model", glm::value_ptr(model));

    if (quad) {
        mesh.draw(30, 6);
    } else {
        mesh.draw();
    }
}

void Rack::draw(const Mesh& mesh, const Material& metal, const Material& panel,
                Shader& lit, Shader& led,
                const Lighting& light, float time) {
    glm::mat4 base(1.0f);
    base = glm::translate(base, pos);
    base = glm::rotate(base, glm::radians(rot),
                       glm::vec3(0.0f, 1.0f, 0.0f));

    metal.apply(lit);
    lit.setVec3("albedo", 0.16f, 0.20f, 0.25f);

    part(mesh, lit, base, {-0.65f, 1.5f, 0.0f},
         {0.1f, 3.0f, 1.5f});

    part(mesh, lit, base, {0.65f, 1.5f, 0.0f},
         {0.1f, 3.0f, 1.5f});

    part(mesh, lit, base, {0.0f, 0.05f, 0.0f},
         {1.4f, 0.1f, 1.5f});

    part(mesh, lit, base, {0.0f, 2.95f, 0.0f},
         {1.4f, 0.1f, 1.5f});

    part(mesh, lit, base, {0.0f, 1.5f, -0.7f},
         {1.2f, 2.8f, 0.1f});

    for (int i = 0; i < 8; i++) {
        float y = 0.3f + i * 0.34f;

        glm::mat4 srv = glm::translate(
            base, glm::vec3(0.0f, y, 0.0f)
        );

        panel.apply(lit);
        lit.setVec3("albedo", 0.32f, 0.36f, 0.42f);

        part(mesh, lit, srv, {0.0f, 0.0f, 0.0f},
             {1.15f, 0.26f, 1.1f});

        for (int k = 0; k < 3; k++) {
            unsigned ledId = id * 24 + i * 3 + k;

            ServerEffects::apply(
                led, light, time, ledId
            );

            part(mesh, led, srv,
                 {-0.42f + k * 0.14f, 0.0f, 0.57f},
                 {0.06f, 0.06f, 0.02f}, true);
        }
    }

    glm::mat4 door = glm::translate(
        base, glm::vec3(-0.6f, 0.1f, 0.82f)
    );

    door = glm::rotate(
        door, glm::radians(-angle),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    metal.apply(lit);
    lit.setVec3("albedo", 0.25f, 0.29f, 0.34f);

    part(mesh, lit, door, {0.0f, 1.4f, 0.0f},
         {0.06f, 2.8f, 0.06f});

    part(mesh, lit, door, {1.2f, 1.4f, 0.0f},
         {0.06f, 2.8f, 0.06f});

    part(mesh, lit, door, {0.6f, 0.0f, 0.0f},
         {1.2f, 0.06f, 0.06f});

    part(mesh, lit, door, {0.6f, 2.8f, 0.0f},
         {1.2f, 0.06f, 0.06f});

    lit.setVec3("albedo", 0.55f, 0.58f, 0.62f);

    part(mesh, lit, door, {1.1f, 1.4f, 0.08f},
         {0.04f, 0.35f, 0.08f});
}
