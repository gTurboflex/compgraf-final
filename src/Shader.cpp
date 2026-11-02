#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;

string Shader::readFile(const char* path) {
    ifstream file(path);
    if (!file) {
        throw runtime_error(string("Cannot open shader file: ") + path);
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int Shader::compileShader(unsigned int type, const string& source, const char* path) {
    unsigned int shader = glCreateShader(type);
    if (shader == 0) {
        cerr << "Cannot create shader: " << path << endl;
        return 0;
    }
    const char* code = source.c_str();
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[1024] = {};
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        cerr << "Shader compilation failed: " << path << '\n' << log << endl;
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    string vertexCode = readFile(vertexPath);
    string fragmentCode = readFile(fragmentPath);

    unsigned int vertex = compileShader(GL_VERTEX_SHADER, vertexCode, vertexPath);
    unsigned int fragment = compileShader(GL_FRAGMENT_SHADER, fragmentCode, fragmentPath);

    if (vertex == 0 || fragment == 0) {
        if (vertex != 0) glDeleteShader(vertex);
        if (fragment != 0) glDeleteShader(fragment);
        throw runtime_error("Could not compile shaders. See the console errors.");
    }

    program = glCreateProgram();
    if (program == 0) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        throw runtime_error("Could not create shader program.");
    }

    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    
    if (!success) {
        char log[1024] = {};
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        cerr << "Shader linking failed: " << vertexPath << ", " << fragmentPath << '\n' << log << endl;
        glDeleteProgram(program);
        throw runtime_error("Could not link shaders. See the console errors.");
    }
}

Shader::~Shader() {
    glDeleteProgram(program);
}

void Shader::use() {
    glUseProgram(program);
}

void Shader::setInt(const char* name, int value) {
    glUniform1i(glGetUniformLocation(program, name), value);
}

void Shader::setFloat(const char* name, float value) {
    glUniform1f(glGetUniformLocation(program, name), value);
}

void Shader::setVec3(const char* name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(program, name), x, y, z);
}

void Shader::setMat4(const char* name, const float* values) {
    glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, values);
}
