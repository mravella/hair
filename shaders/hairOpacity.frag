#version 400 core

uniform sampler2D shadowMap;
uniform mat4 projection;

in vec4 position_g;

const float c = 0.0015;

out vec4 opacityMap;

#define A 0.01

void main(){

    vec4 shadowCoord = projection * position_g;
    float w = shadowCoord.w;
    shadowCoord = (shadowCoord / shadowCoord.w + 1.0) / 2.0;

    float shadowMapDepth = texture(shadowMap, shadowCoord.xy).r - .0001;
    float currDepth = shadowCoord.z;

    opacityMap = vec4(0.0);
    if (currDepth < shadowMapDepth + (1) * c)
        opacityMap.r = A;
    else if (currDepth < shadowMapDepth + (1 + 2) * c)
        opacityMap.g = A;
    else if (currDepth < shadowMapDepth + (1 + 2 + 4) * c)
        opacityMap.b = A;
    else
        opacityMap.a = A;
}
