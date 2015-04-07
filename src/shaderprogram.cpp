#include "shaderprogram.h"

#include "resourceloader.h"
#include "errorchecker.h"
#include <vector>

ShaderProgram::ShaderProgram()
{
    // Default uniform values.
    uniforms.numGroupHairs = 1;
    uniforms.hairGroupSpread = 0.15f;
    uniforms.hairRadius = 0.004f;
    uniforms.taperExponent = 5.0;
    uniforms.noiseAmplitude = 0;
    uniforms.color = glm::vec3(.6f, .4f, .3f);
}

void ShaderProgram::create()
{
    m_id = ResourceLoader::createFullShaderProgram(
                ":/shaders/full.vert", ":/shaders/full.frag", ":/shaders/full.geom",
                ":/shaders/full.tcs", ":/shaders/full.tes");

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

    for (unsigned int i = 0; i < uniformNames.size(); i++)
        m_uniformLocs[uniformNames[i]] = glGetUniformLocation(m_id, uniformNames[i]);
}

void ShaderProgram::bind()
{
    glUseProgram(m_id);
}

void ShaderProgram::unbind()
{
    glUseProgram(0);
}

void ShaderProgram::setGlobalUniforms()
{
    glUniformMatrix4fv(m_uniformLocs["view"], 1, GL_FALSE, glm::value_ptr(uniforms.view));
    glUniformMatrix4fv(m_uniformLocs["projection"], 1, GL_FALSE, glm::value_ptr(uniforms.projection));
}

void ShaderProgram::setPerHairObjectUniforms()
{
    glUniformMatrix4fv(m_uniformLocs["model"], 1, GL_FALSE, glm::value_ptr(uniforms.model));
    glUniform1i(m_uniformLocs["numPatchHairs"], uniforms.numGroupHairs);
    glUniform1i(m_uniformLocs["numSplineVertices"], uniforms.numSplineVertices);
    glUniform1f(m_uniformLocs["hairGroupSpread"], uniforms.hairGroupSpread);
    glUniform1f(m_uniformLocs["hairRadius"], uniforms.hairRadius);
    glUniform1f(m_uniformLocs["taperExponent"], uniforms.taperExponent);
    glUniform1f(m_uniformLocs["noiseAmplitude"], uniforms.noiseAmplitude);
    glUniform3fv(m_uniformLocs["color"], 1, glm::value_ptr(uniforms.color));
}

void ShaderProgram::setPerGuideHairUniforms()
{
    glUniform1i(m_uniformLocs["numHairSegments"], uniforms.numHairVertices-1);
    glUniform3fv(m_uniformLocs["vertexData"], uniforms.numHairVertices, &uniforms.vertexData[0][0]);
    glUniform3fv(m_uniformLocs["triangleFace"], 2, &uniforms.triangleFace[0][0]);
}
