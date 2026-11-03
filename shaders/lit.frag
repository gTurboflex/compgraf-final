#version 330 core
in vec3 worldPosition, worldNormal;
out vec4 color;
uniform vec3 eye, albedo;
in vec2 uv;
uniform sampler2D surfaceMap;
uniform float time, shininess, specularStrength;
uniform bool alarm, ceilingEnabled;
uniform bool fogEnabled;
vec3 applyColdFog(vec3 surface) {
    float layer = exp(-1.6 * max(worldPosition.y, 0.0));
    float haze = fogEnabled ? 0.48 * layer * (1.0-exp(-0.10*distance(eye,worldPosition))) : 0.0;
    vec3 fogColor = alarm ? vec3(.22,.075,.085) : vec3(.24,.38,.48);
    return mix(surface, fogColor, haze);
}
void main() {
    vec3 N = normalize(worldNormal), V = normalize(eye-worldPosition);
    float pulse = 0.2 + 0.8 * pow(0.5 + 0.5*sin(time*6.283185*2.0), 3.0);
    vec3 tint = alarm ? vec3(1.0,0.035,0.015)*pulse : vec3(0.35,0.65,1.0);
    vec3 base = albedo * texture(surfaceMap,uv).r;
    vec3 result = base * tint * 0.07;
    for(int i=0;i<2;i++) {
        vec3 delta = vec3(i==0 ? -2.0 : 2.0,3.5,-1.0)-worldPosition;
        float d=length(delta);
        vec3 L=delta/max(d,0.001), H=normalize(L+V);
        float diffuse=max(dot(N,L),0.0);
        float specular=diffuse>0.0 ? pow(max(dot(N,H),0.0),shininess) : 0.0;
        float enabled=ceilingEnabled ? 1.0 : 0.0;
        result += enabled*tint*(base*diffuse+vec3(specularStrength)*specular)*3.0/(1.0+0.09*d+0.032*d*d);
    }
    color=vec4(applyColdFog(result),1);
}
