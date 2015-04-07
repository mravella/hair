#version 400 core

uniform vec3 color;
uniform mat4 view;

in vec4 position_g;
in vec3 tangent_g;

out vec3 fragColor;

const vec4 lightPosition = vec4(2., 1., 2., 1.);

void main(){
    
    vec4 toLight_N = normalize((view * lightPosition) - position_g);
    vec3 tangent_N = normalize(tangent_g);
    
    float diffuse = sqrt(1. - abs(dot(tangent_N, toLight_N.xyz)));

    vec3 toEye_N = normalize(-position_g.xyz);
    vec3 h_N = normalize(toEye_N + toLight_N.xyz);
    
    
    float specular = pow(sqrt(1. - abs(dot(tangent_N, h_N))), 40.);
    
    
    fragColor =  color * vec3(.5 * diffuse + specular);
    
}
