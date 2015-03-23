#ifndef HAIRPATCH_H
#define HAIRPATCH_H

#include "openglshape.h"

class HairPatch
{
public:
    HairPatch();
    virtual ~HairPatch();

    void init();

    void draw();

private:
    OpenGLShape m_patch;
};

#endif // HAIRPATCH_H
