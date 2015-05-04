#version 400 core

layout(lines) in;
layout(triangle_strip, max_vertices=4) out;

in vec3 tangent_te[]; // Per-vertex, eye-space tangent vector.
in float tessx_te[];
in float colorVariation_te[];

// Transform feedback outputs
out vec3 position_g;
out vec3 tangent_g;
out float colorVariation_g;
out float offset_g;

uniform mat4 projection;
uniform float hairRadius;
uniform float taperExponent;

void main() {
    for(int i = 0; i < gl_in.length(); i++)
    {
        position_g = gl_in[i].gl_Position.xyz;
        tangent_g = tangent_te[i];
        colorVariation_g = colorVariation_te[i];
        offset_g = hairRadius * (1 - pow(tessx_te[i], taperExponent)); // Taper hair so it is thinner at end.
        EmitVertex();

        offset_g *= -1.0;
        EmitVertex();
    }
}
