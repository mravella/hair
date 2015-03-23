#version 400 core

layout(triangles) in;
layout(triangle_strip, max_vertices=4) out;

out vec3 dist;

void main() {
    for(int i = 0; i < gl_in.length(); i++)
    {
      gl_Position = gl_in[i].gl_Position;
      EmitVertex();
    }
}
