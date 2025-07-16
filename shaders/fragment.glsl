#version 120

#define MAX_LIGHTS 8

uniform int uNumLights;
uniform vec3 uLightPositions[MAX_LIGHTS];
uniform vec3 uLightColors[MAX_LIGHTS];
uniform float uLightIntensities[MAX_LIGHTS];

uniform sampler2D uShadowMap;

varying vec3 vColor;
varying vec3 vFragPos;
varying vec4 vFragPosLightSpace;

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
            gl_FragColor = vec4(1,0,0,1); // red for shadowed fragments
        else
            gl_FragColor = vec4(0,1,0,1); // green for lit fragments


        // If in shadow, reduce light contribution
        vec3 lightContrib = diff * uLightColors[i] * uLightIntensities[i] * (1.0 - shadow);

        lighting += lightContrib;
    }

    vec3 finalColor = vColor * lighting;

    // Simple ambient term to avoid pure black shadows (optional)
    finalColor += vColor * 0.1;

    gl_FragColor = vec4(finalColor, 1.0);
}
