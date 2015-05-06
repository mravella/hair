#version 400 core

layout(lines) in;
layout(triangle_strip, max_vertices=4) out;

in vec3 tangent_te[]; // Per-vertex, eye-space tangent vector.
in float tessx_te[];
in float colorVariation_te[];

out vec4 position_g;
out vec3 tangent_g;
out float colorVariation_g;
out float tessx_g;

uniform mat4 projection;
uniform float hairRadius;
uniform float taperExponent;

void main() {
    for(int i = 0; i < gl_in.length(); i++)
    {
        // Eye-space vertex position.
        vec4 position = gl_in[i].gl_Position;

        // Cross tangent and eye vectors to obtain the offset direction for billboarding.
        vec3 offsetDir = cross(normalize(tangent_te[i]), normalize(position.xyz));
        vec4 offset = hairRadius * vec4(offsetDir, 0.0);

        // Taper hair so it is thinner at end.
        offset *= (1 - pow(tessx_te[i], taperExponent));

        tangent_g = tangent_te[i];
        colorVariation_g = colorVariation_te[i];
        tessx_g = tessx_te[i];
        
        position_g = (position + offset);
        gl_Position = projection * position_g;
        EmitVertex();
        
        position_g = (position - offset);
        gl_Position = projection * position_g;
        EmitVertex();
    }
}
