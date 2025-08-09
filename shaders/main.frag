#version 420 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoord;

out vec4 FragColor;

uniform bool uUseTexture;
uniform bool uIsUnlit;
layout(binding = 0) uniform sampler2D uTexture;

uniform int uNumLights;
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform float lightIntensities[4];
uniform float farPlanes[4];
layout(binding = 1) uniform samplerCube shadowMaps[4];

uniform vec3 uViewPos;
uniform float shininess;
uniform float specularStrength;


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
    vec3 ambient = 0.05 * baseColor;

    if (uUseTexture) {
        baseColor = texture(uTexture, TexCoord).rgb;
    }

    if (uIsUnlit) {
        FragColor = vec4(baseColor, 1.0);
        return;
    }

    vec3 lighting = vec3(0.0);
    vec3 viewDir = normalize(uViewPos - FragPos);

    for (int i = 0; i < uNumLights; ++i) {
        vec3 lightDir = normalize(lightPositions[i] - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);

        // Shadow factor (1 = in shadow)
        float shadow = ShadowCalculation(shadowMaps[i], FragPos, lightPositions[i], farPlanes[i]);
        
        // Diffuse component
        vec3 light = lightColors[i] * lightIntensities[i];

        // Specular component
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = specularStrength * spec * light;

        // Combine diffuse and specular with shadowing
        lighting += (1.0 - shadow) * (diff * light + specular);
    }

    if (uUseTexture) {
        FragColor = vec4(baseColor * lighting, 1.0);
        return;
    }

    FragColor = vec4(ambient + baseColor * lighting, 1.0);
}
