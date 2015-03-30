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

    static void testHairData(GLfloat *data, int numVertices, float time);

private:
    OpenGLShape m_patch;

    GLuint m_fullProgram;
};

#endif // HAIRPATCH_H
