#include "shaderprogram.h"

#include "resourceloader.h"
#include <vector>

ShaderProgram::ShaderProgram()
{
    // Default uniform values.
    uniforms.numGroupHairs = 1;
    uniforms.hairGroupWidth = 1.0f;
    uniforms.hairRadius = 100.0f;
    uniforms.color = glm::vec3(.6f, .4f, .3f);
}

void ShaderProgram::create()
{
    m_id = ResourceLoader::createFullShaderProgram(
                ":/shaders/full.vert", ":/shaders/full.frag", ":/shaders/full.geom",
                ":/shaders/full.tcs", ":/shaders/full.tes");

    std::vector<GLchar const *> uniformNames;
    uniformNames.push_back("model");
    uniformNames.push_back("view");
    uniformNames.push_back("projection");
    uniformNames.push_back("numPatchHairs");
    uniformNames.push_back("numHairSegments");
    uniformNames.push_back("numSplineVertices");
    uniformNames.push_back("vertexData");
    uniformNames.push_back("groupWidth");
    uniformNames.push_back("hairRadius");
    uniformNames.push_back("color");

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

void ShaderProgram::setUniforms()
{
    glUniformMatrix4fv(m_uniformLocs["model"], 1, GL_FALSE, glm::value_ptr(uniforms.model));
    glUniformMatrix4fv(m_uniformLocs["view"], 1, GL_FALSE, glm::value_ptr(uniforms.view));
    glUniformMatrix4fv(m_uniformLocs["projection"], 1, GL_FALSE, glm::value_ptr(uniforms.projection));
    glUniform1i(m_uniformLocs["numPatchHairs"], uniforms.numGroupHairs);
    glUniform1i(m_uniformLocs["numHairSegments"], uniforms.numHairVertices-1);
    glUniform1i(m_uniformLocs["numSplineVertices"], uniforms.numSplineVertices);
    glUniform3fv(m_uniformLocs["vertexData"], uniforms.numHairVertices, &uniforms.vertexData[0][0]);
    glUniform1f(m_uniformLocs["groupWidth"], uniforms.hairGroupWidth);
    glUniform1f(m_uniformLocs["hairRadius"], 0.005);
    glUniform3fv(m_uniformLocs["color"], 1, glm::value_ptr(uniforms.color));
}
