#pragma once

#include "Mesh.h"

struct Geometry {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};

Geometry makeCubeGeometry();
