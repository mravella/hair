#version 400 core

in vec2 uv_v;

uniform sampler2D tex;

out vec4 fragColor;

void main(){
    fragColor = texture(tex, uv_v);
}
