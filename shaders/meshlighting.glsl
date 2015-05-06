#include "constants.glsl"
#include "opacitymapping.glsl"

uniform vec3 hairColor;
uniform vec3 lightPosition;
uniform mat4 view, eyeToLight;
uniform sampler2D hairGrowthMap;

vec3 meshColor;

vec3 colorContribution(in vec4 position_WS, in vec4 normal_WS, in vec4 lightPosition_WS)
{
    vec4 toLight = lightPosition_WS - position_WS;
    vec4 normal_N = normalize(normal_WS);
    float diffuse = max(0.f, dot(normalize(toLight), normal_N));
    return diffuse * MESH_DIFFUSE_INTENSITY * meshColor;
}

vec4 meshLighting(in vec4 position_WS, in vec4 normal_WS, in vec2 uv) {
    meshColor = mix(MESH_COLOR, MIN_COLOR*hairColor, texture(hairGrowthMap, uv).r);

    vec4 position_lightSpace = eyeToLight * view * position_WS;

    vec4 color;
    color.w = 1.0;

    // Key light
    color.xyz = colorContribution(position_WS, normal_WS, vec4(lightPosition, 1.0));
    color.xyz *= getHairTransmittance(position_lightSpace);
    color.xyz *= getMeshVisibility(position_lightSpace);

    // Fill light
    color.xyz += FILL_LIGHT_INTENSITY_MESH * colorContribution(position_WS, normal_WS, FILL_LIGHT_POS);

    // Ambient light
    color.xyz += MESH_AMBIENT_INTENSITY * meshColor;

    return color;
}
