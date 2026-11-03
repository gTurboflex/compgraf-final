#include "Texture.h"
#include <fstream>
#include <stdexcept>
#include <vector>

Texture::Texture(const char* path) {
    std::ifstream input(path, std::ios::binary);
    std::string magic;
    int w, h, maximum;
    if (!(input >> magic >> w >> h >> maximum) || magic != "P5" ||
        w < 1 || h < 1 || w > 8192 || h > 8192 || maximum != 255 ||
        input.get() != '\n')
        throw std::runtime_error(std::string("Invalid surface map: ") + path);
    std::vector<unsigned char> pixels(w * h);
    if (!input.read(reinterpret_cast<char*>(pixels.data()), pixels.size()))
        throw std::runtime_error(std::string("Incomplete surface map: ") + path);
    glGenTextures(1, &id);
    bind();
    GLint alignment;
    glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, pixels.data());
    glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}
Texture::~Texture() { glDeleteTextures(1, &id); }
void Texture::bind() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
}
void Material::apply(Shader& shader) const {
    shader.use();
    surface.bind();
    shader.setInt("surfaceMap", 0);
    shader.setVec3("albedo", red, green, blue);
    shader.setFloat("repeatPerUnit", repeatPerUnit);
    shader.setFloat("shininess", shininess);
    shader.setFloat("specularStrength", specular);
}
