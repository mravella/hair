#version 400 core

layout(lines) in;
layout(triangle_strip, max_vertices=4) out;

in vec3 tangent_te[]; // Per-vertex, eye-space tangent vector.
in float tessx_te[];

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

        gl_Position = projection * (position + offset);
        EmitVertex();
        gl_Position = projection * (position - offset);
        EmitVertex();
    }
}
