#ifndef HAIRSHADERPROGRAM_H
#define HAIRSHADERPROGRAM_H

#include "shaderprogram.h"

class HairShaderProgram : public ShaderProgram
{
public:
    HairShaderProgram() {
        // Default uniform values.
        uniforms.numGroupHairs = 1;
        uniforms.hairGroupSpread = 0.15f;
        uniforms.hairRadius = 0.004f;
        uniforms.taperExponent = 5.0;
        uniforms.noiseAmplitude = 0;
        uniforms.color = glm::vec3(.6f, .4f, .3f);
    }

    virtual void setGlobalUniforms() override;

    virtual void setPerObjectUniforms() override;

    virtual void setPerDrawUniforms() override;

protected:
    virtual GLuint createShaderProgram() override;

    virtual std::vector<GLchar const *> getUniformNames() override;

};

#endif // HAIRSHADERPROGRAM_H
