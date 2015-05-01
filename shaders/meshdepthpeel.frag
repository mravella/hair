#version 400 core

#include "meshlighting.glsl"
#include "depthpeel.glsl"

in vec4 position_v;
in vec4 normal_v;
in vec2 uv_v;

uniform mat4 projection;

out vec4 fragColor;

void main(){
    fragColor = meshLighting(position_v, normal_v, uv_v);
    depthPeel(fragColor, projection * view * position_v);
}
