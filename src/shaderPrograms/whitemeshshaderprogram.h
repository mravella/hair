#ifndef WHITEMESHSHADERPROGRAM_H
#define WHITEMESHSHADERPROGRAM_H

#include "meshshaderprogram.h"
#include "resourceloader.h"

class WhiteMeshShaderProgram : public MeshShaderProgram
{
protected:
    GLuint createShaderProgram() override
    {
        return ResourceLoader::createBasicShaderProgram(":/shaders/mesh.vert", ":/shaders/white.frag");
    }
};

#endif // WHITEMESHSHADERPROGRAM_H
