#pragma once

#include "Rack.h"
#include <vector>

using namespace std;

inline int nearbyRack(const vector<Rack>& racks, const glm::vec3& eye) {
    int nearest = -1;
    float distance = 2.3f;
    for (size_t i = 0; i < racks.size(); ++i) {
        const float candidate = glm::length(eye - racks[i].doorPosition());
        if (candidate < distance) {
            nearest = static_cast<int>(i);
            distance = candidate;
        }
    }
    return nearest;
}
