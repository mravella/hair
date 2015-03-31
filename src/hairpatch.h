#ifndef HAIRPATCH_H
#define HAIRPATCH_H

#include "openglshape.h"

class Hair;

class HairPatch
{
public:
    HairPatch();
    virtual ~HairPatch();

    void init();

    void draw();

    void testHairData(GLfloat *data, int numVertices, float time);

private:
    OpenGLShape m_patch;

};

#endif // HAIRPATCH_H
