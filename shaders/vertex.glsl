#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

uniform mat4 uVP;

out vec3 fragColor;

void main() {
    fragColor = aColor;
    gl_Position = uVP * vec4(aPos, 1.0);
}