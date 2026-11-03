#version 330 core
in vec3 worldPosition;
uniform vec3 eye;
out vec4 color;
uniform float time, ledId;
uniform bool alarm;
float hash(float x){return fract(sin(x*127.1+311.7)*43758.5453);}
uniform bool fogEnabled;
vec3 applyColdFog(vec3 surface) {
    float layer = exp(-1.6 * max(worldPosition.y, 0.0));
    float haze = fogEnabled ? 0.48 * layer * (1.0-exp(-0.10*distance(eye,worldPosition))) : 0.0;
    vec3 fogColor = alarm ? vec3(.22,.075,.085) : vec3(.24,.38,.48);
    return mix(surface, fogColor, haze);
}
void main(){
    float phase=hash(ledId+1.0);
    float rate=mix(0.7,3.7,hash(ledId+17.0));
    float cycle=floor(time*rate+phase);
    float on=step(0.38,hash(ledId*13.0+cycle));
    vec3 tint=alarm?vec3(1,0.025,0.01):vec3(0.08,1,0.38);
    color=vec4(applyColdFog(tint*mix(0.06,1.0,on)),1);
}
