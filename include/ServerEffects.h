#pragma once
#include "Lighting.h"
struct ServerEffects {
    static void apply(Shader& shader, const Lighting& lighting, float time, unsigned id) {
        lighting.apply(shader, time);
        shader.setFloat("ledId", static_cast<float>(id));
    }
};
