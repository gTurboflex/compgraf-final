#pragma once
#include "Shader.h"

class Texture {
public:
    explicit Texture(const char* path);
    ~Texture();
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    void bind() const;
private:
    GLuint id = 0;
};

struct Material {
    const Texture& surface;
    float red, green, blue, repeatPerUnit, shininess, specular;
    void apply(Shader& shader) const;
};
