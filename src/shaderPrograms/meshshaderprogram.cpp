#include "meshshaderprogram.h"

#include "resourceloader.h"
#include <vector>

GLuint MeshShaderProgram::createShaderProgram()
{
    return ResourceLoader::createBasicShaderProgram(":/shaders/mesh.vert", ":/shaders/mesh.frag");
}

std::vector<GLchar const *> MeshShaderProgram::getUniformNames()
{
    std::vector<GLchar const *> uniformNames {
        "model",
        "view",
        "projection",
        "lightPosition"
    };
    return uniformNames;
}
void MeshShaderProgram::setGlobalUniforms()
{
    setUniformMatrix4f("projection", uniforms.projection);
    setUniformMatrix4f("view", uniforms.view);
    setUniform3f("lightPosition", uniforms.lightPosition);
}

void MeshShaderProgram::setPerObjectUniforms()
{
    setUniformMatrix4f("model", uniforms.model);
}
