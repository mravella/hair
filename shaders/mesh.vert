#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 position_v;
out vec4 normal_v;

void main() {
    position_v = model * vec4(position, 1.0);
    normal_v = model * vec4(normal, 0.0);
    gl_Position = projection * view * position_v;
}
