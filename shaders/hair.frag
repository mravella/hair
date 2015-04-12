#version 400 core

in vec4 position_g;
in vec3 tangent_g;

out vec3 fragColor;

uniform vec3 color;
uniform mat4 view;
uniform vec3 lightPosition;
uniform sampler2D shadowMap;
uniform sampler2D opacityMap;
uniform mat4 eyeToLight;
uniform float shadowIntensity;

const float occlusionLayerSize = 0.004;

float currDepth;
float closestDepth;

// Returns a value from 0 to 1 representing the amount of occlusion coming from
// the given opacity map layer. (0 means fragment is fully in front of layer,
// 1 means fragment is fully behind layer.)
float shadowLayerOcclusion(int layer)
{
    float layerStart = closestDepth + layer*occlusionLayerSize;
    return clamp((currDepth - layerStart) / occlusionLayerSize, 0.0, 1.0);
}

// Calculates the transmittance of the light source to point p based on the deep
// opacity map, where p is in the light source's clip space.
float getTransmittance(vec4 p)
{
    vec4 shadowCoord = (p / p.w + 1.0) / 2.0;

    currDepth = shadowCoord.z;
    closestDepth = texture(shadowMap, shadowCoord.xy).r;

    vec4 opacityMapValues = texture(opacityMap, shadowCoord.xy);

    float occlusion = 0.0;
    for (int i = 0; i < 4; i++)
        occlusion += mix(0, opacityMapValues[i], shadowLayerOcclusion(i));

    return exp(- shadowIntensity * occlusion);
}

void main()
{
    vec4 toLight_N = normalize((view * vec4(lightPosition, 1.)) - position_g);
    vec3 tangent_N = normalize(tangent_g);
    
    float diffuse = sqrt(1. - abs(dot(tangent_N, toLight_N.xyz)));

    vec3 toEye_N = normalize(-position_g.xyz);
    vec3 h_N = normalize(toEye_N + toLight_N.xyz);
    
    float specular = pow(sqrt(1. - abs(dot(tangent_N, h_N))), 40.);
    
    fragColor = color * vec3(diffuse + specular);

    fragColor *= getTransmittance(eyeToLight * position_g);
}
