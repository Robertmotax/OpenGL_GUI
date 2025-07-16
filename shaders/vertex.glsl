#version 120

#define MAX_LIGHTS 8

attribute vec3 aPos;
attribute vec3 aColor;

uniform mat4 uModel;
uniform mat4 uVP;
uniform mat4 uLightSpaceMatrix;

varying vec3 vColor;
varying vec3 vFragPos;
varying vec4 vFragPosLightSpace;

void main()
{
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vFragPos = worldPos.xyz;
    vFragPosLightSpace = uLightSpaceMatrix * worldPos;
    gl_Position = uVP * worldPos;
    vColor = aColor;
}
