#version 400 core

in vec4 position_v;
in vec4 normal_v;

uniform vec3 lightPosition;

out vec3 fragColor;

void main(){
    vec4 toLight = vec4(lightPosition, 1.0) - position_v;
    float diffuse = dot(normalize(toLight), normalize(normal_v));
    fragColor = vec3(0.2 + 0.3 * diffuse);
}
