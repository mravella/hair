#include "hairshaderprogram.h"

#include "resourceloader.h"

GLuint HairShaderProgram::createShaderProgram()
{
    return ResourceLoader::createFullShaderProgram(
                ":/shaders/full.vert", ":/shaders/full.frag", ":/shaders/full.geom",
                ":/shaders/full.tcs", ":/shaders/full.tes");
}

std::vector<GLchar const *> HairShaderProgram::getUniformNames()
{
    std::vector<GLchar const *> uniformNames {
        "model",
        "view",
        "projection",
        "numPatchHairs",
        "numHairSegments",
        "numSplineVertices",
        "vertexData",
        "hairGroupSpread",
        "hairRadius",
        "taperExponent",
        "noiseAmplitude",
        "triangleFace",
        "color"
    };
    return uniformNames;
}

void HairShaderProgram::setGlobalUniforms()
{
    setUniformMatrix4f("view", uniforms.view);
    setUniformMatrix4f("projection", uniforms.projection);
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
    setUniform3f("color", uniforms.color);
}

void HairShaderProgram::setPerDrawUniforms()
{
    setUniform1i("numHairSegments", uniforms.numHairVertices-1);
    setUniform3fv("vertexData", uniforms.numHairVertices, uniforms.vertexData);
    setUniform3fv("triangleFace", 2, uniforms.triangleFace);
}
