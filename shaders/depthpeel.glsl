uniform sampler2D depthPeelMap;

/**
 * Checks if the point p (in clip-space) is behind the last depth value rendered.
 * If so, the color is unchanged. If not, the color is set to the background color
 * and gl_FragDepth is adjusted.
 */
void depthPeel(inout vec4 color, in vec4 p)
{
    vec4 clipCoord = (p / p.w + 1.0) / 2.0;
    vec2 uv = clipCoord.xy;
    float currDepth = clipCoord.z - 1e-4;
    float lastDepth = texelFetch(depthPeelMap, ivec2(uv * textureSize(depthPeelMap, 0)), 0).r;

    float discardFrag = step(currDepth, lastDepth); // 1.0 if fragment should be discarded, else 0.0

    color = mix(color, BACKGROUND_COLOR, discardFrag);
    gl_FragDepth = mix(gl_FragDepth, 1.0, discardFrag);
}
