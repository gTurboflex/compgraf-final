#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 texCoord;
uniform mat4 model, view, projection;
uniform float repeatPerUnit;
out vec2 uv;
out vec3 worldPosition, worldNormal;
void main() {
    vec4 p = model * vec4(position,1);
    worldPosition = p.xyz;
    vec3 size = vec3(length(model[0].xyz), length(model[1].xyz), length(model[2].xyz));
    vec2 faceSize = abs(normal.x) > 0.5 ? size.zy : abs(normal.y) > 0.5 ? size.xz : size.xy;
    // Keep the existing texture scale when a box is stretched or rotated.
    uv = (texCoord - vec2(0.5)) * faceSize * repeatPerUnit;
    worldNormal = transpose(inverse(mat3(model))) * normal;
    gl_Position = projection * view * p;
}
