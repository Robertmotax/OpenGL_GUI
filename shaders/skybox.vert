#version 420 core

layout(location = 0) in vec3 aPos; 

out vec3 texCoords;  // pass the direction vector for sampling

uniform mat4 uModel;
uniform mat4 uVP;

void main() {
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    texCoords = worldPos.xyz;  // pass world position as direction vector to frag shader
    
    gl_Position = uVP * worldPos;
    gl_Position.z = gl_Position.w;  // push depth to far plane so skybox renders behind everything
}
