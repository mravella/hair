#version 400 core

in vec4 position_g;
in vec3 tangent_g;

uniform mat4 model, view, projection;

uniform sampler2D tex1, tex2;

layout(location = 0) out vec4 frag1;
layout(location = 1) out vec4 frag2; // vec4(tangent.x, tangent.y, tangent.z, depth)

void main()
{
    vec4 clipCoord = projection * position_g;
    clipCoord = (clipCoord / clipCoord.w + 1.0) / 2.0;

    vec2 uv = clipCoord.xy;
    float currDepth = clipCoord.z;

    vec4 currValues[2];
    currValues[0] = texture(tex1, uv);
    currValues[1] = texture(tex2, uv);

    int maxIndex = 0;
    float maxDepth = currValues[0].a;

    for (int i = 1; i < currValues.length(); i++)
    {
        float updateMax = step(maxDepth, currValues[i].a);
        maxDepth += updateMax * (currValues[i].a - maxDepth);
        maxIndex += int(updateMax) * (i - maxIndex);
    }

    currValues[maxIndex] = mix(currValues[maxIndex], vec4(normalize(tangent_g), currDepth),
                               step(currDepth, maxDepth));

    frag1 = currValues[0];
    frag2 = currValues[1];
}
