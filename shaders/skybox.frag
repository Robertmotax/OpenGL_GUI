#version 420 core

in vec3 texCoords; 
out vec4 FragColor;

uniform samplerCube skybox;
uniform bool uUseTexture;

void main()
{
    if(uUseTexture) {
        FragColor = texture(skybox, texCoords);
    } else {
        FragColor = vec4(1.0); // fallback white or any default color
    }
}
