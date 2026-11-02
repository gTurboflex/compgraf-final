#version 330 core
out vec4 color;
uniform float time, ledId;
uniform bool alarm;
float hash(float x){return fract(sin(x*127.1+311.7)*43758.5453);}
void main(){
    float phase=hash(ledId+1.0);
    float rate=mix(0.7,3.7,hash(ledId+17.0));
    float cycle=floor(time*rate+phase);
    float on=step(0.38,hash(ledId*13.0+cycle));
    vec3 tint=alarm?vec3(1,0.025,0.01):vec3(0.08,1,0.38);
    color=vec4(tint*mix(0.06,1.0,on),1);
}
