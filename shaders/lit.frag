#version 330 core
in vec3 worldPosition, worldNormal;
out vec4 color;
uniform vec3 eye, albedo;
uniform float time;
uniform bool alarm, ceilingEnabled;
void main() {
    vec3 N = normalize(worldNormal), V = normalize(eye-worldPosition);
    float pulse = 0.2 + 0.8 * pow(0.5 + 0.5*sin(time*6.283185*2.0), 3.0);
    vec3 tint = alarm ? vec3(1.0,0.035,0.015)*pulse : vec3(0.35,0.65,1.0);
    vec3 result = albedo * tint * 0.07;
    for(int i=0;i<2;i++) {
        vec3 delta = vec3(i==0 ? -2.0 : 2.0,3.5,-1.0)-worldPosition;
        float d=length(delta);
        vec3 L=delta/max(d,0.001), H=normalize(L+V);
        float diffuse=max(dot(N,L),0.0);
        float specular=diffuse>0.0 ? pow(max(dot(N,H),0.0),48.0) : 0.0;
        float enabled=ceilingEnabled ? 1.0 : 0.0;
        result += enabled*tint*(albedo*diffuse+vec3(0.4)*specular)*3.0/(1.0+0.09*d+0.032*d*d);
    }
    color=vec4(result,1);
}
