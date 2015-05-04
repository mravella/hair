#ifndef HAIRRENDERSHADERPROGRAM_H
#define HAIRRENDERSHADERPROGRAM_H

#include "hairshaderprogram.h"
#include "hairopacityshaderprogram.h"
#include "hairCommon.h"
#include "resourceloader.h"
#include "whitehairshaderprogram.h"
#include "hairdepthpeelprogram.h"

class HairRenderShaderProgram : public HairShaderProgram
{
protected:
    virtual GLuint createShaderProgram() override
    {
        return ResourceLoader::createBasicShaderProgram(
                    ":/shaders/hairrender.vert", ":/shaders/hair.frag");
    }
};

class HairFeedbackOpacityShaderProgram : public HairOpacityShaderProgram
{
protected:
    virtual GLuint createShaderProgram() override
    {
        return ResourceLoader::createBasicShaderProgram(
                    ":/shaders/hairrender.vert", ":/shaders/hairOpacity.frag");
    }
};

class HairFeedbackDepthPeelShaderProgram : public HairDepthPeelShaderProgram
{
    virtual GLuint createShaderProgram() override
    {
        return ResourceLoader::createBasicShaderProgram(
                    ":/shaders/hairrender.vert", ":/shaders/hairDepthPeel.frag");
    }
};

class WhiteHairFeedbackShaderProgram : public WhiteHairShaderProgram
{
    virtual GLuint createShaderProgram() override
    {
        return ResourceLoader::createBasicShaderProgram(
                    ":/shaders/hairrender.vert", ":/shaders/white.frag");
    }
};

#endif // HAIRRENDERSHADERPROGRAM_H
