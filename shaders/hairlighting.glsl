#include "constants.glsl"
#include "opacitymapping.glsl"

uniform vec3 color;
uniform float specIntensity, diffuseIntensity;
uniform mat4 view, eyeToLight;
uniform vec3 lightPosition;
uniform float opacity;
uniform float maxColorVariation;

vec3 colorContribution(
        in vec4 position_ES, in vec3 tangent_ES, in vec4 lightPosition_ES, in float colorVariation)
{
    vec4 toLight_N = normalize(view * lightPosition_ES - position_ES);
    vec3 tangent_N = normalize(tangent_ES);
    vec3 toEye_N = normalize(-position_ES.xyz);
    vec3 h_N = normalize(toEye_N + toLight_N.xyz);

    float diffuse = sqrt(1. - abs(dot(tangent_N, toLight_N.xyz)));
    float specular = pow(sqrt(1. - abs(dot(tangent_N, h_N))), HAIR_SHININESS);

    vec3 colorMultiplier = vec3(1.0 + maxColorVariation * (2.0 * colorVariation - 1.0));
    return (diffuseIntensity * diffuse + specIntensity * specular) * color * colorMultiplier;
}

vec4 hairLighting(in vec4 position_ES, in vec3 tangent_ES, in float colorVariation)
{
    vec4 position_lightSpace = eyeToLight * position_ES;

    vec4 color;
    color.w = opacity;

    // Key light
    vec4 lightPos = view * vec4(lightPosition, 1.);
    color.xyz = colorContribution(position_ES, tangent_ES, lightPos, colorVariation);
    color.xyz *= getHairTransmittance(position_lightSpace);
    color.xyz *= getMeshVisibility(position_lightSpace);

    // Fill light
    lightPos = view * FILL_LIGHT_POS;
    color.xyz += FILL_LIGHT_INTENSITY_HAIR * colorContribution(position_ES, tangent_ES, lightPos, colorVariation);

    return color;
}
