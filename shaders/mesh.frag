#version 400 core

#include "constants.glsl"
#include "opacitymapping.glsl"

in vec4 position_v;
in vec4 normal_v;
in vec2 uv_v;

out vec3 fragColor;

uniform vec3 hairColor;
uniform vec3 lightPosition;
uniform mat4 view, eyeToLight;
uniform sampler2D hairGrowthMap;

vec3 meshColor;

vec3 colorContribution(vec4 lightPosition_WS)
{
    vec4 toLight = lightPosition_WS - position_v;
    vec4 normal_N = normalize(normal_v);
    float diffuse = max(0.f, dot(normalize(toLight), normal_N));
    return diffuse * MESH_DIFFUSE_INTENSITY * meshColor;
}

void main(){
    meshColor = mix(MESH_COLOR, hairColor, texture(hairGrowthMap, uv_v).r);

    vec4 position_lightSpace = eyeToLight * view * position_v;

    // Key light
    fragColor = colorContribution(vec4(lightPosition, 1.0));
    fragColor *= getHairTransmittance(position_lightSpace);
    fragColor *= getMeshVisibility(position_lightSpace);

    // Fill light
    fragColor += FILL_LIGHT_INTENSITY_MESH * colorContribution(FILL_LIGHT_POS);

    // Ambient light
    fragColor += MESH_AMBIENT_INTENSITY * meshColor;
}
