#version 400 core

#include "constants.glsl"

uniform sampler2D shadowMap;
uniform mat4 projection;
uniform float occlusionLayerSize;

in vec4 position_g;

out vec4 opacityMap;

#define A 0.01

void main(){

    vec4 shadowCoord = projection * position_g;
    float w = shadowCoord.w;
    shadowCoord = (shadowCoord / shadowCoord.w + 1.0) / 2.0;

    float shadowMapDepth = texture(shadowMap, shadowCoord.xy).r - .0001;
    float currDepth = shadowCoord.z;

    opacityMap = vec4(0.0);
    if (currDepth < shadowMapDepth + (1) * OPACITY_MAP_LAYER_SIZE)
        opacityMap.r = A;
    else if (currDepth < shadowMapDepth + (1 + 2) * OPACITY_MAP_LAYER_SIZE)
        opacityMap.g = A;
    else if (currDepth < shadowMapDepth + (1 + 2 + 4) * OPACITY_MAP_LAYER_SIZE)
        opacityMap.b = A;
    else
        opacityMap.a = A;
}
