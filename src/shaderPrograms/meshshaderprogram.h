#ifndef MESHSHADERPROGRAM_H
#define MESHSHADERPROGRAM_H

#include "shaderprogram.h"

class MeshShaderProgram : public ShaderProgram
{
public:
    virtual void setGlobalUniforms() override;

    virtual void setPerObjectUniforms() override;

protected:
    virtual GLuint createShaderProgram() override;

};

#endif // MESHSHADERPROGRAM_H
