#ifndef HAIRDEPTHPEELPROGRAM_H
#define HAIRDEPTHPEELPROGRAM_H

#include "hairshaderprogram.h"
#include "resourceloader.h"

class HairDepthPeelShaderProgram : public HairShaderProgram
{
public:
    virtual GLuint createShaderProgram() override
    {
        return ResourceLoader::createFullShaderProgram(
                    ":/shaders/hair.vert", ":/shaders/hairDepthPeel.frag", ":/shaders/hair.geom",
                    ":/shaders/hair.tcs", ":/shaders/hair.tes");
    }
};

#endif // HAIRDEPTHPEELPROGRAM_H
