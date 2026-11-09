#include "Mesh.h"

#include <limits>
#include <stdexcept>

using namespace std;

Mesh::Mesh(const vector<Vertex>& vertices, const vector<GLuint>& indices) {
    if (vertices.empty() || indices.empty() || indices.size() % 3 != 0) {
        throw invalid_argument("Mesh requires vertices and complete indexed triangles.");
    }
    if (indices.size() > static_cast<size_t>(numeric_limits<GLsizei>::max())) {
        throw length_error("Mesh has too many indices for an OpenGL draw call.");
    }
    for (GLuint index : indices) {
        if (index >= vertices.size()) {
            throw out_of_range("Mesh index references a missing vertex.");
        }
    }
    indexCount = static_cast<GLsizei>(indices.size());

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const void*>(offsetof(Vertex, normal)));
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Mesh::draw() const {
    draw(0, static_cast<size_t>(indexCount));
}

void Mesh::draw(size_t firstIndex, size_t count) const {
    const auto total = static_cast<size_t>(indexCount);
    
    if (firstIndex > total || count > total - firstIndex) {
        throw out_of_range("Mesh draw range exceeds the index buffer.");
    }
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(count), GL_UNSIGNED_INT, reinterpret_cast<const void*>(firstIndex * sizeof(GLuint)));
}
