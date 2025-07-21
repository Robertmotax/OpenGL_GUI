#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 vColor;
out vec2 vTexCoord;

uniform mat4 uModel;
uniform mat4 uVP;
uniform int isNDC;

void main() {
    vec4 pos = uVP * uModel * vec4(aPos, 1.0);
    if (isNDC == 1) {
        pos.z = -0.9;  // Near the front of the screen in NDC
    }

    gl_Position = pos;
    vColor = aColor;
    vTexCoord = aTexCoord;
}
