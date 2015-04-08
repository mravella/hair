#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

out vec2 uv_v;

void main() {
    uv_v = uv;
    gl_Position = vec4(position, 1.0);
}
