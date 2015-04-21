#ifndef HAIRKBUFFERPROGRAM_H
#define HAIRKBUFFERPROGRAM_H

#include "hairshaderprogram.h"
#include "resourceloader.h"

class HairKBufferProgram : public HairShaderProgram
{
public:
    void setGlobalUniforms() override
    {
        setUniformMatrix4f("projection", uniforms.projection);
        setUniformMatrix4f("view", uniforms.view);
    }

protected:
    GLuint createShaderProgram() override
    {
        return ResourceLoader::createFullShaderProgram(
                    ":/shaders/hair.vert", ":/shaders/kbuffer.frag", ":/shaders/hair.geom",
                    ":/shaders/hair.tcs", ":/shaders/hair.tes");
    }
};

#endif // HAIRKBUFFERPROGRAM_H
