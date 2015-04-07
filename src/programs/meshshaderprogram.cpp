#include "meshshaderprogram.h"

#include "resourceloader.h"
#include <vector>

GLuint MeshShaderProgram::createShaderProgram()
{
    return ResourceLoader::createBasicShaderProgram(":/shaders/basic.vert", ":/shaders/basic.frag");
}

std::vector<GLchar const *> MeshShaderProgram::getUniformNames()
{
    std::vector<GLchar const *> uniformNames {
        "model",
        "view",
        "projection"
    };
    return uniformNames;
}
void MeshShaderProgram::setGlobalUniforms()
{
    setUniformMatrix4f("projection", uniforms.projection);
    setUniformMatrix4f("view", uniforms.view);
}

void MeshShaderProgram::setPerObjectUniforms()
{
    setUniformMatrix4f("model", uniforms.model);
}
