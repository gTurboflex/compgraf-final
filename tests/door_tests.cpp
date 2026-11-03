#include "RackDoors.h"
#include <stdexcept>
#include <iostream>
void check(bool value) { if (!value) throw std::runtime_error("Door test failed"); }
int main() {
    RackDoors doors;
    check(doors.nearby({0,1.65f,3}) == -1);
    int rack = doors.nearby({-1,1.65f,0});
    check(rack == 1);
    doors.toggle(-1); check(!doors.isOpen(0));
    doors.toggle(rack); check(doors.isOpen(1) && !doors.isOpen(2));
    auto closed = doors.transform(1);
    for (int i=0; i<60; ++i) doors.update(1.f/60);
    auto opened = doors.transform(1);
    check(glm::length(glm::vec3(opened[0]-closed[0])) > .5f);
    check(glm::length(glm::vec3(opened[3]-closed[3])) < .0001f);
    doors.toggle(1);
    for (int i=0; i<60; ++i) doors.update(1.f/60);
    check(glm::length(glm::vec3(doors.transform(1)[0]-closed[0])) < .0001f);
    std::cout << "Door proximity, independent toggle, hinge and close checks passed\n";
}
