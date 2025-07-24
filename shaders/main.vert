#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 Color;
out vec2 TexCoord;
out vec4 FragPosLightSpace[4];

uniform mat4 uModel;
uniform mat4 uVP;
uniform mat4 lightSpaceMatrices[4];

void main()
{
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    Normal = mat3(transpose(inverse(uModel))) * aNormal;
    Color = aColor;
    TexCoord = aTexCoord;

    for (int i = 0; i < 4; ++i)
        FragPosLightSpace[i] = lightSpaceMatrices[i] * worldPos;

    gl_Position = uVP * worldPos;
}