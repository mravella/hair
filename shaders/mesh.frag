#version 400 core

in vec4 position_v;
in vec4 normal_v;

out vec3 fragColor;

uniform vec3 lightPosition;
uniform mat4 view, eyeToLight;
uniform sampler2D hairShadowMap;
uniform sampler2D meshShadowMap;
uniform sampler2D opacityMap;
uniform float shadowIntensity;
uniform float occlusionLayerSize;
uniform bool useShadows;

const vec3 MESH_COLOR = vec3(221.0, 211.0, 242.0) / 255.0;

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
    currDepth = shadowCoord.z - 0.001;

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

    ivec2 iUV = ivec2(size * uv);
    s1 = step(currDepth, texelFetch(meshShadowMap, iUV + offset.xx, 0).r);
    s2 = step(currDepth, texelFetch(meshShadowMap, iUV + offset.xy, 0).r);
    s3 = step(currDepth, texelFetch(meshShadowMap, iUV + offset.yx, 0).r);
    s4 = step(currDepth, texelFetch(meshShadowMap, iUV + offset.yy, 0).r);
    float meshVisibility = mix( mix(s1, s2, f.y), mix(s3, s4, f.y), f.x );

    float transmittance = exp(- shadowIntensity * occlusion) * mix(0.2, 1.0, meshVisibility);
    return mix(1.0, transmittance, useShadows);
}

void main(){
    vec4 toLight = vec4(lightPosition, 1.0) - position_v;
    float diffuse = dot(normalize(toLight), normalize(normal_v));
    fragColor = (0.2 + 0.3 * diffuse) * MESH_COLOR;

    fragColor *= getTransmittance(eyeToLight * view * position_v);
}
