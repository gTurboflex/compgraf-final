#version 330 core
layout(location=0) in vec3 position;
uniform mat4 model, view, projection;
out vec3 worldPosition;
void main(){
    vec4 p=model*vec4(position,1);
    worldPosition=p.xyz;
    gl_Position=projection*view*p;
}
