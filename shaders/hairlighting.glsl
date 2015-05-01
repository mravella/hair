#include "constants.glsl"
#include "opacitymapping.glsl"

uniform vec3 color;
uniform float specIntensity, diffuseIntensity;
uniform mat4 view, eyeToLight;
uniform vec3 lightPosition;

vec3 colorContribution(in vec4 position_ES, in vec3 tangent_ES, in vec4 lightPosition_ES)
{
    vec4 toLight_N = normalize(view * lightPosition_ES - position_ES);
    vec3 tangent_N = normalize(tangent_ES);
    vec3 toEye_N = normalize(-position_ES.xyz);
    vec3 h_N = normalize(toEye_N + toLight_N.xyz);
    float diffuse = sqrt(1. - abs(dot(tangent_N, toLight_N.xyz)));

    float specular = pow(sqrt(1. - abs(dot(tangent_N, h_N))), HAIR_SHININESS);
    return (diffuseIntensity * diffuse + specIntensity * specular) * color;
}

vec4 hairLighting(in vec4 position_ES, in vec3 tangent_ES)
{
    vec4 position_lightSpace = eyeToLight * position_ES;

    vec4 color;
    color.w = HAIR_OPACITY;

    // Key light
    color.xyz = colorContribution(position_ES, tangent_ES, (view * vec4(lightPosition, 1.)));
    color.xyz *= getHairTransmittance(position_lightSpace);
    color.xyz *= getMeshVisibility(position_lightSpace);

    // Fill light
    color.xyz += FILL_LIGHT_INTENSITY_HAIR * colorContribution(position_ES, tangent_ES, view * FILL_LIGHT_POS);

    return color;
}
