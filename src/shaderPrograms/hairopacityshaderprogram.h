#ifndef HAIROPACITYSHADERPROGRAM_H
#define HAIROPACITYSHADERPROGRAM_H

#include "hairCommon.h"
#include "shaderprogram.h"

class HairOpacityShaderProgram : public ShaderProgram
{
public:
    HairOpacityShaderProgram() {
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

};

#endif // HAIROPACITYSHADERPROGRAM_H
