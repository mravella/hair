#include "shaderprogram.h"

#include "resourceloader.h"
#include "errorchecker.h"
#include <vector>

void ShaderProgram::create()
{
    id = createShaderProgram();
    std::vector<const GLchar *> uniformNames = getUniformNames();
    for (unsigned int i = 0; i < uniformNames.size(); i++)
        m_uniformLocs[uniformNames[i]] = glGetUniformLocation(id, uniformNames[i]);
    m_created = true;
}

void ShaderProgram::bind()
{
    if (!m_created)
    {
        std::cout << "Tried to bind shader program before creating it" << std::endl;
        exit(1);
    }
    glUseProgram(id);
}

void ShaderProgram::unbind()
{
    glUseProgram(0);
}

void ShaderProgram::setUniform1i(const GLchar *name, int value)
{
    glUniform1i(m_uniformLocs[name], value);
}

void ShaderProgram::setUniform1f(const GLchar *name, float value)
{
    glUniform1f(m_uniformLocs[name], value);
}

void ShaderProgram::setUniform3f(const GLchar *name, glm::vec3 &value)
{
    glUniform3fv(m_uniformLocs[name], 1, glm::value_ptr(value));
}

void ShaderProgram::setUniform3fv(const GLchar *name, GLsizei count, glm::vec3 *values)
{
    glUniform3fv(m_uniformLocs[name], count, &values[0][0]);
}

void ShaderProgram::setUniformMatrix4f(const GLchar *name, glm::mat4 &value)
{
    glUniformMatrix4fv(m_uniformLocs[name], 1, GL_FALSE, glm::value_ptr(value));
}
