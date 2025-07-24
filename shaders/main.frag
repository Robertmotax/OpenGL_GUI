#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Color;            // Vertex color
in vec2 TexCoord;         // Texture coordinates
in vec4 FragPosLightSpace[4];  // For shadow mapping

out vec4 FragColor;

uniform sampler2D uTexture;
uniform bool uUseTexture;

uniform int uNumLights;
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform float lightIntensities[4];
uniform sampler2D shadowMaps[4];

float ShadowCalculation(sampler2D shadowMap, vec4 fragPosLightSpace, vec3 lightDir) {
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 || projCoords.y < 0.0 || projCoords.y > 1.0)
    return 0.0; // not in shadow

    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // Depth in shadow map
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    // Current fragment depth from light's POV
    float currentDepth = projCoords.z;

    // Bias to prevent acne
    float bias = max(0.0015 * (1.0 - dot(Normal, lightDir)), 0.0005);

    // In shadow?
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

    return shadow;
}


void main() {
    vec3 normal = normalize(Normal);
    vec3 lighting = vec3(0.0);

    for (int i = 0; i < uNumLights; ++i) {
        vec3 lightDir = normalize(lightPositions[i] - FragPos);
        float diff = max(abs(dot(normal, lightDir)), 0.0);

        float shadow = ShadowCalculation(shadowMaps[i], FragPosLightSpace[i], lightDir);
        vec3 lightColor = lightColors[i] * lightIntensities[i];

        lighting += (1.0 - shadow) * diff * lightColor;
    }

    vec3 baseColor = Color;
    if (uUseTexture) {
        baseColor *= texture(uTexture, TexCoord).rgb;
    }

    FragColor = vec4(baseColor * lighting, 1.0);
}
