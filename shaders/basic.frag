#version 400 core

in vec4 position_v;
in vec4 normal_v;

uniform mat4 view;

const vec4 lightPosition = vec4(2., 1., 2., 1.);

out vec3 fragColor;

void main(){
    vec4 toLight = (view * lightPosition) - position_v;
    float diffuse = dot(normalize(toLight), normalize(normal_v));
    fragColor = vec3(0.2 + 0.3 * diffuse);
}
