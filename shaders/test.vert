#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec4 position_g;
out vec3 tangent_g;

uniform mat4 model, view, projection;

void main()
{
    position_g = view * model * vec4(position, 1.0);
    tangent_g = color;

    gl_Position = projection * position_g;
}
