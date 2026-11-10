#include "UI.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <iomanip>
#include <sstream>

using namespace std;

namespace {
array<unsigned char, 7> glyph(char c) {
    switch (c) {
        case 'A': return {14,17,17,31,17,17,17};
        case 'B': return {30,17,17,30,17,17,30};
        case 'C': return {14,17,16,16,16,17,14};
        case 'D': return {30,17,17,17,17,17,30};
        case 'E': return {31,16,16,30,16,16,31};
        case 'F': return {31,16,16,30,16,16,16};
        case 'G': return {14,17,16,23,17,17,15};
        case 'H': return {17,17,17,31,17,17,17};
        case 'I': return {31,4,4,4,4,4,31};
        case 'J': return {7,2,2,2,18,18,12};
        case 'K': return {17,18,20,24,20,18,17};
        case 'L': return {16,16,16,16,16,16,31};
        case 'M': return {17,27,21,21,17,17,17};
        case 'N': return {17,25,25,21,19,19,17};
        case 'O': return {14,17,17,17,17,17,14};
        case 'P': return {30,17,17,30,16,16,16};
        case 'Q': return {14,17,17,17,21,18,13};
        case 'R': return {30,17,17,30,20,18,17};
        case 'S': return {15,16,16,14,1,1,30};
        case 'T': return {31,4,4,4,4,4,4};
        case 'U': return {17,17,17,17,17,17,14};
        case 'V': return {17,17,17,17,17,10,4};
        case 'W': return {17,17,17,21,21,21,10};
        case 'X': return {17,17,10,4,10,17,17};
        case 'Y': return {17,17,10,4,4,4,4};
        case 'Z': return {31,1,2,4,8,16,31};
        case '0': return {14,17,19,21,25,17,14};
        case '1': return {4,12,4,4,4,4,14};
        case '2': return {14,17,1,2,4,8,31};
        case '3': return {30,1,1,14,1,1,30};
        case '4': return {2,6,10,18,31,2,2};
        case '5': return {31,16,16,30,1,1,30};
        case '6': return {14,16,16,30,17,17,14};
        case '7': return {31,1,2,4,8,8,8};
        case '8': return {14,17,17,14,17,17,14};
        case '9': return {14,17,17,15,1,1,14};
        case '.': return {0,0,0,0,0,6,6};
        case ':': return {0,6,6,0,6,6,0};
        case '-': return {0,0,0,31,0,0,0};
        case '/': return {1,1,2,4,8,16,16};
        case '|': return {4,4,4,4,4,4,4};
        case '+': return {0,4,4,31,4,4,0};
        case '(': return {2,4,8,8,8,4,2};
        case ')': return {8,4,2,2,2,4,8};
        case '[': return {14,8,8,8,8,8,14};
        case ']': return {14,2,2,2,2,2,14};
        default: return {};
    }
}
const char* onOff(bool value) { return value ? "ON" : "OFF"; }
}

UI::UI(const char* vertexPath, const char* fragmentPath) : shader(vertexPath, fragmentPath) {
    GLint oldVao, oldBuffer;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &oldVao);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldBuffer);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(2 * sizeof(float)));
    glBindVertexArray(oldVao);
    glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
}

UI::~UI() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void UI::rectangle(float x, float y, float width, float height,
                   float r, float g, float b, float a) {
    const float left = 2 * x / cachedWidth - 1;
    const float right = 2 * (x + width) / cachedWidth - 1;
    const float top = 1 - 2 * y / cachedHeight;
    const float bottom = 1 - 2 * (y + height) / cachedHeight;
    const Vertex corners[] = {{left,top,r,g,b,a}, {left,bottom,r,g,b,a},
                              {right,bottom,r,g,b,a}, {right,top,r,g,b,a}};
    for (int index : {0,1,2,0,2,3}) vertices.push_back(corners[index]);
}

void UI::text(const string& value, float x, float y, float scale,
              float r, float g, float b) {
    for (unsigned char c : value) {
        const auto rows = glyph(static_cast<char>(toupper(c)));
        for (int row = 0; row < 7; ++row)
            for (int col = 0; col < 5; ++col)
                if (rows[row] & (1 << (4 - col)))
                    rectangle(x + col * scale, y + row * scale, scale, scale, r, g, b, 1);
        x += 6 * scale;
    }
}

