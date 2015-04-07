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

    virtual ~HairShaderProgram() { }

    virtual void create();

    // Sends all global uniforms to the shader program.
    virtual void setGlobalUniforms();

    // Sends all hair object-specific uniforms to the shader program.
    virtual void setPerHairObjectUniforms();

    // Sends all guide hair-specific uniforms to the shader program.
    virtual void setPerGuideHairUniforms();

};

#endif // HAIRSHADERPROGRAM_H
