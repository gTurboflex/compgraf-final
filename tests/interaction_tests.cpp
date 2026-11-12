#include "Interaction.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>

using namespace std;

void require(bool ok, const char* message) {
    if (!ok) { cerr << message << '\n'; exit(1); }
}

int main() {
    vector<Rack> racks;
    for (int i = 0; i < 4; ++i) racks.emplace_back(glm::vec3(-3.f + i * 2.f, 0, -2));
    require(nearbyRack(racks, {0, 1.65f, 3}) == -1, "Distant camera selected a rack");
    require(nearbyRack(racks, {1, 1.65f, 0}) == 2, "Wrong nearest rack");
    racks[2].toggle();
    for (int i = 0; i < 120; ++i) for (auto& rack : racks) rack.update(1.f / 60.f);
    require(racks[2].isOpen() && abs(racks[2].doorAngle() - 100.f) < 0.001f,
            "Selected door did not finish opening");
    require(!racks[1].isOpen() && racks[1].doorAngle() == 0, "Another door changed");
    racks[2].toggle();
    for (int i = 0; i < 120; ++i) racks[2].update(1.f / 60.f);
    require(!racks[2].isOpen() && racks[2].doorAngle() == 0, "Door did not close");
    racks[2].toggle();
    racks[2].update(-1.f);
    racks[2].update(numeric_limits<float>::quiet_NaN());
    require(racks[2].doorAngle() == 0, "Invalid time changed the door");
    racks[2].update(20.f);
    require(abs(racks[2].doorAngle() - 9.f) < 0.001f, "Long stall was not clamped");
    Rack slow({0,0,0}), fast({0,0,0});
    slow.toggle(); fast.toggle();
    for (int i = 0; i < 15; ++i) slow.update(1.f / 30.f);
    for (int i = 0; i < 72; ++i) fast.update(1.f / 144.f);
    require(abs(slow.doorAngle() - fast.doorAngle()) < 0.001f, "Door speed depends on FPS");
    Rack rotated({10,0,4});
    rotated.rot = 90;
    require(glm::length(rotated.doorPosition() - glm::vec3(10.82f,1.5f,4)) < 0.001f,
            "Door interaction ignored the rack transform");
    require(nearbyRack({}, {0,0,0}) == -1, "Empty scene selected a rack");
}
