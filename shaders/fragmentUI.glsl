#version 330

uniform bool useTexture;
uniform sampler2D textureSampler;

varying vec3 vColor;
varying vec2 vTexCoord;

void main() {
    if (useTexture) {
        vec4 textureColor = texture(textureSampler, vTexCoord);
        gl_FragColor = textureColor;
    } else {
        gl_FragColor = vec4(vColor, 1.0);
    }
}
