#include "texturedquadshaderprogram.h"

#include "resourceloader.h"

GLuint TexturedQuadShaderProgram::createShaderProgram()
{
    return ResourceLoader::createBasicShaderProgram(
                ":/shaders/texturedquad.vert", ":/shaders/texturedquad.frag");
}

std::vector<GLchar const *> TexturedQuadShaderProgram::getUniformNames()
{
    std::vector<GLchar const *> uniformNames;
    return uniformNames;
}
