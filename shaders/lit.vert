#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
uniform mat4 model, view, projection;
uniform float repeatPerUnit;
out vec2 uv;
out vec3 worldPosition, worldNormal;
void main() {
    vec4 p = model * vec4(position,1);
    worldPosition = p.xyz;
    // Face-local distances retain the physical aspect ratio after nonuniform scaling.
    vec3 metric = position * vec3(length(model[0].xyz), length(model[1].xyz), length(model[2].xyz));
    uv = (abs(normal.x) > 0.5 ? metric.zy : abs(normal.y) > 0.5 ? metric.xz : metric.xy) * repeatPerUnit;
    worldNormal = transpose(inverse(mat3(model))) * normal;
    gl_Position = projection * view * p;
}
