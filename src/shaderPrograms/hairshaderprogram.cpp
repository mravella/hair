#include "hairshaderprogram.h"

#include "resourceloader.h"

GLuint HairShaderProgram::createShaderProgram()
{
    return ResourceLoader::createFullShaderProgram(
                ":/shaders/hair.vert", ":/shaders/hair.frag", ":/shaders/hair.geom",
                ":/shaders/hair.tcs", ":/shaders/hair.tes");
}

void HairShaderProgram::setGlobalUniforms()
{
    setUniformMatrix4f("view", uniforms.view);
    setUniformMatrix4f("projection", uniforms.projection);
    setUniformMatrix4f("eyeToLight", uniforms.eyeToLight);
    setUniform3f("lightPosition", uniforms.lightPosition);
    setUniform1i("hairShadowMap", uniforms.hairShadowMap);
    setUniform1i("meshShadowMap", uniforms.meshShadowMap);
    setUniform1i("opacityMap", uniforms.opacityMap);
    setUniform1i("noiseTexture", uniforms.noiseTexture);
    setUniform1f("shadowIntensity", uniforms.shadowIntensity);
    setUniform1i("useShadows", uniforms.useShadows);
}

void HairShaderProgram::setPerObjectUniforms()
{
    setUniformMatrix4f("model", uniforms.model);
    setUniform1i("numPatchHairs", uniforms.numGroupHairs);
    setUniform1i("numSplineVertices", uniforms.numSplineVertices);
    setUniform1f("hairGroupSpread", uniforms.hairGroupSpread);
    setUniform1f("hairRadius", uniforms.hairRadius);
    setUniform1f("taperExponent", uniforms.taperExponent);
    setUniform1f("noiseAmplitude", uniforms.noiseAmplitude);
    setUniform1f("noiseFrequency", uniforms.noiseFrequency);
    setUniform3f("color", uniforms.color);
}

void HairShaderProgram::setPerDrawUniforms()
{
    setUniform1i("numHairSegments", uniforms.numHairVertices-1);
    setUniform3fv("vertexData", uniforms.numHairVertices, uniforms.vertexData);
    setUniform3fv("triangleFace", 2, uniforms.triangleFace);
}
