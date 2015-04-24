#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 tangent;
layout(location = 2) in float tessx;

out vec3 tangent_te;
out float tessx_te;

uniform mat4 view;

void main()
{
    gl_Position = view * vec4(position, 1.0);
    tangent_te = (view * vec4(tangent, 0.0)).xyz;

    tessx_te = tessx; // TODO pass this through transform feedback
}
