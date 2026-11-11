#pragma once

#include <glad/gl.h>
#include <string>

using namespace std;

using namespace std;

class Shader {
public:
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    void use();

    void setInt(const char* name, int value);
    void setFloat(const char* name, float value);
    void setVec3(const char* name, float x, float y, float z);
    void setMat4(const char* name, const float* values);

private:
    unsigned int program;

    string readFile(const char* path);
    unsigned int compileShader(unsigned int type, const string& source,
    string readFile(const char* path);
    unsigned int compileShader(unsigned int type, const string& source,
                               const char* path);
};