void UI::draw(int width, int height, const UIState& state) {
    if (width <= 0 || height <= 0) return;
    ostringstream fpsLine;
    fpsLine << "FPS " << fixed << setprecision(1) << state.fps
            << " | FRAME " << setprecision(2) << state.frameMs << " MS";
    const string status = string("ALARM ") + onOff(state.alarm) +
        " | CEILING " + onOff(state.ceiling) + " | FOG " + onOff(state.fog);
    const string key = fpsLine.str() + status +
                            (state.helpVisible ? "HELP" : "");
    GLint oldProgram, oldVao, oldBuffer, srcRgb, dstRgb, srcAlpha, dstAlpha, equationRgb, equationAlpha;
    GLboolean depthMask;
    const bool depth = glIsEnabled(GL_DEPTH_TEST), blend = glIsEnabled(GL_BLEND);
    const bool cull = glIsEnabled(GL_CULL_FACE), scissor = glIsEnabled(GL_SCISSOR_TEST);
    glGetIntegerv(GL_CURRENT_PROGRAM, &oldProgram);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &oldVao);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldBuffer);
    glGetIntegerv(GL_BLEND_SRC_RGB, &srcRgb); glGetIntegerv(GL_BLEND_DST_RGB, &dstRgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &srcAlpha); glGetIntegerv(GL_BLEND_DST_ALPHA, &dstAlpha);
    glGetIntegerv(GL_BLEND_EQUATION_RGB, &equationRgb); glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &equationAlpha);
    glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMask);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    if (key != cachedText || width != cachedWidth || height != cachedHeight) {
        cachedText = key; cachedWidth = width; cachedHeight = height;
        vertices.clear();
        const float scale = min(float(height) / 720.f, float(width) / 580.f);
        const float unit = 2 * scale, line = 23 * scale;
        const float panelHeight = (state.helpVisible ? 132 : 98) * scale;
        rectangle(12*scale,12*scale,540*scale,panelHeight,0.015f,0.025f,0.045f,0.91f);
        rectangle(12*scale,12*scale,4*scale,panelHeight,0.1f,0.8f,0.9f,1);
        text(fpsLine.str(),26*scale,25*scale,unit,0.4f,1.f,0.8f);
        text(status,26*scale,25*scale+line,unit,state.alarm?1.f:0.85f,state.alarm?0.45f:0.9f,0.9f);
        if (state.helpVisible) {
            text("A: ALARM | L: CEILING LIGHT",26*scale,84*scale,unit,0.85f,0.9f,0.98f);
            text("F: FOG | H: HIDE HELP | ESC: EXIT",26*scale,107*scale,unit,0.85f,0.9f,0.98f);
        } else {
            text("H: SHOW HELP",26*scale,25*scale+2*line,unit,0.85f,0.9f,0.98f);
        }
        glBufferData(GL_ARRAY_BUFFER,static_cast<GLsizeiptr>(vertices.size()*sizeof(Vertex)),vertices.data(),GL_DYNAMIC_DRAW);
    }
    glDisable(GL_DEPTH_TEST); glDepthMask(GL_FALSE); glDisable(GL_CULL_FACE); glDisable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND); glBlendFuncSeparate(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquationSeparate(GL_FUNC_ADD,GL_FUNC_ADD);
    shader.use();
    glDrawArrays(GL_TRIANGLES,0,static_cast<GLsizei>(vertices.size()));
    if (depth) glEnable(GL_DEPTH_TEST);
    if (!blend) glDisable(GL_BLEND);
    if (cull) glEnable(GL_CULL_FACE);
    if (scissor) glEnable(GL_SCISSOR_TEST);
    glDepthMask(depthMask);
    glBlendFuncSeparate(srcRgb,dstRgb,srcAlpha,dstAlpha);
    glBlendEquationSeparate(equationRgb,equationAlpha);
    glUseProgram(oldProgram);
    glBindVertexArray(oldVao); glBindBuffer(GL_ARRAY_BUFFER,oldBuffer);
}
