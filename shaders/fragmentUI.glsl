#version 330

uniform bool useTexture;
uniform sampler2D textureSampler;

in vec3 vColor;
in vec2 vTexCoord;
out vec4 FragColor;

void main() {
    if (useTexture) {
        vec4 textureColor = texture(textureSampler, vTexCoord);

        if(textureColor.a < 0.1)
            discard;

        FragColor = textureColor;
    } else {
        FragColor = vec4(vColor, 1.0);
    }
}
