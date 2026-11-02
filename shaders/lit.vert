#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 normal;
uniform mat4 model, view, projection;
out vec3 worldPosition, worldNormal;
void main() {
    vec4 p = model * vec4(position,1);
    worldPosition = p.xyz;
    worldNormal = transpose(inverse(mat3(model))) * normal;
    gl_Position = projection * view * p;
}
