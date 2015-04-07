#include "errorchecker.h"

#include "GL/glew.h"
#include <stdio.h>

void ErrorChecker::printGLErrors(std::string prefix)
{
    std::string errors[] = {"GL_INVALID_ENUM",
                            "GL_INVALID_VALUE",
                            "GL_INVALID_OPERATION",
                            "GL_STACK_OVERFLOW",
                            "GL_STACK_UNDERFLOW",
                            "GL_OUT_OF_MEMORY",
                            "GL_INVALID_FRAMEBUFFER_OPERATION",
                            "GL_CONTEXT_LOST"};

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("Error (%s): %s\n", prefix.c_str(), errors[err - GL_INVALID_ENUM].c_str());
    }
}
