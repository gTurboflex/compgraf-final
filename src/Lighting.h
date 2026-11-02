#pragma once
#include "Shader.h"
// Public controls API: call toggles on key press edges.
struct Lighting {
    bool alarm = false;
    bool ceilingEnabled = true;
    void toggleAlarm() { alarm = !alarm; }
    void toggleCeiling() { ceilingEnabled = !ceilingEnabled; }
    void apply(Shader& shader, float time) const {
        shader.use();
        shader.setFloat("time", time);
        shader.setInt("alarm", alarm);
        shader.setInt("ceilingEnabled", ceilingEnabled);
    }
};
