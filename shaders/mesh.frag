#version 400 core

#include "constants.glsl"

in vec4 position_v;
in vec4 normal_v;
in vec4 OS_position;

out vec3 fragColor;

uniform vec3 lightPosition;
uniform mat4 view, eyeToLight;
uniform sampler2D hairShadowMap;
uniform sampler2DShadow meshShadowMap;
uniform sampler2D opacityMap;
uniform float shadowIntensity;
uniform float occlusionLayerSize;
uniform bool useShadows;

float currDepth;

// Samples all layers of the opacity map at the given UV coordinates. Returns a
// number corresponding to the amount of occlusion from other hair fragments.
float occlusionSample(vec2 uv)
{
    vec4 opacityMapValues = texture(opacityMap, uv);

    float occlusion = 0.; // Amount of occlusion from opacity map layers
    float layerSize = occlusionLayerSize; // Size of current layer
    float layerStart = texelFetch(hairShadowMap, ivec2(uv * textureSize(hairShadowMap, 0)), 0).r;

    for (int layer = 0; layer < 4; layer++)
    {
        float t = clamp((currDepth - layerStart) / layerSize, 0.0, 1.0);
        occlusion += mix(0, opacityMapValues[layer], t);

        layerStart += layerSize;
        layerSize *= 2.0;
    }
    return occlusion;
}

// Calculates the transmittance of the light source to point p based on the deep
// opacity map, where p is in the light source's clip space.
float getTransmittance(vec4 p)
{
    vec4 shadowCoord = (p / p.w + 1.0) / 2.0;
    vec2 uv = shadowCoord.xy;
    currDepth = shadowCoord.z - 0.0001;

    vec2 size = textureSize(hairShadowMap, 0); // Size of texture (e.g. 1024, 1024)
    vec2 texelSize = vec2(1.) / size; // Size of texel (e.g. 1/1024, 1/1024)

    ivec2 offset = ivec2(0, 1);

    // Linearly interpolate between four samples of deep opacity map.
    vec2 f = fract(uv * size);
    float s1 = occlusionSample(uv + texelSize * offset.xx);
    float s2 = occlusionSample(uv + texelSize * offset.xy);
    float s3 = occlusionSample(uv + texelSize * offset.yx);
    float s4 = occlusionSample(uv + texelSize * offset.yy);
    float occlusion = mix( mix(s1, s2, f.y), mix(s3, s4, f.y), f.x );

    shadowCoord.z -= 0.0005;
    float meshVisibility = texture(meshShadowMap, shadowCoord.xyz);

    float transmittance = exp(- shadowIntensity * occlusion) * mix(0.5, 1.0, meshVisibility);
    return mix(1.0, transmittance, useShadows);
}

vec3 colorContribution(vec4 lightPosition_WS)
{
    vec4 toLight = lightPosition_WS - position_v;
    vec4 normal_N = normalize(normal_v);
    float diffuse = max(0.f, dot(normalize(toLight), normal_N));
    return (MESH_AMBIENT_INTENSITY + MESH_DIFFUSE_INTENSITY * diffuse) * MESH_COLOR;
}

void main(){
    // Key light
    fragColor = colorContribution(vec4(lightPosition, 1.0));
    fragColor *= getTransmittance(eyeToLight * view * position_v);

    // Fill light
    fragColor += FILL_LIGHT_INTENSITY * colorContribution(FILL_LIGHT_POS);
}
