#ifndef HAIRRENDERSHADERPROGRAM_H
#define HAIRRENDERSHADERPROGRAM_H

#include "hairshaderprogram.h"
#include "hairCommon.h"
#include "resourceloader.h"

class HairRenderShaderProgram : public HairShaderProgram
{
protected:
    GLuint createShaderProgram() override
    {
        return ResourceLoader::createGeomShaderProgram(
                    ":/shaders/hairrender.vert", ":/shaders/hair.frag", ":/shaders/hair.geom");
    }
};

#endif // HAIRRENDERSHADERPROGRAM_H
