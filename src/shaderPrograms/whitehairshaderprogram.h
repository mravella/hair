#ifndef WHITEHAIRSHADERPROGRAM_H
#define WHITEHAIRSHADERPROGRAM_H

#include "hairshaderprogram.h"
#include "resourceloader.h"

class WhiteHairShaderProgram : public HairShaderProgram
{
protected:
    virtual GLuint createShaderProgram() override
    {
        return ResourceLoader::createFullShaderProgram(
                    ":/shaders/hair.vert", ":/shaders/white.frag", ":/shaders/hair.geom",
                    ":/shaders/hair.tcs", ":/shaders/hair.tes");
    }
};

#endif // WHITEHAIRSHADERPROGRAM_H
