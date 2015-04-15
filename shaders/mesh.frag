#version 400 core

#include "constants.glsl"
#include "opacitymapping.glsl"

in vec4 position_v;
in vec4 normal_v;

out vec3 fragColor;

uniform vec3 lightPosition;
uniform mat4 view, eyeToLight;

vec3 colorContribution(vec4 lightPosition_WS)
{
    vec4 toLight = lightPosition_WS - position_v;
    vec4 normal_N = normalize(normal_v);
    float diffuse = max(0.f, dot(normalize(toLight), normal_N));
    return (MESH_AMBIENT_INTENSITY + MESH_DIFFUSE_INTENSITY * diffuse) * MESH_COLOR;
}

void main(){
    // Key light
    fragColor = colorContribution(vec4(lightPosition, 1.0));
    fragColor *= getTransmittance(eyeToLight * view * position_v);

    // Fill light
    fragColor += FILL_LIGHT_INTENSITY * colorContribution(FILL_LIGHT_POS);
}
