#version 400 core

in vec4 position_v;
in vec4 normal_v;

out vec3 fragColor;

uniform vec3 lightPosition;
uniform mat4 view, eyeToLight;
uniform sampler2D shadowMap;
uniform sampler2D opacityMap;
uniform float shadowIntensity;

const float occlusionLayerSize = 0.0015;

float currDepth;

// Samples all layers of the opacity map at the given UV coordinates. Returns a
// number corresponding to the amount of occlusion from other hair fragments.
float occlusionSample(vec2 uv)
{
    vec4 opacityMapValues = texture(opacityMap, uv);

    float occlusion = 0.; // Amount of occlusion from opacity map layers
    float layerSize = occlusionLayerSize; // Size of current layer
    float layerStart = texelFetch(shadowMap, ivec2(uv * textureSize(shadowMap, 0)), 0).r;

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
    currDepth = shadowCoord.z;

    vec2 size = textureSize(shadowMap, 0); // Size of texture (e.g. 1024, 1024)
    vec2 texelSize = vec2(1.) / size; // Size of texel (e.g. 1/1024, 1/1024)

    // Linearly interpolate between four samples of deep opacity map.
    vec2 f = fract(uv * size);
    float s1 = occlusionSample(uv + texelSize * vec2(0., 0.));
    float s2 = occlusionSample(uv + texelSize * vec2(0., 1.));
    float s3 = occlusionSample(uv + texelSize * vec2(1., 0.));
    float s4 = occlusionSample(uv + texelSize * vec2(1., 1.));
    float occlusion = mix( mix(s1, s2, f.y), mix(s3, s4, f.y), f.x );

    return exp(- shadowIntensity * occlusion);
}

void main(){
    vec4 toLight = vec4(lightPosition, 1.0) - position_v;
    float diffuse = dot(normalize(toLight), normalize(normal_v));
    fragColor = vec3(0.2 + 0.3 * diffuse);

    fragColor *= getTransmittance(eyeToLight * view * position_v);
}
