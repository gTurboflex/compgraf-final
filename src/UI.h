#pragma once

#include "Shader.h"
#include <string>
#include <vector>

using namespace std;

struct UIState {
    double fps = 0;
    double frameMs = 0;
    bool helpVisible = true;
    bool alarm = false;
    bool ceiling = true;
    bool fog = true;
};

class UI {
public:
    UI(const char* vertexShaderPath, const char* fragmentShaderPath);
    ~UI();
    UI(const UI&) = delete;
    UI& operator=(const UI&) = delete;
    void draw(int framebufferWidth, int framebufferHeight, const UIState& state);

private:
    struct Vertex { float x, y, r, g, b, a; };
    Shader shader;
    GLuint vao = 0, vbo = 0;
    int cachedWidth = 0, cachedHeight = 0;
    string cachedText;
    vector<Vertex> vertices;
    void rectangle(float x, float y, float width, float height,
                   float r, float g, float b, float a);
    void text(const string& value, float x, float y, float scale,
              float r, float g, float b);
};
