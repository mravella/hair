#include "texturedquadshaderprogram.h"

#include "resourceloader.h"

GLuint TexturedQuadShaderProgram::createShaderProgram()
{
    return ResourceLoader::createBasicShaderProgram(
                ":/shaders/texturedquad.vert", ":/shaders/texturedquad.frag");
}
