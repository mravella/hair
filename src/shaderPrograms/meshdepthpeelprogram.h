#ifndef MESHDEPTHPEEL_H
#define MESHDEPTHPEEL_H

#include "meshshaderprogram.h"
#include "resourceloader.h"

class MeshDepthPeelShaderProgram : public MeshShaderProgram
{
public:
    virtual GLuint createShaderProgram() override
    {
        return ResourceLoader::createBasicShaderProgram(
                    ":/shaders/mesh.vert", ":/shaders/meshdepthpeel.frag");
    }
};

#endif // MESHDEPTHPEEL_H
