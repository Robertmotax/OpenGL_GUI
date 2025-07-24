#version 330

attribute vec3 aPos;
attribute vec3 aColor;
attribute vec2 aTexCoord;

varying vec3 vColor;
varying vec2 vTexCoord;

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
