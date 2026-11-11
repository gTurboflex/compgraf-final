#pragma once

#include <glm/glm.hpp>
#include "Shader.h"
#include "Lighting.h"
#include "Mesh.h"
#include "Texture.h"

class Rack {
public:
    glm::vec3 pos;
    float rot = 0.0f;
    unsigned id = 0;

    Rack(glm::vec3 p);

    void toggle();
    void update(float dt);

    void draw(const Mesh& mesh, const Material& metal, const Material& panel,
              Shader& lit, Shader& led,
              const Lighting& light, float time);

private:
    bool opened = false;
    float angle = 0.0f;

    void part(const Mesh& mesh, Shader& sh, const glm::mat4& base,
              glm::vec3 p, glm::vec3 size,
              bool quad = false);
};
