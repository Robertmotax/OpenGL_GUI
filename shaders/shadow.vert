#version 420 core

layout(location = 0) in vec3 aPos;

uniform mat4 uModel;
uniform mat4 uShadowMatrix;  // The view-projection matrix for current face

out vec4 FragPos;

void main() {
    FragPos = uModel * vec4(aPos, 1.0);
    gl_Position = uShadowMatrix * FragPos;
}
