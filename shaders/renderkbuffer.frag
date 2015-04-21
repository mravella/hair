#version 400 core

#include "constants.glsl"
#include "opacitymapping.glsl"

in vec2 uv_v;

out vec3 fragColor;

uniform sampler2D tex1, tex2;
uniform vec3 color;
uniform mat4 projection, view, eyeToLight;
uniform vec3 lightPosition;

vec3 colorContribution(in vec4 position, in vec3 tangent, in vec4 lightPosition_ES)
{
    vec4 toLight_N = normalize(view * lightPosition_ES - position);
    vec3 tangent_N = normalize(tangent);
    vec3 toEye_N = normalize(-position.xyz);
    vec3 h_N = normalize(toEye_N + toLight_N.xyz);
    float diffuse = sqrt(1. - abs(dot(tangent_N, toLight_N.xyz)));

    float specular = pow(sqrt(1. - abs(dot(tangent_N, h_N))), HAIR_SHININESS);
    return (HAIR_DIFFUSE_INTENSITY * diffuse + HAIR_SPEC_INTENSITY * specular) * color;
}

void sort(inout vec4 fragments[2])
{
    vec4 frag1 = texture(tex1, uv_v);
    vec4 frag2 = texture(tex2, uv_v);

    fragments[0] = mix(frag1, frag2, step(frag2.a, frag1.a));
    fragments[1] = mix(frag2, frag1, step(frag2.a, frag1.a));
}

void main()
{
    vec4 fragments[2];
    sort(fragments);

    fragColor = vec3(0.0);
    for (int i = 0; i < fragments.length(); i++)
    {
        vec4 position_clipSpace = vec4(vec3(uv_v, fragments[i].a) * 2.0 - 1.0, 1.0);
        vec4 position_eyeSpace = inverse(projection) * position_clipSpace;
        position_eyeSpace /= position_eyeSpace.w;


        vec4 position_lightSpace = eyeToLight * position_eyeSpace;

//        // Key light
//        vec3 currFragColor = colorContribution(
//                    position_eyeSpace, fragments[i].rgb, (view * vec4(lightPosition, 1.)));
//        currFragColor *= getHairTransmittance(position_lightSpace);
//        currFragColor *= mix(0.2, 1.0, getMeshVisibility(position_lightSpace));

//        // Fill light
//        currFragColor += FILL_LIGHT_INTENSITY_HAIR *  colorContribution(
//                    position_eyeSpace, fragments[i].rgb, view * FILL_LIGHT_POS);

//        fragColor += HAIR_OPACITY * pow(1.0 - HAIR_OPACITY, float(i)) * currFragColor;

        fragColor = position_eyeSpace.xyz;
    }
}
