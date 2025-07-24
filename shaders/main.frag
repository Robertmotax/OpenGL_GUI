#version 420 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoord;

out vec4 FragColor;

uniform bool uUseTexture;
layout(binding = 0) uniform sampler2D uTexture;

uniform int uNumLights;
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform float lightIntensities[4];
uniform float farPlanes[4];
layout(binding = 1) uniform samplerCube shadowMaps[4];

float ShadowCalculation(samplerCube shadowMap, vec3 fragPos, vec3 lightPos, float farPlane) {
    vec3 fragToLight = fragPos - lightPos;
    float currentDepth = length(fragToLight);
    float closestDepth = texture(shadowMap, fragToLight).r * farPlane;

    float bias = 0.05;
    return (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
}

void main() {
    vec3 norm = normalize(Normal);
    vec3 baseColor = Color;

    if (uUseTexture) {
        baseColor *= texture(uTexture, TexCoord).rgb;
    }

    vec3 lighting = vec3(0.0);
    for (int i = 0; i < uNumLights; ++i) {
        vec3 lightDir = normalize(lightPositions[i] - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        float shadow = ShadowCalculation(shadowMaps[i], FragPos, lightPositions[i], farPlanes[i]);
        vec3 light = lightColors[i] * lightIntensities[i];

        lighting += (1.0 - shadow) * diff * light;
    }

    FragColor = vec4(baseColor * lighting, 1.0);
}
