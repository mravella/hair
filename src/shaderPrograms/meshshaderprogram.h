#ifndef MESHSHADERPROGRAM_H
#define MESHSHADERPROGRAM_H

#include "shaderprogram.h"

class MeshShaderProgram : public ShaderProgram
{
public:
    MeshShaderProgram() { }

    virtual ~MeshShaderProgram() { }

    virtual void setGlobalUniforms() override;

    virtual void setPerObjectUniforms() override;

protected:
    virtual GLuint createShaderProgram() override;

    virtual std::vector<GLchar const *> getUniformNames() override;

};

#endif // MESHSHADERPROGRAM_H
