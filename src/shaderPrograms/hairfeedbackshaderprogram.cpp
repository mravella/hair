#include "hairfeedbackshaderprogram.h"

#include "resourceloader.h"

GLuint HairFeedbackShaderProgram::createShaderProgram()
{
    const GLchar* varyings[] = {"WS_position", "WS_tangent", "tessx"};
    return ResourceLoader::createTessFeedbackShaderProgram(
                ":/shaders/hair.vert",
                ":/shaders/hair.tcs",
                ":/shaders/hair.tes",
                varyings, 3);
}
