#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <cstddef>
#include <vector>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void draw() const;
    void draw(std::size_t firstIndex, std::size_t count) const;

private:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei indexCount = 0;
};
