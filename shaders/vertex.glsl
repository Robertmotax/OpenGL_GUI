#version 330

#define MAX_LIGHTS 8

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uVP;
uniform mat4 uLightSpaceMatrix;

out vec3 vColor;
out vec3 vFragPos;
out vec4 vFragPosLightSpace;
out vec2 vTexCoord;

void main()
{
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vFragPos = worldPos.xyz;
    vFragPosLightSpace = uLightSpaceMatrix * worldPos;
    gl_Position = uVP * worldPos;
    vColor = aColor;
    vTexCoord = aTexCoord;
}
