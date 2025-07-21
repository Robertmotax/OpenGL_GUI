#version 330

#define MAX_LIGHTS 8

uniform int uNumLights;
uniform vec3 uLightPositions[MAX_LIGHTS];
uniform vec3 uLightColors[MAX_LIGHTS];
uniform float uLightIntensities[MAX_LIGHTS];

uniform bool useTexture;
uniform sampler2D textureSampler;
uniform sampler2D uShadowMap;

in vec3 vColor;
in vec2 vTexCoord;
in vec3 vFragPos;
in vec4 vFragPosLightSpace;

out vec4 FragColor;


float calculateShadow(vec4 fragPosLightSpace) {
    // Perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range for texture coordinates
    projCoords = projCoords * 0.5 + 0.5;

    // If outside shadow map, no shadow
    if(projCoords.x < 0.0 || projCoords.x > 1.0 ||
       projCoords.y < 0.0 || projCoords.y > 1.0 ||
       projCoords.z < 0.0 || projCoords.z > 1.0)
        return 0.0;

    float closestDepth = texture2D(uShadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    // Bias to avoid shadow acne
    float bias = 0.005;

    // In shadow if current fragment depth is greater than stored closest depth
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    return shadow;
}

void main()
{
    vec3 normal = vec3(0, 0, 1); // Flat normal facing forward, replace with real normal if available

    vec3 lighting = vec3(0.0);

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (i >= uNumLights) break;

        vec3 lightDir = normalize(uLightPositions[i] - vFragPos);
        float diff = max(dot(normal, lightDir), 0.0);

        // Calculate shadow factor for this fragment
        float shadow = calculateShadow(vFragPosLightSpace);

        if(shadow > 0.5)
            FragColor = vec4(1,0,0,1); // red for shadowed fragments
        else
            FragColor = vec4(0,1,0,1); // green for lit fragments


        // If in shadow, reduce light contribution
        vec3 lightContrib = diff * uLightColors[i] * uLightIntensities[i] * (1.0 - shadow);

        lighting += lightContrib;
    }

    //depending if we wish to apply some texture or not
    vec3 baseColor = useTexture
        ? texture(textureSampler, vTexCoord).rgb
        : vColor;

    vec3 finalColor = baseColor * lighting;
    // Simple ambient term to avoid pure black shadows (optional)
    finalColor += baseColor * 0.1;

    FragColor = vec4(finalColor, 1.0);
}
