#version 400 core

#include "constants.glsl"
#include "opacitymapping.glsl"

in vec4 position_g;
in vec3 tangent_g;

out vec3 fragColor;

uniform vec3 color;
uniform float specIntensity, diffuseIntensity;
uniform mat4 view, eyeToLight;
uniform vec3 lightPosition;


vec3 colorContribution(vec4 lightPosition_ES)
{
    vec4 toLight_N = normalize(view * lightPosition_ES - position_g);
    vec3 tangent_N = normalize(tangent_g);
    vec3 toEye_N = normalize(-position_g.xyz);
    vec3 h_N = normalize(toEye_N + toLight_N.xyz);
    float diffuse = sqrt(1. - abs(dot(tangent_N, toLight_N.xyz)));

    float specular = pow(sqrt(1. - abs(dot(tangent_N, h_N))), HAIR_SHININESS);
    return (diffuseIntensity * diffuse + specIntensity * specular) * color;
}

void main()
{
    vec4 position_lightSpace = eyeToLight * position_g;

    // Key light
    fragColor = colorContribution((view * vec4(lightPosition, 1.)));
    fragColor *= getHairTransmittance(position_lightSpace);
    fragColor *= mix(0.2, 1.0, getMeshVisibility(position_lightSpace));

    // Fill light
    fragColor += FILL_LIGHT_INTENSITY_HAIR * colorContribution(view * FILL_LIGHT_POS);
}
