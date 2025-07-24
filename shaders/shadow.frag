#version 420 core

in vec4 FragPos;
uniform vec3 uLightPos;
uniform float uFarPlane;

void main() {
    float lightDist = length(FragPos.xyz - uLightPos);
    lightDist = lightDist / uFarPlane;
    gl_FragDepth = lightDist;
}
