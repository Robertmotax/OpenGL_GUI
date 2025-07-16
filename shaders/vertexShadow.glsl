#version 120

uniform mat4 uModel;
uniform mat4 lightSpaceMatrix;

attribute vec3 aPos;

void main() {
    gl_Position = lightSpaceMatrix * uModel * vec4(aPos, 1.0);
}
