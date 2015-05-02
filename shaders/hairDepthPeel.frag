#version 400 core

#include "hairlighting.glsl"
#include "depthpeel.glsl"

in vec4 position_g;
in vec3 tangent_g;
in float colorVariation_g;

uniform mat4 projection;

out vec4 fragColor;

void main()
{
    fragColor = hairLighting(position_g, tangent_g, colorVariation_g);
    depthPeel(fragColor, projection * position_g);
}
