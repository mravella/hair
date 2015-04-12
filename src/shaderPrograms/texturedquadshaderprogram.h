#ifndef TEXTUREDQUADSHADERPROGRAM_H
#define TEXTUREDQUADSHADERPROGRAM_H

#include "shaderprogram.h"

class TexturedQuadShaderProgram : public ShaderProgram
{
protected:
    virtual GLuint createShaderProgram() override;

};

#endif // TEXTUREDQUADSHADERPROGRAM_H
