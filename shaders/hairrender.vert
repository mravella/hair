#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 tangent;
layout(location = 2) in float colorVariation;
layout(location = 3) in float tessx;

out vec4 position_g;
out vec3 tangent_g;
out float colorVariation_g;
out float tessx_g;

uniform mat4 projection, view;
uniform float taperExponent;
uniform float hairRadius;

void main()
{
    // Convert from world to eye space.
    vec4 position_ES = view * vec4(position, 1.0);
    vec3 tangent_ES = (view * vec4(tangent, 0.0)).xyz;

    // Offset position.
    vec3 offsetDir = cross(normalize(tangent_ES), normalize(position_ES.xyz));
    position_ES.xyz += sign(tessx) * hairRadius * (1.0 - pow(abs(tessx), taperExponent)) * offsetDir;
    gl_Position = projection * position_ES;

    // Send outputs to frag shader.
    position_g = position_ES;
    tangent_g = tangent_ES;
    colorVariation_g = colorVariation;
    tessx_g = abs(tessx);
}
