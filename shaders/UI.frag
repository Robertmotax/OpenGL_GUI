#version 420 core

in vec3 vColor;
in vec2 vTexCoord;

uniform bool useTexture;
uniform sampler2D textureSampler;

out vec4 FragColor;

void main() {
    if (useTexture) {
        FragColor = texture(textureSampler, vTexCoord);
    } else {
        FragColor = vec4(vColor, 1.0);
    }
}
